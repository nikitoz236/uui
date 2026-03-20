#include "utf.h"

#define DP_OFF
#define DP_NAME "UTF"
#include "dp.h"

unsigned utf16_to_utf8(uint8_t * dst, unsigned dst_max, const uint16_t * src, unsigned src_len)
{
    unsigned si = 0;
    unsigned di = 0;
    while (1) {
        if (src_len != 0) {
            if (si >= src_len) {
                break;
            }
        }
        if (dst_max == 0) {
            break;
        }
        utf16_t c_u16 = src[si++];

        if (c_u16 >= 0xD800 && c_u16 <= 0xDFFF) {
            if (di < dst_max) {
                dst[di++] = '@';
            }
            /* Любой суррогат превращается в одну '@', пропускать только один 16-битный символ */
        } else {
            if (c_u16 <= 0x7F) {
                dst[di++] = (uint8_t)c_u16;
                dp("1 byte encode, char"); dpx(c_u16, 2); dp(" di "); dpd(di, 4); dp(" si "); dpd(si, 4); dn();
            } else if (c_u16 <= 0x7FF) {
                if (dst_max < 2) {
                    break;
                }
                dst[di++] = (uint8_t)(0xC0 | ((c_u16 >> 6) & 0x1F));
                dst[di++] = (uint8_t)(0x80 | (c_u16 & 0x3F));
                dst_max--;
            } else {
                if (dst_max < 3) {
                    break;
                }
                dst[di++] = (uint8_t)(0xE0 | ((c_u16 >> 12) & 0x0F));
                dst[di++] = (uint8_t)(0x80 | ((c_u16 >> 6) & 0x3F));
                dst[di++] = (uint8_t)(0x80 | (c_u16 & 0x3F));
                dst_max -= 2;
            }
        }
        dst_max--;

        if (c_u16 == 0) {
            break;
            dp("0 detected, di "); dpd(di, 4); dp(" si "); dpd(si, 4); dn();
        }
    }
    return di;
}
