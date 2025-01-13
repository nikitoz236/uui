#pragma once
#include <stdint.h>

#include "honda_dlc_units.h"

// нужно дергать в основном цикле
void dlc_poll(void);
// работает через обертку kline.h
// при появлении новых данных вызывается metric_ecu_data_ready()
// в зависимости от доступности блока дергает tc_engine_set_status()

// здесь нужен интерфейс чтобы экран с дампом мог показать сырые данные
// а также чтобы экран с дампом мог запросить какое то другое устройство
void dlc_dump_request(honda_unit_t unit);
unsigned dlc_dump_get_new_data(uint8_t * data, unsigned * address, honda_unit_t * unit);
