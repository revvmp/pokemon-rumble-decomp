import math
import struct

from nw4r_layout import ANIMATION_TARGETS, HORIZONTAL_POSITIONS, TEXT_ALIGNMENTS, VERTICAL_POSITIONS
from gx_texture import FILTERS, WRAP_MODES


def fail(path, message):
    raise ValueError(f'{path}: {message}')


def record(value, path, required, optional=()):
    if not isinstance(value, dict):
        fail(path, 'expected an object')
    missing = set(required) - value.keys()
    extra = value.keys() - set(required) - set(optional)
    if missing:
        fail(path, 'missing fields: ' + ', '.join(sorted(missing)))
    if extra:
        fail(path, 'unknown fields: ' + ', '.join(sorted(extra)))


def sequence(value, path, length=None, maximum=65535):
    if not isinstance(value, list) or len(value) > maximum:
        fail(path, f'expected an array of at most {maximum} entries')
    if length is not None and len(value) != length:
        fail(path, f'expected {length} entries')


def integer(value, path, minimum=0, maximum=65535):
    if type(value) is not int or not minimum <= value <= maximum:
        fail(path, f'expected an integer from {minimum} to {maximum}')


def boolean(value, path):
    if type(value) is not bool:
        fail(path, 'expected a boolean')


def number(value, path):
    if type(value) not in (int, float):
        fail(path, 'expected a finite number')
    try:
        if not math.isfinite(value):
            fail(path, 'expected a finite number')
        encoded = struct.pack('>f', value)
    except (OverflowError, struct.error):
        fail(path, 'number is outside the float32 range')
    if not math.isfinite(struct.unpack('>f', encoded)[0]):
        fail(path, 'number is outside the float32 range')


def text(value, path, byte_capacity=None, encoding='utf-8'):
    if not isinstance(value, str) or '\0' in value:
        fail(path, 'expected text without embedded NUL characters')
    try:
        encoded = value.encode(encoding)
    except UnicodeError:
        fail(path, f'text is not encodable as {encoding}')
    terminator = 2 if encoding == 'utf-16-be' else 1
    if byte_capacity is not None and len(encoded) + terminator > byte_capacity:
        fail(path, f'text including its terminator exceeds {byte_capacity} bytes')


def choice(value, path, choices):
    if not isinstance(value, str) or value not in choices:
        fail(path, 'expected one of: ' + ', '.join(choices))


def vector(value, path, length):
    sequence(value, path, length)
    for index, item in enumerate(value):
        number(item, f'{path}[{index}]')


def colors(value, path, count, signed=False):
    sequence(value, path, count)
    for index, color in enumerate(value):
        sequence(color, f'{path}[{index}]', 4)
        for channel, component in enumerate(color):
            integer(component, f'{path}[{index}][{channel}]', -1024 if signed else 0, 1023 if signed else 255)


def named_records(values, path, capacity):
    sequence(values, path)
    names = set()
    for index, value in enumerate(values):
        if not isinstance(value, dict) or 'name' not in value:
            fail(f'{path}[{index}]', 'expected a named object')
        text(value['name'], f'{path}[{index}].name', capacity)
        if value['name'] in names:
            fail(path, f'duplicate name: {value["name"]}')
        names.add(value['name'])
    return names


def reference(value, path, names):
    if not isinstance(value, str) or value not in names:
        fail(path, f'unknown reference: {value!r}')


def position(value, path):
    record(value, path, ('horizontal', 'vertical'))
    choice(value['horizontal'], path + '.horizontal', HORIZONTAL_POSITIONS)
    choice(value['vertical'], path + '.vertical', VERTICAL_POSITIONS)


