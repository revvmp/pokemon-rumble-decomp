import argparse
import hashlib
import json
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

from audit_alignment import dol_ranges, range_bytes
from map_exception_symbols import Elf


def digest(data):
    return hashlib.sha256(data).hexdigest()


def attach_owners(elf):
    owner = None
    for symbol in elf.symbols:
        if symbol['kind'] == 4:
            owner = symbol['name']
        symbol['owner'] = owner


def section_name(elf, symbol):
    index = symbol['section']
    return elf.sections[index]['name'] if 0 < index < len(elf.sections) else None


class LinkedSymbols:
    def __init__(self, elf):
        self.elf = elf
        attach_owners(elf)
        self.globals = defaultdict(list)
        self.locals = defaultdict(list)
        for symbol in elf.symbols:
            if symbol['kind'] in (3, 4) or not section_name(elf, symbol):
                continue
            if symbol['binding']:
                self.globals[symbol['name']].append(symbol)
            else:
                self.locals[(symbol['owner'], symbol['name'])].append(symbol)

    def lookup(self, symbol):
        if symbol['binding']:
            candidates = self.globals[symbol['name']]
        else:
            candidates = self.locals[(symbol['owner'], symbol['name'])]
        return candidates[0] if len(candidates) == 1 else None


def original_names(path):
    addresses = defaultdict(set)
    for line in path.read_text(encoding='utf-8').splitlines():
        match = re.match(r'^(\S+)\s*=\s*[^:]+:(0x[0-9A-Fa-f]+);', line)
        if match:
            addresses[match[1]].add(int(match[2], 16))
    return {name: next(iter(values)) for name, values in addresses.items() if len(values) == 1}


def target_address(elf, symbol, origins, names):
    section = section_name(elf, symbol)
    if section in origins:
        return origins[section] + symbol['value']
    if symbol['section'] == 0xFFF1:
        return symbol['value']
    if symbol['section'] == 0:
        return names.get(symbol['name'])
    return None


def source_address(symbol, linked):
    if symbol['section'] == 0xFFF1:
        return symbol['value']
    match = linked.lookup(symbol)
    return match['value'] if match is not None else None


def relocations(elf, symbol, resolve):
    result = []
    start = symbol['value']
    for offset, (target, kind, addend) in sorted(elf.relocations.get(symbol['section'], {}).items()):
        if not start <= offset < start + symbol['size']:
            continue
        address = resolve(target)
        if address is None:
            return None
        result.append((offset - start, kind, address + addend))
    return result


def inspect_unit(unit, report_unit, root, linked, names, sections):
    target_path = (root / unit['target_path']).resolve()
    source_path = (root / unit['base_path']).resolve()
    target = Elf(target_path)
    source = Elf(source_path)
    attach_owners(source)
    target_counts = Counter(symbol['name'] for symbol in target.symbols)
    origins = {
        section['name']: int(section['metadata']['virtual_address'])
        for section in report_unit['sections']
        if 'virtual_address' in section.get('metadata', {})
    }
    counts = Counter(symbol['name'] for symbol in source.symbols)
    candidates = defaultdict(list)
    for symbol in source.symbols:
        section = section_name(source, symbol)
        if section not in sections or symbol['kind'] != 1 or not symbol['size'] or counts[symbol['name']] != 1:
            continue
        final = linked.lookup(symbol)
        if final is None or final['size'] != symbol['size'] or section_name(linked.elf, final) != section:
            continue
        candidates[(section, final['value'], symbol['size'])].append(symbol)
    mappings = {}
    records = []
    rejected = Counter()
    for symbol in target.symbols:
        section = section_name(target, symbol)
        if section not in sections or symbol['kind'] != 1 or not symbol['size']:
            continue
        if target_counts[symbol['name']] != 1:
            rejected['ambiguous_target_name'] += 1
            continue
        address = target_address(target, symbol, origins, names)
        choices = candidates[(section, address, symbol['size'])]
        if len(choices) != 1:
            rejected['no_unique_address_owner_and_size'] += 1
            continue
        candidate = choices[0]
        if symbol['name'] == candidate['name']:
            continue
        if target.body(symbol) != source.body(candidate):
            rejected['raw_bytes_differ'] += 1
            continue
        target_relocs = relocations(target, symbol, lambda item: target_address(target, item, origins, names))
        source_relocs = relocations(source, candidate, lambda item: source_address(item, linked))
        if target_relocs is None or source_relocs is None:
            rejected['unresolved_relocation'] += 1
            continue
        if target_relocs != source_relocs:
            rejected['relocations_differ'] += 1
            continue
        previous = unit.get('symbol_mappings', {}).get(symbol['name'])
        if previous is not None and previous != candidate['name']:
            raise ValueError(f"{unit['name']}: conflicting existing mapping for {symbol['name']}")
        mappings[symbol['name']] = candidate['name']
        records.append({
            'target_symbol': symbol['name'],
            'source_symbol': candidate['name'],
            'source_owner': candidate['owner'],
            'section': section,
            'address': f'0x{address:08X}',
            'size': symbol['size'],
            'raw_sha256': digest(target.body(symbol)),
            'normalized_relocations': target_relocs,
        })
    if len(set(mappings.values())) != len(mappings):
        raise ValueError(f"{unit['name']}: non-bijective mapping candidates")
    if target_path.read_bytes() != target.data or source_path.read_bytes() != source.data:
        raise ValueError(f"{unit['name']}: input object changed during audit")
    return {
        'name': unit['name'],
        'target_sha256': digest(target.data),
        'source_sha256': digest(source.data),
        'mappings': mappings,
        'ambiguous_existing_mappings': sorted(name for name in unit.get('symbol_mappings', {}) if target_counts[name] > 1),
        'records': records,
        'rejected': dict(rejected),
    }


