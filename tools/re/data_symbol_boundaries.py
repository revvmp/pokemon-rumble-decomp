import argparse
import hashlib
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

from data_symbol_mappings import LinkedSymbols, attach_owners, original_names, relocations, section_name, source_address, target_address, verify_linked_data
from map_exception_symbols import Elf


DATA_SECTIONS = {'.data', '.sdata', '.rodata', '.sdata2'}


def fingerprint(data):
    return hashlib.sha256(data).hexdigest()


def configured_symbols(text):
    result = defaultdict(list)
    for line in text.splitlines():
        match = re.match(r'^(\S+)\s*=\s*([^:]+):(0x[0-9A-Fa-f]+);.*\bsize:(0x[0-9A-Fa-f]+)', line)
        if match:
            result[int(match[3], 16)].append({
                'name': match[1],
                'section': match[2],
                'size': int(match[4], 16),
                'line': line,
            })
    return result


def retained_candidates(source, linked):
    result = defaultdict(list)
    counts = Counter(symbol['name'] for symbol in source.symbols)
    attach_owners(source)
    for symbol in source.symbols:
        section = section_name(source, symbol)
        if symbol['kind'] != 1 or not symbol['size'] or section not in DATA_SECTIONS:
            continue
        if counts[symbol['name']] != 1:
            continue
        final = linked.lookup(symbol)
        if final is None or final['size'] != symbol['size'] or section_name(linked.elf, final) != section:
            continue
        result[section].append((final['value'], symbol))
    return result


def empty_interval(target, symbol, start, end):
    if start > end or any(target.body(symbol)[start:end]):
        return False
    low = symbol['value'] + start
    high = symbol['value'] + end
    return not any(low <= offset < high for offset in target.relocations.get(symbol['section'], {}))


def pooled_string_candidates(source, original, address, extent):
    if len(extent) < 2:
        return extent
    deltas = {final_address - symbol['value'] for final_address, symbol in extent}
    sections = {symbol['section'] for final_address, symbol in extent}
    if len(deltas) != 1 or len(sections) != 1:
        return extent
    delta = next(iter(deltas))
    section = next(iter(sections))
    names = {symbol['name'] for final_address, symbol in extent}
    counts = Counter(symbol['name'] for symbol in source.symbols)
    anchors = [f'0x{final_address:08X}' for final_address, symbol in extent]
    result = list(extent)
    for symbol in source.symbols:
        if symbol['kind'] != 1 or symbol['binding'] != 2 or symbol['section'] != section:
            continue
        if not symbol['name'].startswith('@STRING@') or symbol['name'] in names or counts[symbol['name']] != 1:
            continue
        final_address = delta + symbol['value']
        if not address <= final_address < address + original['size']:
            continue
        body = source.body(symbol)
        if len(body) < 2 or body[-1] != 0 or any(byte < 32 or byte > 126 for byte in body[:-1]):
            continue
        if any(symbol['value'] <= offset < symbol['value'] + symbol['size'] for offset in source.relocations.get(section, {})):
            continue
        recovered = dict(symbol, address_evidence={'kind': 'pooled_weak_string', 'retained_anchors': anchors, 'section_delta': delta})
        result.append((final_address, recovered))
    return sorted(result, key=lambda item: item[0])


def prove_object(target, original, source, candidate, offset, origins, names, linked):
    if offset < 0 or offset + candidate['size'] > original['size']:
        return None
    part = dict(original, value=original['value'] + offset, size=candidate['size'])
    if target.body(part) != source.body(candidate):
        return None
    expected = relocations(target, part, lambda item: target_address(target, item, origins, names))
    actual = relocations(source, candidate, lambda item: source_address(item, linked))
    if expected is None or actual is None or expected != actual:
        return None
    return {
        'source_symbol': candidate['name'],
        'source_owner': candidate['owner'],
        'size': candidate['size'],
        'raw_sha256': fingerprint(source.body(candidate)),
        'normalized_relocations': expected,
        'address_evidence': candidate.get('address_evidence', {'kind': 'retained_final_symbol'}),
    }


