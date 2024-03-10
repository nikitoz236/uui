#pragma once
#include <stdint.h>

#include "honda_units.h"

void dlc_poll(void);

/*
    здесь нужен интерфейс чтобы экран с дампом мог показать сырые данные
    а также чтобы экран с дампом мог запросить какое то другое устройство
*/

void dlc_dump_request(honda_unit_t unit);
unsigned dlc_dump_get_new_data(uint8_t * data, unsigned * address);
