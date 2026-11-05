import argparse
import hashlib
import json
import struct
import sys
from collections import Counter
from pathlib import Path


class Elf:
    def __init__(self, path, allowed_types=(1,)):
        self.path = path
        self.data = path.read_bytes()
        if self.data[:7] != b'\x7fELF\x01\x02\x01':
            raise ValueError(f'{path}: expected big-endian ELF32')
        header = struct.unpack_from('>16sHHIIIIIHHHHHH', self.data)
        if header[1] not in allowed_types or header[2] != 20 or header[11] != 40:
            raise ValueError(f'{path}: unsupported PowerPC ELF type')
        entries = [struct.unpack_from('>10I', self.data, header[6] + index * 40) for index in range(header[12])]
        names = entries[header[13]]
        strings = self.data[names[4]:names[4] + names[5]]
        self.sections = [dict(name=self.string(strings, row[0]), kind=row[1], address=row[3], offset=row[4], size=row[5], link=row[6], info=row[7], stride=row[9]) for row in entries]
        tables = [section for section in self.sections if section['kind'] == 2]
        if len(tables) != 1 or tables[0]['stride'] != 16:
            raise ValueError(f'{path}: expected one ELF symbol table')
        table = tables[0]
        strings = self.section_data(table['link'])
        self.symbols = []
        for offset in range(table['offset'], table['offset'] + table['size'], 16):
            name, value, size, info, other, section = struct.unpack_from('>IIIBBH', self.data, offset)
            self.symbols.append(dict(name=self.string(strings, name), value=value, size=size, section=section, kind=info & 15, binding=info >> 4))
        self.relocations = {}
        for section in self.sections:
            if section['kind'] != 4:
                continue
            if section['stride'] != 12:
                raise ValueError(f'{path}: unsupported relocation stride')
            relocations = self.relocations.setdefault(section['info'], {})
            for offset in range(section['offset'], section['offset'] + section['size'], 12):
                address, info, addend = struct.unpack_from('>IIi', self.data, offset)
                if address in relocations:
                    raise ValueError(f'{path}: duplicate relocation address')
                relocations[address] = (self.symbols[info >> 8], info & 255, addend)

    @staticmethod
    def string(data, offset):
        return data[offset:data.index(0, offset)].decode('utf-8')

    def section_data(self, index):
        section = self.sections[index]
        start = section['offset']
        return self.data[start:start + section['size']]

    def body(self, symbol):
        data = self.section_data(symbol['section'])
        start = symbol['value'] - self.sections[symbol['section']]['address']
        end = start + symbol['size']
        if start < 0 or end > len(data):
            raise ValueError(f'{self.path}: symbol exceeds section')
        return data[start:end]

    def records(self):
        indexes = [index for index, section in enumerate(self.sections) if section['name'] == 'extabindex']
        if not indexes:
            return {}
        if len(indexes) != 1:
            raise ValueError(f'{self.path}: ambiguous exception-index section')
        index = indexes[0]
        relocations = self.relocations.get(index, {})
        result = {}
        covered = set()
        expected_relocations = set()
        for symbol in self.symbols:
            if symbol['section'] != index or symbol['kind'] != 1:
                continue
            if symbol['size'] != 12:
                raise ValueError(f'{self.path}: invalid exception-index record size')
            start = symbol['value']
            span = set(range(start, start + 12))
            if covered.intersection(span):
                raise ValueError(f'{self.path}: overlapping exception-index records')
            covered.update(span)
            expected_relocations.update((start, start + 8))
            function = relocations.get(start)
            table = relocations.get(start + 8)
            if not function or not table or function[1:] != (1, 0) or table[1:] != (1, 0):
                raise ValueError(f'{self.path}: unsupported exception-index relocations')
            if function[0]['kind'] != 2 or table[0]['kind'] != 1 or not table[0]['size'] or self.sections[table[0]['section']]['name'] != 'extab':
                raise ValueError(f'{self.path}: invalid exception-index targets')
            name = function[0]['name']
            if name in result:
                raise ValueError(f'{self.path}: duplicate exception-index function {name}')
            result[name] = (symbol, table[0])
        if covered != set(range(self.sections[index]['size'])) or expected_relocations != set(relocations):
            raise ValueError(f'{self.path}: uncovered exception-index bytes or relocations')
        return result

    def record_relocations(self, symbol, names):
        start = symbol['value']
        return [(offset - start, names.get(target['name'], target['name']), kind, addend) for offset, (target, kind, addend) in sorted(self.relocations.get(symbol['section'], {}).items()) if start <= offset < start + symbol['size']]