def inspect_unit(unit, report, root, linked, names, configured):
    target = Elf((root / unit['target_path']).resolve())
    source = Elf((root / unit['base_path']).resolve())
    origins = {
        section['name']: int(section['metadata']['virtual_address'])
        for section in report['sections']
        if 'virtual_address' in section.get('metadata', {})
    }
    candidates = retained_candidates(source, linked)
    boundaries = []
    partitions = []
    rejected = Counter()
    for original in target.symbols:
        section = section_name(target, original)
        if original['kind'] != 1 or section not in DATA_SECTIONS or original['name'].startswith('gap_'):
            continue
        if section not in origins:
            rejected['missing_section_origin'] += 1
            continue
        address = origins[section] + original['value']
        lines = configured[address]
        if len(lines) != 1:
            rejected['ambiguous_configured_address'] += 1
            continue
        configured_symbol = lines[0]
        if configured_symbol['name'] != original['name'] or configured_symbol['size'] != original['size'] or configured_symbol['section'] != section:
            rejected['target_annotation_changed_since_split'] += 1
            continue
        extent = [item for item in candidates[section] if address <= item[0] < address + original['size']]
        extent.sort(key=lambda item: item[0])
        extent = pooled_string_candidates(source, original, address, extent)
        if not extent or extent[0][0] != address or len({item[0] for item in extent}) != len(extent):
            rejected['no_unique_retained_partition_start'] += 1
            continue
        if len(extent) == 1:
            candidate = extent[0][1]
            if candidate['size'] >= original['size']:
                continue
            proof = prove_object(target, original, source, candidate, 0, origins, names, linked)
            if proof is None or not empty_interval(target, original, candidate['size'], original['size']):
                rejected['boundary_content_or_relocations'] += 1
                continue
            new = re.sub(r'size:0x[0-9A-Fa-f]+', f"size:0x{candidate['size']:X}", configured_symbol['line'])
            boundaries.append({
                'address': f'0x{address:08X}',
                'section': section,
                'old': configured_symbol['line'],
                'new': new,
                'preserved_zero_tail_bytes': original['size'] - candidate['size'],
                **proof,
            })
            continue
        objects = []
        previous = 0
        for final_address, candidate in extent:
            offset = final_address - address
            if not empty_interval(target, original, previous, offset):
                break
            if offset and configured[final_address]:
                break
            proof = prove_object(target, original, source, candidate, offset, origins, names, linked)
            if proof is None:
                break
            target_name = original['name'] if offset == 0 else f'lbl_{final_address:08X}'
            if offset == 0:
                line = re.sub(r'size:0x[0-9A-Fa-f]+', f"size:0x{candidate['size']:X}", configured_symbol['line'])
            else:
                line = f"{target_name} = {section}:0x{final_address:08X}; // type:object size:0x{candidate['size']:X} scope:local"
            objects.append({
                'address': f'0x{final_address:08X}',
                'new_line': line,
                'target_symbol': target_name,
                **proof,
            })
            previous = offset + candidate['size']
        if len(objects) != len(extent) or not empty_interval(target, original, previous, original['size']):
            rejected['partition_content_ownership_or_relocations'] += 1
            continue
        partitions.append({
            'address': f'0x{address:08X}',
            'section': section,
            'old': configured_symbol['line'],
            'size': original['size'],
            'symbols': objects,
            'preserved_zero_gap_bytes': original['size'] - sum(item['size'] for item in objects),
        })
    return {
        'unit': unit['name'],
        'target_sha256': fingerprint(target.data),
        'source_sha256': fingerprint(source.data),
        'boundaries': boundaries,
        'partitions': partitions,
        'rejected': dict(rejected),
    }, [(target.path, target.data), (source.path, source.data)]


def main():
    parser = argparse.ArgumentParser(description='Audit real data-symbol boundaries and complete merged-symbol partitions without editing build inputs.')
    parser.add_argument('--project', type=Path, default=Path('.'))
    parser.add_argument('--unit', action='append')
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    root = args.project.resolve()
    paths = [root / 'objdiff.json', root / 'build/wpse-v1/report.json', root / 'config/wpse-v1/symbols.txt']
    snapshots = [(path, path.read_bytes()) for path in paths]
    snapshots.extend((path, path.read_bytes()) for path in (
        root / 'build/wpse-v1/main.dol',
        root / 'orig/wpse-v1/main.dol',
        root / 'config/targets/wpse-v1.json',
    ))
    config = json.loads(snapshots[0][1])
    reports = {unit['name']: unit for unit in json.loads(snapshots[1][1])['units']}
    configured = configured_symbols(snapshots[2][1].decode('utf-8'))
    names = original_names(paths[2])
    elf = Elf(root / 'build/wpse-v1/main.elf', allowed_types=(2,))
    snapshots.append((elf.path, elf.data))
    dol_hash = verify_linked_data(root, elf, DATA_SECTIONS)
    linked = LinkedSymbols(elf)
    wanted = set(args.unit or [unit['name'] for unit in config['units'] if unit['name'].startswith('main/game/')])
    units = [unit for unit in config['units'] if unit['name'] in wanted]
    if {unit['name'] for unit in units} != wanted:
        raise ValueError('Unknown requested unit')
    audits = []
    for unit in units:
        audit, inputs = inspect_unit(unit, reports[unit['name']], root, linked, names, configured)
        audits.append(audit)
        snapshots.extend(inputs)
    output = args.output.resolve()
    if output.suffix.lower() != '.json' or not any(output.is_relative_to(directory) for directory in (root / 'build', root / 'tools/local')):
        raise ValueError('Audit output must be a JSON file under build/ or tools/local/')
    if output in {path for path, data in snapshots}:
        raise ValueError('Output collides with an audit input')
    if output.exists() and any(output.samefile(path) for path, data in snapshots):
        raise ValueError('Output aliases an audit input')
    for path, data in snapshots:
        if path.read_bytes() != data:
            raise ValueError(f'Input changed during audit: {path}')
    result = {
        'schema_version': 1,
        'main_dol_sha256': dol_hash,
        'main_elf_sha256': fingerprint(elf.data),
        'boundary_count': sum(len(unit['boundaries']) for unit in audits),
        'partition_count': sum(len(unit['partitions']) for unit in audits),
        'units': audits,
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print(f"Audited {len(audits)} units: {result['boundary_count']} boundaries, {result['partition_count']} partitions")


if __name__ == '__main__':
    main()
