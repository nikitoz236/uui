#include <stddef.h>
#include "str_utils.h"
#include "mstimer.h"
#include "kline.h"
#include "dlc_poll.h"
#include "metrics_ecu.h"
#include "tc.h"

#include "dp.h"

/*

модуль определяет порядок опроса устройств, объем данных

дергает kline

определяет когда мотор заведен а когда нет

пинает вычисление метрик

*/

static mstimer_t timeout = {};

#define UNIT_RESPONCE_TIMEOUT           200
#define UNIT_NEXT_POLL_TRY              2000

uint8_t engine_state = 0;

typedef struct {
    uint8_t cmd;
    unsigned len;
} dlc_unit_desc_t;

static const dlc_unit_desc_t honda_units_desc[] = {
    [HONDA_UNIT_ECU] = { .cmd = 0x20, .len = 0x100 },
    [HONDA_UNIT_ABS] = { .cmd = 0xA0, .len = 0x80 },
    [HONDA_UNIT_SRS] = { .cmd = 0x60, .len = 0x80 },
};

typedef struct {
    uint8_t cmd;
    uint8_t frame_len;
    uint8_t offset;
    uint8_t len;
    uint8_t cs;
} kline_request_t;

static kline_request_t kline_request = {};
static uint8_t rx_type = 0;

static uint8_t kline_rx_buf[32] = {};

static uint8_t dump_poll = 0;
static uint8_t dump_data[16] = {};
static uint8_t dump_addr = 0;
static uint8_t dump_len = 0;

static honda_unit_t dump_unit = HONDA_UNIT_COUNT;       // no active

static honda_unit_t current_unit = HONDA_UNIT_ECU;
static unsigned next_address = 0;

static uint8_t calc_cs(uint8_t * data, unsigned len)
{
    uint8_t cs = 0;
    while (len--) {
        cs -= *data++;
    }
    return cs;
}

static void dlc_prepare(honda_unit_t unit, uint8_t offset, uint8_t len)
{
    if (unit >= HONDA_UNIT_COUNT) {
        return;
    }

    kline_request.cmd = honda_units_desc[unit].cmd;
    kline_request.frame_len = sizeof(kline_request);
    kline_request.offset = offset;
    kline_request.len = len;
    kline_request.cs = calc_cs((uint8_t *)&kline_request, offsetof(kline_request_t, cs));

    rx_type = 0;                    // ожидаемый тип ответ

    kline_rx_buf[1] = 0;            // портим буфер чтобы не обработать его еще раз
}

void dlc_dump_request(honda_unit_t unit)
{
    if (unit >= HONDA_UNIT_COUNT) {
        return;
    }
    dump_unit = unit;
}

unsigned dlc_dump_get_new_data(uint8_t * data, unsigned * address, honda_unit_t * unit)
{
    unsigned len = dump_len;
    if (dump_len) {
        *address = dump_addr;
        *unit = dump_unit;
        str_cp(data, dump_data, dump_len);
        dump_len = 0;
    }
    return len;
}

static void dlc_next(void)
{
    /*
        планировщик обязательно будет опрашивать ECU
        если есть запрос на дамп, то он будет выполнен как только закончится полностью чтение всей памяти ECU
        между запросами на дамп обязательно будет вставлен один цикл опроса ECU
    */
    uint8_t len = 16;
    if (next_address < honda_units_desc[current_unit].len) {
        if ((next_address + len) >= honda_units_desc[current_unit].len) {
            len = honda_units_desc[current_unit].len - next_address;
        }
    } else {
        if (dump_poll) {
            current_unit = HONDA_UNIT_ECU;
            dump_poll = 0;
        } else {
            if (dump_unit == HONDA_UNIT_COUNT) {
                current_unit = HONDA_UNIT_ECU;
            } else {
                dump_poll = 1;
                current_unit = dump_unit;
                dump_unit = HONDA_UNIT_COUNT;
            }
        }
        next_address = 0;
    }
    dlc_prepare(current_unit, next_address, len);
    next_address += len;
}

static void dlc_first(void)
{
    current_unit = HONDA_UNIT_ECU;
    dlc_prepare(current_unit, 0, 16);
    next_address = 16;
}

unsigned check_rx_frame_valid(void)
{
    if (kline_rx_buf[0] != rx_type) {
        return 0;
    }
    if (kline_rx_buf[1] != kline_request.len + 3) {
        return 0;
    }
    uint8_t cs = calc_cs(kline_rx_buf, kline_request.len + 2);
    if (cs != kline_rx_buf[kline_request.len + 2]) {
        return 0;
    }
    return 1;
}

static void dlc_send_request(void)
{
    kline_start_transaction((uint8_t *)&kline_request, sizeof(kline_request), kline_rx_buf, kline_request.len + 3);
    mstimer_start_timeout(&timeout, UNIT_RESPONCE_TIMEOUT);
}

static void dlc_poll_process_rx_data(void)
{
    dpn("DLC poll process rx data");
    if (check_rx_frame_valid()) {
        if (engine_state == 0) {
            engine_state = 1;
            tc_engine_set_status(1);
        }
        if (dump_poll) {
            dump_addr = kline_request.offset;
            dump_len = kline_request.len;
            str_cp(dump_data, &kline_rx_buf[2], kline_request.len);
        }
        if (current_unit == HONDA_UNIT_ECU) {
            metric_ecu_data_ready(kline_request.offset, &kline_rx_buf[2], kline_request.len);
        }
        dlc_next();
    }
    dlc_send_request();
}

static void dlc_poll_process_timeout(void)
{
    dpn("DLC poll process timeout");
    if (engine_state) {
        engine_state = 0;
        tc_engine_set_status(0);
    }
    dlc_first();
    dlc_send_request();
    mstimer_start_timeout(&timeout, UNIT_NEXT_POLL_TRY);
}

void dlc_poll(void)
{
    if (kline_is_resp_available()) {
        dlc_poll_process_rx_data();
    } else if (mstimer_is_over(&timeout)) {
        dlc_poll_process_timeout();
    }
}
