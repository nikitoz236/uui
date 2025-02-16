#pragma once

typedef struct {
    uint8_t cmd;
    uint8_t frame_len;
    uint8_t offset;
    uint8_t len;
    uint8_t cs;
} kline_request_t;

static inline uint8_t calc_cs(uint8_t * data, unsigned len)
{
    uint8_t cs = 0;
    while (len--) {
        cs -= *data++;
    }
    return cs;
}