def validate_layout(layout):
    record(layout, 'layout', ('version', 'layout', 'textures', 'fonts', 'materials', 'panes', 'groups'))
    integer(layout['version'], 'layout.version', 10, 10)
    record(layout['layout'], 'layout.layout', ('origin', 'size'))
    integer(layout['layout']['origin'], 'layout.layout.origin', 0, 1)
    vector(layout['layout']['size'], 'layout.layout.size', 2)
    resources = {}
    for kind in ('textures', 'fonts'):
        resources[kind] = named_records(layout[kind], f'layout.{kind}', None)
        for index, resource in enumerate(layout[kind]):
            path = f'layout.{kind}[{index}]'
            record(resource, path, ('name', 'type'))
            integer(resource['type'], path + '.type', 0, 255)
    materials = named_records(layout['materials'], 'layout.materials', 20)
    for index, material in enumerate(layout['materials']):
        path = f'layout.materials[{index}]'
        record(material, path, ('name', 'tev_colors', 'constant_colors', 'texture_maps', 'texture_transforms', 'texture_coordinate_generators'))
        colors(material['tev_colors'], path + '.tev_colors', 3, True)
        colors(material['constant_colors'], path + '.constant_colors', 4)
        for kind in ('texture_maps', 'texture_transforms', 'texture_coordinate_generators'):
            sequence(material[kind], path + '.' + kind, maximum=8)
        for entry_index, entry in enumerate(material['texture_maps']):
            entry_path = f'{path}.texture_maps[{entry_index}]'
            record(entry, entry_path, ('texture', 'wrap_s', 'wrap_t', 'min_filter', 'mag_filter'))
            reference(entry['texture'], entry_path + '.texture', resources['textures'])
            for axis in ('s', 't'):
                integer(entry['wrap_' + axis], entry_path + '.wrap_' + axis, 0, 2)
            integer(entry['min_filter'], entry_path + '.min_filter', 0, 5)
            integer(entry['mag_filter'], entry_path + '.mag_filter', 0, 1)
        for entry_index, entry in enumerate(material['texture_transforms']):
            entry_path = f'{path}.texture_transforms[{entry_index}]'
            record(entry, entry_path, ('translate', 'rotate', 'scale'))
            vector(entry['translate'], entry_path + '.translate', 2)
            number(entry['rotate'], entry_path + '.rotate')
            vector(entry['scale'], entry_path + '.scale', 2)
        for entry_index, entry in enumerate(material['texture_coordinate_generators']):
            entry_path = f'{path}.texture_coordinate_generators[{entry_index}]'
            record(entry, entry_path, ('type', 'source', 'matrix'))
            for field in ('type', 'source', 'matrix'):
                integer(entry[field], entry_path + '.' + field, 0, 255)
    pane_names = set()
    def validate_panes(panes, path, depth):
        if depth > 64:
            fail(path, 'pane hierarchy exceeds 64 levels')
        sequence(panes, path)
        for index, pane in enumerate(panes):
            pane_path = f'{path}[{index}]'
            if not isinstance(pane, dict) or pane.get('kind') not in ('pane', 'picture', 'text'):
                fail(pane_path, 'expected a pane, picture or text object')
            fields = ['kind', 'name', 'user_data', 'visible', 'influenced_alpha', 'location_adjust', 'origin', 'alpha', 'translate', 'rotate', 'scale', 'size']
            if pane['kind'] == 'picture':
                fields += ['vertex_colors', 'material', 'texture_coordinates']
            elif pane['kind'] == 'text':
                fields += ['text_capacity', 'material', 'font', 'text_origin', 'text_alignment', 'text_colors', 'font_size', 'character_spacing', 'line_spacing', 'text']
            record(pane, pane_path, fields, ('children',))
            text(pane['name'], pane_path + '.name', 16)
            text(pane['user_data'], pane_path + '.user_data', 8)
            if pane['name'] in pane_names:
                fail(pane_path, 'duplicate pane name')
            pane_names.add(pane['name'])
            for flag in ('visible', 'influenced_alpha', 'location_adjust'):
                boolean(pane[flag], pane_path + '.' + flag)
            position(pane['origin'], pane_path + '.origin')
            integer(pane['alpha'], pane_path + '.alpha', 0, 255)
            for field, count in (('translate', 3), ('rotate', 3), ('scale', 2), ('size', 2)):
                vector(pane[field], pane_path + '.' + field, count)
            if 'material' in pane:
                reference(pane['material'], pane_path + '.material', materials)
            if pane['kind'] == 'picture':
                colors(pane['vertex_colors'], pane_path + '.vertex_colors', 4)
                sequence(pane['texture_coordinates'], pane_path + '.texture_coordinates', maximum=8)
                for coordinate_index, coordinates in enumerate(pane['texture_coordinates']):
                    vector(coordinates, f'{pane_path}.texture_coordinates[{coordinate_index}]', 8)
            elif pane['kind'] == 'text':
                integer(pane['text_capacity'], pane_path + '.text_capacity', 1, 32767)
                reference(pane['font'], pane_path + '.font', resources['fonts'])
                position(pane['text_origin'], pane_path + '.text_origin')
                choice(pane['text_alignment'], pane_path + '.text_alignment', TEXT_ALIGNMENTS)
                colors(pane['text_colors'], pane_path + '.text_colors', 2)
                vector(pane['font_size'], pane_path + '.font_size', 2)
                number(pane['character_spacing'], pane_path + '.character_spacing')
                number(pane['line_spacing'], pane_path + '.line_spacing')
                text(pane['text'], pane_path + '.text', pane['text_capacity'] * 2, 'utf-16-be')
            if 'children' in pane:
                validate_panes(pane['children'], pane_path + '.children', depth + 1)
    validate_panes(layout['panes'], 'layout.panes', 0)
    named_records(layout['groups'], 'layout.groups', 16)
    for index, group in enumerate(layout['groups']):
        path = f'layout.groups[{index}]'
        record(group, path, ('name', 'panes'))
        sequence(group['panes'], path + '.panes')
        for pane in group['panes']:
            reference(pane, path + '.panes', pane_names)
    return pane_names


