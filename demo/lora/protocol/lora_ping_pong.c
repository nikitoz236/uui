#include "lora_ping_pong.h"
#include "lora.h"
#include "uptime.h"
#include "timers_32.h"

#define DP_NOTABLE
#include "dp.h"

unsigned do_ping(uint32_t seq, ping_stats_t * st, unsigned timeout_ms)
{
    st->stat.seq = seq;

    if (!lora_send((const uint8_t *)&seq, sizeof(seq))) {
        return 0;
    }

    lora_rx_start();

    timer_32_t tim = {};
    t32_run(&tim, get_uptime_ms(), timeout_ms);

    lora_pong_t pong;

    while (t32_is_active(&tim, get_uptime_ms())) {
        unsigned n = lora_rx_read((uint8_t *)&pong, sizeof(pong));
        if (n == sizeof(pong)) {
            lora_standby();

            uint8_t rssi;
            int8_t snr;
            lora_get_packet_status(&rssi, &snr);

            st->stat.cnt++;
            st->pong = pong;
            st->rem = rssi_snr_from_raw(pong.rssi, pong.snr);
            st->loc = rssi_snr_from_raw(rssi, snr);
            return 1;
        }
    }

    lora_standby();
    st->stat.lost++;
    return 0;
}

unsigned do_pong(void)
{
    static pong_state_t state;

    uint32_t seq = 0;
    unsigned n = lora_rx_read((uint8_t *)&seq, sizeof(seq));

    if (n != sizeof(seq)) {
        return 0;
    }

    uint8_t rssi_raw;
    int8_t snr_raw;
    lora_get_packet_status(&rssi_raw, &snr_raw);

    state.rx_cnt++;

    uint32_t lost = 0;
    if (state.prev_seq != 0 && seq > state.prev_seq + 1) {
        lost = seq - state.prev_seq - 1;
    }
    state.prev_seq = seq;

    dp("received: #"); dpd(seq, 5);
    dp("  cnt="); dpd(state.rx_cnt, 5);
    dp("  lost="); dpd(lost, 5);
    dp("  rssi=-"); dpd(rssi_raw / 2, 3);
    dp("  snr=");
    if (snr_raw < 0) {
        dp("-"); dpd((unsigned)(-snr_raw) / 4, 2);
    } else {
        dpd((unsigned)snr_raw / 4, 3);
    }
    dn();

    lora_pong_t pong = {
        .rx_seq = seq,
        .rx_lost = lost,
        .rx_cnt = state.rx_cnt,
        .tx_cnt = state.tx_cnt + 1,
        .rssi = rssi_raw,
        .snr = snr_raw,
    };

    if (!lora_send((const uint8_t *)&pong, sizeof(pong))) {
        dpn("TX TIMEOUT");
        return 0;
    }

    state.tx_cnt++;
    lora_rx_start();

    dp("\treply: seq="); dpd(pong.rx_seq, 5);
    dp("  lost="); dpd(pong.rx_lost, 5);
    dp("  rx_cnt="); dpd(pong.rx_cnt, 5);
    dp("  tx_cnt="); dpd(pong.tx_cnt, 5);
    dp("  rssi=-"); dpd(pong.rssi / 2, 3);
    dp("  snr=");
    if (pong.snr < 0) {
        dp("-"); dpd((unsigned)(-pong.snr) / 4, 2);
    } else {
        dpd((unsigned)pong.snr / 4, 3);
    }
    dn();
    dn();

    return 1;
}