def verify_linked_data(root, elf, sections):
    manifest = json.loads((root / 'config/targets/wpse-v1.json').read_text(encoding='utf-8'))
    expected = [item for item in manifest['artifacts'] if item['path'] == 'main.dol']
    if len(expected) != 1:
        raise ValueError('Expected exactly one main.dol artifact')
    dol, ranges = dol_ranges(root / 'build/wpse-v1/main.dol')
    original = (root / 'orig/wpse-v1/main.dol').read_bytes()
    if len(dol) != expected[0]['size'] or digest(dol) != expected[0]['sha256'] or dol != original:
        raise ValueError('Current and original main.dol must match the frozen manifest')
    checked = []
    for index, section in enumerate(elf.sections):
        if section['name'] not in sections:
            continue
        address = section['address']
        reference = range_bytes(dol, ranges, address, address + section['size'])
        if elf.section_data(index) != reference:
            raise ValueError(f"Final ELF section {section['name']} differs from frozen DOL")
        checked.append(section['name'])
    if set(checked) != sections or len(checked) != len(sections):
        raise ValueError('Expected one linked ELF section for every requested data section')
    return digest(dol)


def main():
    parser = argparse.ArgumentParser(description='Propose data symbol mappings from verified final addresses, ownership, raw bytes and relocations.')
    parser.add_argument('--project', type=Path, default=Path('.'))
    parser.add_argument('--unit', action='append')
    parser.add_argument('--section', action='append', choices=('.data', '.sdata', '.rodata', '.sdata2'))
    parser.add_argument('--output', type=Path)
    parser.add_argument('--config-output', type=Path)
    args = parser.parse_args()
    root = args.project.resolve()
    sections = set(args.section or ['.data', '.sdata'])
    raw_config = (root / 'objdiff.json').read_bytes()
    config = json.loads(raw_config)
    raw_report = (root / 'build/wpse-v1/report.json').read_bytes()
    report = json.loads(raw_report)
    reports = {unit['name']: unit for unit in report['units']}
    units = config['units']
    if args.unit:
        wanted = set(args.unit)
        units = [unit for unit in units if unit['name'] in wanted]
        if {unit['name'] for unit in units} != wanted:
            raise ValueError('Unknown requested unit')
    elf = Elf(root / 'build/wpse-v1/main.elf', allowed_types=(2,))
    dol_sha256 = verify_linked_data(root, elf, sections)
    linked = LinkedSymbols(elf)
    names = original_names(root / 'config/wpse-v1/symbols.txt')
    audits = [inspect_unit(unit, reports[unit['name']], root, linked, names, sections) for unit in units]
    if (root / 'objdiff.json').read_bytes() != raw_config or (root / 'build/wpse-v1/report.json').read_bytes() != raw_report or (root / 'build/wpse-v1/main.elf').read_bytes() != elf.data:
        raise ValueError('Shared build inputs changed during audit; rerun after the build completes')
    result = {
        'schema_version': 1,
        'main_dol_sha256': dol_sha256,
        'main_elf_sha256': digest(elf.data),
        'report_sha256': digest(raw_report),
        'sections': sorted(sections),
        'mapping_count': sum(len(audit['mappings']) for audit in audits),
        'ambiguous_existing_mapping_count': sum(len(audit['ambiguous_existing_mappings']) for audit in audits),
        'mapped_bytes': sum(record['size'] for audit in audits for record in audit['records']),
        'units': audits,
    }
    text = json.dumps(result, indent=2) + '\n'
    destinations = []
    for path in (args.output, args.config_output):
        if path is None:
            continue
        destination = path.resolve()
        if root / 'tools/local' not in destination.parents:
            raise ValueError('Diagnostic outputs must be inside tools/local')
        if destination.suffix != '.json' or destination in destinations:
            raise ValueError('Diagnostic outputs must be separate JSON files')
        destinations.append(destination)
    if args.output:
        args.output.write_text(text, encoding='utf-8')
    else:
        sys.stdout.write(text)
    if args.config_output:
        configured = []
        for unit, audit in zip(units, audits):
            entry = dict(unit)
            entry['base_path'] = str((root / unit['base_path']).resolve())
            entry['target_path'] = str((root / unit['target_path']).resolve())
            retained = {name: value for name, value in unit.get('symbol_mappings', {}).items() if name not in audit['ambiguous_existing_mappings']}
            entry['symbol_mappings'] = {**retained, **audit['mappings']}
            configured.append(entry)
        output_config = dict(config)
        output_config['units'] = configured
        args.config_output.write_text(json.dumps(output_config, indent=2) + '\n', encoding='utf-8')
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, KeyError, IndexError, TypeError, struct.error) as error:
        print(f'error: {error}', file=sys.stderr)
        sys.exit(1)