def validate_animation(animation):
    record(animation, 'animation', ('version', 'tag', 'frame_count', 'loop', 'contents'))
    integer(animation['version'], 'animation.version', 10, 10)
    tag = animation['tag']
    record(tag, 'animation.tag', ('name', 'order', 'start_frame', 'end_frame', 'flags', 'groups'))
    text(tag['name'], 'animation.tag.name')
    for field in ('order', 'start_frame', 'end_frame'):
        integer(tag[field], 'animation.tag.' + field)
    if tag['end_frame'] < tag['start_frame']:
        fail('animation.tag', 'end_frame precedes start_frame')
    integer(tag['flags'], 'animation.tag.flags', 0, 255)
    named_records(tag['groups'], 'animation.tag.groups', 17)
    for index, group in enumerate(tag['groups']):
        path = f'animation.tag.groups[{index}]'
        record(group, path, ('name', 'flags'))
        integer(group['flags'], path + '.flags', 0, 255)
    integer(animation['frame_count'], 'animation.frame_count', 1, 65535)
    boolean(animation['loop'], 'animation.loop')
    named_records(animation['contents'], 'animation.contents', 20)
    for index, content in enumerate(animation['contents']):
        path = f'animation.contents[{index}]'
        record(content, path, ('name', 'type', 'tracks'))
        choice(content['type'], path + '.type', ('pane',))
        sequence(content['tracks'], path + '.tracks', maximum=255)
        for track_index, track in enumerate(content['tracks']):
            track_path = f'{path}.tracks[{track_index}]'
            record(track, track_path, ('kind', 'targets'))
            choice(track['kind'], track_path + '.kind', ANIMATION_TARGETS)
            sequence(track['targets'], track_path + '.targets', maximum=255)
            for target_index, target in enumerate(track['targets']):
                target_path = f'{track_path}.targets[{target_index}]'
                record(target, target_path, ('id', 'target', 'curve', 'keys'))
                integer(target['id'], target_path + '.id', 0, 255)
                choice(target['target'], target_path + '.target', ANIMATION_TARGETS[track['kind']])
                choice(target['curve'], target_path + '.curve', ('hermite',))
                sequence(target['keys'], target_path + '.keys')
                if not target['keys']:
                    fail(target_path + '.keys', 'expected at least one key')
                previous_frame = -math.inf
                for key_index, key in enumerate(target['keys']):
                    key_path = f'{target_path}.keys[{key_index}]'
                    record(key, key_path, ('frame', 'value', 'slope'))
                    for field in ('frame', 'value', 'slope'):
                        number(key[field], key_path + '.' + field)
                    if key['frame'] < previous_frame:
                        fail(key_path, 'keys must be ordered by frame')
                    previous_frame = key['frame']


def validate_texture(texture):
    record(texture, 'texture', ('format', 'wrap_s', 'wrap_t', 'min_filter', 'mag_filter', 'lod_bias', 'edge_lod', 'min_lod', 'max_lod', 'image'))
    choice(texture['format'], 'texture.format', ('I4',))
    for axis in ('s', 't'):
        choice(texture['wrap_' + axis], 'texture.wrap_' + axis, WRAP_MODES)
    choice(texture['min_filter'], 'texture.min_filter', FILTERS)
    choice(texture['mag_filter'], 'texture.mag_filter', FILTERS[:2])
    number(texture['lod_bias'], 'texture.lod_bias')
    boolean(texture['edge_lod'], 'texture.edge_lod')
    integer(texture['min_lod'], 'texture.min_lod', 0, 0)
    integer(texture['max_lod'], 'texture.max_lod', 0, 0)
    text(texture['image'], 'texture.image')


def validate_manifest(manifest):
    record(manifest, 'manifest', ('schema', 'assets'))
    integer(manifest['schema'], 'manifest.schema', 1, 1)
    sequence(manifest['assets'], 'manifest.assets')
    if not manifest['assets']:
        fail('manifest.assets', 'expected at least one asset')
    paths = set()
    for index, asset in enumerate(manifest['assets']):
        path = f'manifest.assets[{index}]'
        record(asset, path, ('path', 'type', 'source'))
        text(asset['path'], path + '.path')
        text(asset['source'], path + '.source')
        choice(asset['type'], path + '.type', ('layout', 'animation', 'texture'))
        if asset['path'] in paths:
            fail(path, 'duplicate asset path')
        paths.add(asset['path'])
