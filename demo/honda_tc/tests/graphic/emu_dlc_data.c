#include "dlc_poll.h"
#include "str_utils.h"

typedef struct {
    unsigned len;
    uint8_t * data;
    uint8_t cmd;
} mem_map_t;


#define OBD_SPEED_KMH			123
#define OBD_RPM					4567



static const mem_map_t honda_units_map[] = {
    [HONDA_UNIT_ECU] = { .cmd = 0x20, .len = 0x100, .data = (uint8_t []){
        (OBD_RPM / 64),						// 0x00		rpm * 4 high byte
        ((OBD_RPM % 64) * 4),				// 0x01		rpm * 4 low byte
        OBD_SPEED_KMH,						// 0x02		speed km/h
                        0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xA1, 0x00, 0x00, 0x01,
        0x29, 0x6C, 0x30, 0x00, 0x19, 0x13, 0x00, 0x93, 0x84, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
        0x83, 0x00, 0x80, 0x00, 0x02, 0x74, 0x54, 0x00, 0x17, 0x61, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x2D, 0x37, 0x00, 0x2D, 0x07, 0x2D, 0x19, 0x19, 0x19, 0x00, 0x03, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0B, 0xB8, 0x10, 0x96, 0x0B, 0x10, 0x10, 0x00, 0x07, 0x00,
        0xF8, 0x00, 0xF8, 0x00, 0xF0, 0x00, 0xF8, 0xF8, 0xFC, 0xF0, 0x00, 0xF8, 0xF8, 0x00, 0x00, 0x80,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x61, 0x02, 0x00, 0x49, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    } },
    [HONDA_UNIT_ABS] = { .cmd = 0xA0, .len = 0x80, .data = (uint8_t []){
        0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF4,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0xFF, 0xB5, 0x00, 0x00, 0x00, 0x00,
        0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x63, 0x5D, 0x5D, 0x02, 0x01, 0x02, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x4C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xCA, 0xFE,
        0x03, 0x0D, 0x02, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    } },
    [HONDA_UNIT_SRS] = { .cmd = 0x60, .len = 0x80, .data = (uint8_t []){
        0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x03, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x04, 0x8F, 0x8F, 0x8F, 0x8F, 0x95, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0A, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    } },
};

unsigned current_unit = HONDA_UNIT_ECU;
unsigned current_address = -1;

void dlc_dump_request(honda_unit_t unit)
{
    if (current_address < honda_units_map[current_unit].len) {
        return;
    }
    current_unit = unit;
    current_address = 0;
}

unsigned dlc_dump_get_new_data(uint8_t * data, unsigned * address, honda_unit_t * unit)
{
    if (current_address < honda_units_map[current_unit].len) {
        // printf("DLC EMU read %s dump from %d\n", honda_dlc_unit_name(current_unit), current_address);
        *address = current_address;
        *unit = current_unit;
        str_cp(data, &honda_units_map[current_unit].data[current_address], 16);

        int r = rand();
        r &= 0x7F;
        if (r < 0x10) {
            printf("DLC emu - data random pick!\n");
            data[r]++;
        }

        current_address += 16;
        return 16;
    }
    return 0;
}
