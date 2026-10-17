#include <revolution/net.h>
#include <string.h>

#define NET_ASSERT(exp) ((void)0)

#define SHA1_ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define SHA1_K0 0x5A827999
#define SHA1_K1 0x6ED9EBA1
#define SHA1_K2 0x8F1BBCDC
#define SHA1_K3 0xCA62C1D6

#define SHA1_SCHEDULE(w, i) \
    if ((i) >= 16) { \
        (w)[(i) & 15] = SHA1_ROTL((w)[((i) - 3) & 15] ^ (w)[((i) - 8) & 15] ^ ((w)[((i) - 16) & 15] ^ (w)[((i) - 14) & 15]), 1); \
    }

#define SHA1_ROUND(f, k) \
    temp = (f) + (k); \
    SHA1_SCHEDULE(w, i); \
    temp = w[i & 15] + (SHA1_ROTL(a, 5) + (temp + e)); \
    e = d; \
    d = c; \
    c = SHA1_ROTL(b, 30); \
    b = a; \
    a = temp

static const u8 sPaddingHead = 0x80;
static const u8 sPaddingZero[8] = {0};

static void NETSHA1ProcessBlock(NETSHA1Context* context) {
    u32 w[16];
    u32 a, b, c, d, e, temp;
    int i;

    a = context->state[0];
    b = context->state[1];
    c = context->state[2];
    d = context->state[3];
    e = context->state[4];
    for (i = 0; i < 16; i++) {
        w[i] = ((u32*)context->buffer)[i];
    }

    for (i = 0; i < 20; i++) {
        SHA1_ROUND((b & c) | (~b & d), SHA1_K0);
    }
    for (; i < 40; i++) {
        SHA1_ROUND(b ^ c ^ d, SHA1_K1);
    }
    for (; i < 60; i++) {
        SHA1_ROUND((c & d) | (b & (c | d)), SHA1_K2);
    }
    for (; i < 80; i++) {
        SHA1_ROUND(b ^ c ^ d, SHA1_K3);
    }

    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
}

static inline void NETSHA1FillZero(NETSHA1Context* context, u32 length) {
    u32 fillLength;

    while (length != 0) {
        fillLength = NET_SHA1_BLOCK_SIZE - context->bufferLength;
        if (fillLength > length) {
            fillLength = length;
        }
        memset(context->buffer + context->bufferLength, 0, fillLength);
        length -= fillLength;
        context->bufferLength += fillLength;
        if (context->bufferLength >= NET_SHA1_BLOCK_SIZE) {
            NETSHA1ProcessBlock(context);
            context->bufferLength = 0;
            if (++context->blockCountLo == 0) {
                context->blockCountHi++;
            }
        }
    }
}

void NETSHA1GetDigest(NETSHA1Context* context, void* digest) {
    u32 bitLength[2];

    bitLength[1] = (context->blockCountLo << 9) + (context->bufferLength << 3);
    bitLength[0] = (context->blockCountHi << 9) + (context->blockCountLo >> 23);

    NETSHA1Update(context, &sPaddingHead, 1);
    if (NET_SHA1_BLOCK_SIZE - context->bufferLength < sizeof(bitLength)) {
        NETSHA1Update(context, sPaddingZero, NET_SHA1_BLOCK_SIZE - context->bufferLength);
    }
    NETSHA1FillZero(context, NET_SHA1_BLOCK_SIZE - sizeof(bitLength) - context->bufferLength);
    NETSHA1Update(context, bitLength, sizeof(bitLength));

    memcpy(digest, context->state, NET_SHA1_DIGEST_SIZE);
}

void NETSHA1Update(NETSHA1Context* context, const void* input, u32 length) {
    u32 copyLength;

    NET_ASSERT(input != NULL || length == 0);

    while (length != 0) {
        copyLength = NET_SHA1_BLOCK_SIZE - context->bufferLength;
        if (copyLength > length) {
            copyLength = length;
        }
        memcpy(context->buffer + context->bufferLength, input, copyLength);
        input = (const u8*)input + copyLength;
        length -= copyLength;
        context->bufferLength += copyLength;
        if (context->bufferLength >= NET_SHA1_BLOCK_SIZE) {
            NETSHA1ProcessBlock(context);
            context->bufferLength = 0;
            if (++context->blockCountLo == 0) {
                context->blockCountHi++;
            }
        }
    }
}

void NETSHA1Init(NETSHA1Context* context) {
    context->blockCountLo = 0;
    context->blockCountHi = 0;
    context->bufferLength = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
}
