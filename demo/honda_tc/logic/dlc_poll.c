#include <stddef.h>
#include "str_utils.h"
#include "mstimer.h"
#include "kline.h"
#include "dlc_poll.h"
#include "dlc_frame.h"
#include "metrics_ecu.h"
#include "tc.h"

#define DP_NAME "DLC POLL"
#include "dp.h"

/*

модуль определяет порядок опроса устройств, объем данных

дергает kline

определяет когда мотор заведен а когда нет

пинает вычисление метрик

нужно чтобы данный модуль учитывал эхо kline

*/

static mstimer_t timeout = {};

#define UNIT_RESPONCE_TIMEOUT           200
#define UNIT_NEXT_POLL_TRY              2000

#define DLC_DUMP_INACTIVE               HONDA_UNIT_COUNT

static uint8_t engine_state = 0;

static kline_request_t dlc_req = {};
static uint8_t rx_type = 0;

static uint8_t kline_rx_buf[32] = {};
static uint8_t * kline_responce = &kline_rx_buf[sizeof(kline_request_t)];

static uint8_t dump_poll = 0;
static uint8_t dump_data[16] = {};
static uint8_t dump_addr = 0;
static uint8_t dump_len = 0;
static honda_unit_t dump_unit = 0;

static honda_unit_t dump_unit_req = DLC_DUMP_INACTIVE;       // no active

static honda_unit_t current_unit = HONDA_UNIT_ECU;
static unsigned next_address = 0;

static void dlc_prepare(honda_unit_t unit, uint8_t offset, uint8_t len)
{
    if (unit >= HONDA_UNIT_COUNT) {
        return;
    }

    dlc_req.cmd = honda_dlc_unit_address(unit);
    dlc_req.frame_len = sizeof(kline_request_t);
    dlc_req.offset = offset;
    dlc_req.len = len;
    dlc_req.cs = calc_cs((uint8_t *)&dlc_req, offsetof(kline_request_t, cs));

    rx_type = 0;                    // ожидаемый тип ответ

    kline_rx_buf[1] = 0;            // портим буфер чтобы не обработать его еще раз
}

void dlc_dump_request(honda_unit_t unit)
{
    if (unit >= HONDA_UNIT_COUNT) {
        return;
    }
    dump_unit_req = unit;
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

        есть проблемки.
            вопервых мотор толкьо со второго круга опрашивается.
            хочется чтобы мотор опрашивался без дампа только по короткому списку
    */
    uint8_t len = 16;
    unsigned unit_max_addr = honda_dlc_unit_len(current_unit);
    if (next_address < unit_max_addr) {
        // все адреса в юните
        if ((next_address + len) >= unit_max_addr) {
            len = unit_max_addr - next_address;
        }
    } else {
        // юнит закончился
        if (dump_poll) {
            current_unit = HONDA_UNIT_ECU;
            dump_poll = 0;
        } else {
            if (dump_unit_req == DLC_DUMP_INACTIVE) {
                current_unit = HONDA_UNIT_ECU;
            } else {
                dump_poll = 1;
                current_unit = dump_unit_req;
                dump_unit_req = DLC_DUMP_INACTIVE;
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
    dp("process rx data: "); dpxd(kline_responce, 1, dlc_req.len + 3);
    if (kline_responce[0] != rx_type) {
        dpn("    wrong rx_type");
        return 0;
    }
    if (kline_responce[1] != dlc_req.len + 3) {
        dpn("    wrong len");
        return 0;
    }
    uint8_t cs = calc_cs(kline_responce, dlc_req.len + 2);
    if (cs != kline_responce[dlc_req.len + 2]) {
        dp("    wrong cs: "); dpx(cs, 1); dn();
        return 0;
    }
    dn();
    return 1;
}

static void dlc_send_request(void)
{
    kline_start_transaction((uint8_t *)&dlc_req, sizeof(kline_request_t), kline_rx_buf, sizeof(kline_request_t) + dlc_req.len + 3);
    mstimer_start_timeout(&timeout, UNIT_RESPONCE_TIMEOUT);
}

static void dlc_poll_process_rx_data(void)
{
    // dpn("process rx data");

    if (check_rx_frame_valid()) {
        if (engine_state == 0) {
            engine_state = 1;
            tc_engine_set_status(1);
        }
        if (dump_poll) {
            dump_addr = dlc_req.offset;
            dump_len = dlc_req.len;
            dump_unit = current_unit;
            str_cp(dump_data, &kline_responce[2], dlc_req.len);
        }
        if (current_unit == HONDA_UNIT_ECU) {
            metric_ecu_data_ready(dlc_req.offset, &kline_responce[2], dlc_req.len);
        }
        dlc_next();
    }
    dlc_send_request();
}

static void dlc_poll_process_timeout(void)
{
    dpn("process timeout");
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
