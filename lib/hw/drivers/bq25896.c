#include "i2c.h"

void bq25896_power_off(void)
{
    static const uint8_t batfet_dis[] = { 0x09, (1 << 5) };
    i2c_transaction(0x6B, batfet_dis, sizeof(batfet_dis), 0, 0);
}
