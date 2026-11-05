import zlib


from nw4r_layout import align, pack, unpack


WRAP_MODES = ('clamp', 'repeat', 'mirror')
FILTERS = ('nearest', 'linear', 'nearest_mipmap_nearest', 'linear_mipmap_nearest', 'nearest_mipmap_linear', 'linear_mipmap_linear')


def paeth(left, up, upper_left):
    estimate = left + up - upper_left
    distances = (abs(estimate - left), abs(estimate - up), abs(estimate - upper_left))
    return (left, up, upper_left)[distances.index(min(distances))]


def decode_png(data):
    if data[:8] != b'\x89PNG\r\n\x1a\n':
        raise ValueError('Expected a PNG image')
    offset = 8
    image_data = bytearray()
    dimensions = None
    while offset < len(data):
        length, kind = unpack(data, 'I4s', offset)
        payload = data[offset + 8:offset + 8 + length]
        checksum = unpack(data, 'I', offset + 8 + length)[0]
        if checksum != zlib.crc32(kind + payload) & 0xFFFFFFFF:
            raise ValueError('Invalid PNG chunk checksum')
        offset += 12 + length
        if kind == b'IHDR':
            width, height, depth, color, compression, filtering, interlace = unpack(payload, 'IIBBBBB')
            if depth != 8 or color != 0 or compression or filtering or interlace:
                raise ValueError('Expected a non-interlaced 8-bit grayscale PNG')
            if dimensions is not None or width == 0 or height == 0 or width > 1024 or height > 1024:
                raise ValueError('Invalid PNG dimensions')
            dimensions = width, height
        elif kind == b'IDAT':
            image_data.extend(payload)
        elif kind == b'IEND':
            if length != 0 or offset != len(data):
                raise ValueError('Invalid PNG ending')
            break
        elif not kind[0] & 32:
            raise ValueError(f'Unsupported critical PNG chunk: {kind!r}')
    else:
        raise ValueError('Missing PNG ending')
    if dimensions is None:
        raise ValueError('Missing PNG dimensions')
    width, height = dimensions
    expected_size = height * (width + 1)
    decompressor = zlib.decompressobj()
    scanlines = decompressor.decompress(image_data, expected_size + 1)
    if len(scanlines) != expected_size or not decompressor.eof or decompressor.unused_data:
        raise ValueError('PNG scanline extent does not match the image dimensions')
    pixels = bytearray(width * height)
    for y in range(height):
        start = y * (width + 1)
        filtering = scanlines[start]
        if filtering > 4:
            raise ValueError('Invalid PNG scanline filter')
        for x in range(width):
            left = pixels[y * width + x - 1] if x else 0
            up = pixels[(y - 1) * width + x] if y else 0
            upper_left = pixels[(y - 1) * width + x - 1] if x and y else 0
            predictors = (0, left, up, (left + up) // 2, paeth(left, up, upper_left))
            pixels[y * width + x] = (scanlines[start + 1 + x] + predictors[filtering]) & 255
    return width, height, bytes(pixels)


def encode_texture(metadata, width, height, pixels):
    if metadata['format'] != 'I4':
        raise ValueError('Only I4 texture encoding is supported')
    if width % 8 or height % 8 or len(pixels) != width * height:
        raise ValueError('I4 dimensions must be multiples of 8')
    if any(pixel % 17 for pixel in pixels):
        raise ValueError('I4 grayscale values must be multiples of 17')
    image = bytearray()
    for tile_y in range(0, height, 8):
        for tile_x in range(0, width, 8):
            for y in range(8):
                for x in range(0, 8, 2):
                    index = (tile_y + y) * width + tile_x + x
                    image.append((pixels[index] // 17 << 4) | (pixels[index + 1] // 17))
    output = pack('3I', 0x20AF30, 1, 12) + pack('2I', 20, 0)
    output += pack('HHIIIIIIfBBBB', height, width, 0, 64, WRAP_MODES.index(metadata['wrap_s']), WRAP_MODES.index(metadata['wrap_t']), FILTERS.index(metadata['min_filter']), FILTERS.index(metadata['mag_filter']), metadata['lod_bias'], metadata['edge_lod'], metadata['min_lod'], metadata['max_lod'], 0)
    return align(output, 32) + image