def inspect_unit(unit, root):
    target_path = (root / unit['target_path']).resolve()
    base_path = (root / unit['base_path']).resolve()
    target = Elf(target_path)
    base = Elf(base_path)
    target_records = target.records()
    base_records = base.records()
    mappings = dict(unit.get('symbol_mappings') or {})
    identities = []
    missing = []
    for function, target_pair in target_records.items():
        base_function = mappings.get(function, function)
        base_pair = base_records.get(base_function)
        if base_pair is None:
            missing.append(function)
            continue
        for target_symbol, base_symbol in zip(target_pair, base_pair):
            previous = mappings.get(target_symbol['name'])
            if previous is not None and previous != base_symbol['name']:
                raise ValueError(f"{unit['name']}: conflicting mapping for {target_symbol['name']}")
            mappings[target_symbol['name']] = base_symbol['name']
        identities.append((function, target_pair, base_pair))
    counts = Counter(mappings.values())
    for function, target_pair, base_pair in identities:
        if any(counts[symbol['name']] != 1 for symbol in base_pair):
            raise ValueError(f"{unit['name']}: ambiguous mapped exception symbol for {function}")
    records = []
    for function, target_pair, base_pair in identities:
        record = {'function': function}
        for label, target_symbol, base_symbol in zip(('index', 'table'), target_pair, base_pair):
            record[label] = {
                'target_symbol': target_symbol['name'],
                'base_symbol': base_symbol['name'],
                'target_size': target_symbol['size'],
                'base_size': base_symbol['size'],
                'raw_bytes_equal': target.body(target_symbol) == base.body(base_symbol),
                'relocations_equal': target.record_relocations(target_symbol, mappings) == base.record_relocations(base_symbol, {}),
            }
        records.append(record)
    configured = dict(unit)
    configured.update(target_path=str(target_path), base_path=str(base_path), symbol_mappings=mappings)
    evidence = {
        'name': unit['name'],
        'target_sha256': hashlib.sha256(target.data).hexdigest(),
        'base_sha256': hashlib.sha256(base.data).hexdigest(),
        'target_records': len(target_records),
        'base_records': len(base_records),
        'mapped_records': len(records),
        'missing_functions': missing,
        'exact_tables': sum(row['table']['raw_bytes_equal'] and row['table']['relocations_equal'] for row in records),
        'exact_indexes': sum(row['index']['raw_bytes_equal'] and row['index']['relocations_equal'] for row in records),
        'records': records,
    }
    return configured, evidence


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--project', type=Path, default=Path('.'))
    parser.add_argument('--unit', action='append')
    parser.add_argument('--output', type=Path)
    parser.add_argument('--config-output', type=Path)
    args = parser.parse_args()
    root = args.project.resolve()
    source = root / 'objdiff.json'
    config = json.loads(source.read_text(encoding='utf-8'))
    units = config['units']
    inputs = {source.resolve()}
    inputs.update((root / unit[key]).resolve() for unit in units for key in ('target_path', 'base_path') if unit.get(key))
    outputs = [path.resolve() for path in (args.output, args.config_output) if path is not None]
    if len(outputs) != len(set(outputs)) or inputs.intersection(outputs):
        raise ValueError('Outputs must be distinct and must not overwrite any input')
    for path in outputs:
        if path.exists() and any(path.samefile(item) for item in inputs if item.exists()):
            raise ValueError('Output aliases an input file')
    if len(outputs) == 2 and all(path.exists() for path in outputs) and outputs[0].samefile(outputs[1]):
        raise ValueError('Outputs alias each other')
    if args.unit:
        requested = set(args.unit)
        units = [unit for unit in units if unit['name'] in requested]
        if {unit['name'] for unit in units} != requested:
            raise ValueError('Unknown requested unit')
    configured = []
    audits = []
    errors = []
    for unit in units:
        if not unit.get('base_path') or not unit.get('target_path'):
            raise ValueError(f"{unit['name']}: missing object path")
        try:
            mapped, evidence = inspect_unit(unit, root)
        except (ValueError, IndexError, KeyError, struct.error, UnicodeError) as error:
            errors.append({'name': unit['name'], 'error': str(error)})
            continue
        configured.append(mapped)
        if evidence['target_records']:
            audits.append(evidence)
    result = {'schema_version': 1, 'units': audits, 'errors': errors}
    text = json.dumps(result, indent=2) + '\n'
    if args.output:
        args.output.write_text(text, encoding='utf-8')
    else:
        sys.stdout.write(text)
    if errors:
        raise ValueError(f'{len(errors)} units could not be audited; no configuration emitted')
    if args.config_output:
        if args.config_output.resolve() == source:
            raise ValueError('The diagnostic configuration must not overwrite the shared configuration')
        output_config = dict(config)
        output_config['units'] = configured
        args.config_output.write_text(json.dumps(output_config, indent=2) + '\n', encoding='utf-8')
    return 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, KeyError, struct.error) as error:
        print(f'error: {error}', file=sys.stderr)
        sys.exit(1)
