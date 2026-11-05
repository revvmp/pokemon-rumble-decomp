from pathlib import PurePosixPath

from nw4r_layout import align, pack, write_string


def encode_archive(members):
    tree = {}
    for name, payload in members.items():
        path = PurePosixPath(name)
        if path.is_absolute() or not path.parts or '\\' in name or ':' in name or any(part in ('.', '..') for part in path.parts):
            raise ValueError(f'Invalid archive member path: {name}')
        branch = tree
        for part in path.parts[:-1]:
            branch = branch.setdefault(part, {})
            if not isinstance(branch, dict):
                raise ValueError(f'File used as a directory: {name}')
        if path.name in branch:
            raise ValueError(f'Duplicate archive path: {name}')
        branch[path.name] = payload
    nodes = []
    def append_directory(name, children, parent):
        index = len(nodes)
        entry = {'name': name, 'parent': parent, 'end': None}
        nodes.append(entry)
        for child_name, child in sorted(children.items()):
            if isinstance(child, dict):
                append_directory(child_name, child, index)
            else:
                nodes.append({'name': child_name, 'payload': child})
        entry['end'] = len(nodes)
    append_directory('', tree, 0)
    names = bytearray()
    for node in nodes:
        node['name_offset'] = len(names)
        names.extend(write_string(node['name']))
    header_size = len(nodes) * 12 + len(names)
    data_start = (32 + header_size + 31) & ~31
    table = bytearray()
    payload = bytearray()
    for node in nodes:
        if 'payload' in node:
            contents = node['payload']
            table.extend(pack('3I', node['name_offset'], data_start + len(payload), len(contents)))
            payload.extend(align(contents, 32))
        else:
            table.extend(pack('3I', 0x01000000 | node['name_offset'], node['parent'], node['end']))
    header = pack('4I', 0x55AA382D, 32, header_size, data_start) + bytes(16)
    return align(header + table + names, 32) + payload
