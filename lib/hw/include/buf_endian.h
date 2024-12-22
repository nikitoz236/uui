#include <stdint.h>

// Модуль для работы с многобайтными значениями в передаваемых и принимаемых буфферах
// сырых данных типа u8

// _from_ - функции для получения значения из буффера по указателю
// _to_ - функции для помещения значения в буффер по указателю

// big endian           -- NATIVE FOR MODBUS
//             24  16  8   0
// value : 0x AA  BB  CC  DD
// buffer:
//     N+0: AA
//     N+1: BB
//     N+2: CC
//     N+3: DD

// little endian        -- NATIVE FOR CORTEX
//             24  16  8   0
// value : 0x AA  BB  CC  DD
// buffer:
//     N+0: DD
//     N+1: CC
//     N+2: BB
//     N+3: AA

#if __BYTE_ORDER != __LITTLE_ENDIAN
    #error only little endian architecture support
#endif

static inline void u16_to_be_buf8(uint8_t * buf, uint16_t value)
{
    buf[0] = (value >> 8) & 0xFF;
    buf[1] = (value >> 0) & 0xFF;
}

static inline void u16_to_le_buf8(uint8_t * buf, uint16_t value)
{
    buf[0] = (value >> 0) & 0xFF;
    buf[1] = (value >> 8) & 0xFF;
}

static inline void u32_to_be_buf8(uint8_t * buf, uint32_t value)
{
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8) & 0xFF;
    buf[3] = (value >> 0) & 0xFF;
}

static inline void u32_to_le_buf8(uint8_t * buf, uint32_t value)
{
    buf[0] = (value >> 0) & 0xFF;
    buf[1] = (value >> 8) & 0xFF;
    buf[2] = (value >> 16) & 0xFF;
    buf[3] = (value >> 24) & 0xFF;
}

static inline void u32_to_be_buf16(uint16_t * buf, uint32_t value)
{
    buf[0] = value >> 16;
    buf[1] = value & 0xFFFF;
}

static inline void u32_to_le_buf16(uint16_t * buf, uint32_t value)
{
    buf[0] = value & 0xFFFF;
    buf[1] = value >> 16;
}

static inline uint16_t u16_from_be_buf8(const uint8_t * buf)
{
    return (buf[0] << 8) + (buf[1] << 0);
}

static inline uint16_t u16_from_le_buf8(const uint8_t * buf)
{
    return (buf[0] << 0) + (buf[1] << 8);
}

static inline uint32_t u32_from_be_buf8(const uint8_t * buf)
{
    return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0);
}

static inline uint32_t u32_from_le_buf8(const uint8_t * buf)
{
    return (buf[0] << 0) + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}

static inline uint32_t u32_from_be_buf16(const uint16_t * buf)
{
    return (buf[0] << 16) + (buf[1] << 0);
}

static inline uint32_t u32_from_le_buf16(const uint16_t * buf)
{
    return (buf[0] << 0) + (buf[1] << 16);
}
