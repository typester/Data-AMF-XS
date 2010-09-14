#include <arpa/inet.h>

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __LITTLE_ENDIAN__
#elif __BYTE_ORDER == __BIG_ENDIAN
#define __BIG_ENDIAN__
#endif
#endif

static void* swap_bytes(void* data, int len) {
    unsigned char* b = (unsigned char*)data;
    unsigned char  t;

    if (2 == len) {
        t = b[0]; b[0] = b[1]; b[1] = t;
    }
    else if (4 == len) {
        t = b[0]; b[0] = b[3]; b[3] = t;
        t = b[1]; b[1] = b[2]; b[2] = t;
    }
    else if (8 == len) {
        t = b[0]; b[0] = b[7]; b[7] = t;
        t = b[1]; b[1] = b[6]; b[6] = t;
        t = b[2]; b[2] = b[5]; b[5] = t;
        t = b[3]; b[3] = b[4]; b[4] = t;
    }

    return data;
}

