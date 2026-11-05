import struct


HORIZONTAL_POSITIONS = ('left', 'center', 'right')
VERTICAL_POSITIONS = ('top', 'center', 'bottom')
TEXT_ALIGNMENTS = ('origin', 'left', 'center', 'right')
ANIMATION_KINDS = {'RLPA': 'pane_transform', 'RLVC': 'pane_color'}
TRANSFORM_TARGETS = ('translate_x', 'translate_y', 'translate_z', 'rotate_x', 'rotate_y', 'rotate_z', 'scale_x', 'scale_y', 'width', 'height')
COLOR_TARGETS = tuple(f'{corner}_{channel}' for corner in ('top_left', 'top_right', 'bottom_left', 'bottom_right') for channel in ('red', 'green', 'blue', 'alpha')) + ('alpha',)
ANIMATION_TARGETS = {'pane_transform': TRANSFORM_TARGETS, 'pane_color': COLOR_TARGETS}


def unpack(data, fmt, offset=0):
    return struct.unpack_from('>' + fmt, data, offset)


def pack(fmt, *values):
    return struct.pack('>' + fmt, *values)


def align(data, boundary=4):
    return data + bytes((-len(data)) % boundary)


def write_string(value, size=None, encoding='utf-8'):
    if not isinstance(value, str) or '\0' in value:
        raise ValueError('Expected text without embedded NUL characters')
    raw = value.encode(encoding) + b'\0'
    if size is not None:
        if len(raw) > size:
            raise ValueError(f'Text including its terminator exceeds {size} bytes')
        raw += bytes(size - len(raw))
    return raw


def make_block(kind, payload):
    return kind.encode('ascii') + pack('I', len(payload) + 8) + payload


def make_file(magic, version, blocks):
    payload = b''.join(blocks)
    return pack('4sHHIHH', magic.encode('ascii'), 0xFEFF, version, 16 + len(payload), 16, len(blocks)) + payload


def write_colors(colors, signed=False):
    return b''.join(pack('4h' if signed else '4B', *value) for value in colors)


def encode_position(value):
    return HORIZONTAL_POSITIONS.index(value['horizontal']) + 3 * VERTICAL_POSITIONS.index(value['vertical'])


def encode_resources(kind, resources):
    table = bytearray()
    names = bytearray()
    for resource in resources:
        table.extend(pack('IB3x', len(resources) * 8 + len(names), resource['type']))
        names.extend(write_string(resource['name']))
    return make_block(kind, align(pack('H2x', len(resources)) + table + names))


def encode_material(material, texture_indices):
    maps = material['texture_maps']
    transforms = material['texture_transforms']
    generators = material['texture_coordinate_generators']
    bits = len(maps) | (len(transforms) << 4) | (len(generators) << 8)
    output = write_string(material['name'], 20)
    output += write_colors(material['tev_colors'], True) + write_colors(material['constant_colors']) + pack('I', bits)
    for entry in maps:
        output += pack('HBB', texture_indices[entry['texture']], entry['wrap_s'] | (((entry['min_filter'] - 1) & 7) << 2), entry['wrap_t'] | (((entry['mag_filter'] - 1) & 1) << 2))
    for entry in transforms:
        output += pack('5f', *entry['translate'], entry['rotate'], *entry['scale'])
    for entry in generators:
        output += pack('3Bx', entry['type'], entry['source'], entry['matrix'])
    return output


def encode_materials(materials, texture_indices):
    table = bytearray()
    payload = bytearray()
    start = 12 + len(materials) * 4
    for material in materials:
        table.extend(pack('I', start + len(payload)))
        payload.extend(encode_material(material, texture_indices))
    return make_block('mat1', pack('H2x', len(materials)) + table + payload)


