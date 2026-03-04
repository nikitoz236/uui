#include "lora_ping_pong.h"
#include "lora.h"
#include "uptime.h"
#include "timers_32.h"

#include "dp.h"

void dp_lora_signal(rssi_snr_t * q)
{
    dp("rssi: "); dpds(q->rssi, 4); dp("  snr: "); dpds(q->rssi, 4);
}

unsigned do_ping(ping_stats_t * state, unsigned timeout_ms)
{
    if (!lora_send((const uint8_t *)&state->seq, sizeof(uint32_t))) {
        dpn("not sended");
        return 0;
    }

    lora_rx_start();

    timer_32_t tim = {};
    t32_run(&tim, get_uptime_ms(), timeout_ms);

    while (t32_is_active(&tim, get_uptime_ms())) {
        if (lora_rx_check_dio_pin()) {
            unsigned n = lora_rx_read((uint8_t *)&state->pong, sizeof(pong_state_t));
            if (n == sizeof(pong_state_t)) {
                lora_get_packet_status(&state->q.rssi, &state->q.snr);
                lora_standby();
                return 1;
            } else {
                dp("recieved bad len");
            }
        }
    }

    dpn("not recieved");
    lora_standby();
    return 0;
}

unsigned do_responce(pong_state_t * state)
{
    lora_rx_start();

    while (!lora_rx_check_dio_pin()) {};

    unsigned n = lora_rx_read((uint8_t *)&state->seq, sizeof(uint32_t));

    if (n != sizeof(uint32_t)) {
        return 0;
    }

    state->cnt++;

    lora_get_packet_status(&state->q.rssi, &state->q.snr);


    dp("received. "); dp("  seq: "); dpd(state->seq, 6); dp("  cnt: "); dpd(state->cnt, 6); dp("  "); dp_lora_signal(&state->q); dn();

    if (!lora_send((const uint8_t *)state, sizeof(pong_state_t))) {
        dpn("TX TIMEOUT");
        return 0;
    }

    return 1;
}