def encode_pane(pane, material_indices, font_indices):
    kind = {'pane': 'pan1', 'picture': 'pic1', 'text': 'txt1'}[pane['kind']]
    flags = int(pane['visible']) | (int(pane['influenced_alpha']) << 1) | (int(pane['location_adjust']) << 2)
    payload = pack('3Bx', flags, encode_position(pane['origin']), pane['alpha'])
    payload += write_string(pane['name'], 16) + write_string(pane['user_data'], 8)
    payload += pack('10f', *pane['translate'], *pane['rotate'], *pane['scale'], *pane['size'])
    if kind == 'pic1':
        payload += write_colors(pane['vertex_colors'])
        payload += pack('HBx', material_indices[pane['material']], len(pane['texture_coordinates']))
        payload += b''.join(pack('8f', *coords) for coords in pane['texture_coordinates'])
    elif kind == 'txt1':
        text = pane['text'].encode('utf-16-be') + bytes(2)
        payload += pack('4HBB2xI', pane['text_capacity'] * 2, len(text), material_indices[pane['material']], font_indices[pane['font']], encode_position(pane['text_origin']), TEXT_ALIGNMENTS.index(pane['text_alignment']), 116)
        payload += write_colors(pane['text_colors'])
        payload += pack('4f', *pane['font_size'], pane['character_spacing'], pane['line_spacing'])
        payload = align(payload + text)
    return make_block(kind, payload)


def encode_layout(layout):
    settings = layout['layout']
    texture_indices = {entry['name']: index for index, entry in enumerate(layout['textures'])}
    material_indices = {entry['name']: index for index, entry in enumerate(layout['materials'])}
    font_indices = {entry['name']: index for index, entry in enumerate(layout['fonts'])}
    blocks = [make_block('lyt1', pack('B3x2f', settings['origin'], *settings['size'])), encode_resources('txl1', layout['textures']), encode_resources('fnl1', layout['fonts']), encode_materials(layout['materials'], texture_indices)]
    def append_panes(panes):
        for pane in panes:
            blocks.append(encode_pane(pane, material_indices, font_indices))
            if 'children' in pane:
                blocks.append(make_block('pas1', b''))
                append_panes(pane['children'])
                blocks.append(make_block('pae1', b''))
    append_panes(layout['panes'])
    for group in layout['groups']:
        blocks.append(make_block('grp1', write_string(group['name'], 16) + pack('H2x', len(group['panes'])) + b''.join(write_string(name, 16) for name in group['panes'])))
    return make_file('RLYT', layout['version'], blocks)


def encode_animation(animation):
    tag = animation['tag']
    name = write_string(tag['name'])
    groups = tag['groups']
    group_offset = 28 + len(align(name))
    tag_payload = pack('HHIIHHB3x', tag['order'], len(groups), 28, group_offset, tag['start_frame'], tag['end_frame'], tag['flags']) + align(name)
    for group in groups:
        tag_payload += write_string(group['name'], 17) + pack('B2x', group['flags'])
    contents = []
    for content in animation['contents']:
        tracks = []
        for track in content['tracks']:
            targets = []
            for target in track['targets']:
                curve_type = {'step': 1, 'hermite': 2}[target['curve']]
                keys = target['keys']
                encoded = pack('3BxH2xI', target['id'], ANIMATION_TARGETS[track['kind']].index(target['target']), curve_type, len(keys), 12)
                for key in keys:
                    encoded += pack('3f', key['frame'], key['value'], key['slope']) if curve_type == 2 else pack('fH2x', key['frame'], key['value'])
                targets.append(encoded)
            table, payload = bytearray(), bytearray()
            for target in targets:
                table.extend(pack('I', 8 + 4 * len(targets) + len(payload)))
                payload.extend(target)
            anim_kind = next(value for value, name in ANIMATION_KINDS.items() if name == track['kind'])
            tracks.append(pack('4sB3x', anim_kind.encode('ascii'), len(targets)) + table + payload)
        table, payload = bytearray(), bytearray()
        for track in tracks:
            table.extend(pack('I', 24 + 4 * len(tracks) + len(payload)))
            payload.extend(track)
        contents.append(write_string(content['name'], 20) + pack('BB2x', len(tracks), ('pane', 'material').index(content['type'])) + table + payload)
    table, payload = bytearray(), bytearray()
    for content in contents:
        table.extend(pack('I', 20 + 4 * len(contents) + len(payload)))
        payload.extend(content)
    animation_payload = pack('HBxHHI', animation['frame_count'], animation['loop'], 0, len(contents), 20) + table + payload
    return make_file('RLAN', animation['version'], [make_block('pat1', tag_payload), make_block('pai1', animation_payload)])
