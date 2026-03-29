#include "tlora_hw.h"
#include "delay_blocking.h"
#include "fat32.h"
#include "sd_cache.h"
#include "str_utils.h"

#include <stdint.h>
#include <stddef.h>

const char * file_path = "died_in_your_arms.wav";

/* ── WAV заголовок ────────────────────────────────────── */
typedef struct __attribute__((packed)) {
    uint8_t  riff[4];          /* "RIFF" */
    uint32_t file_size;
    uint8_t  wave[4];          /* "WAVE" */
    uint8_t  fmt_id[4];        /* "fmt " */
    uint32_t fmt_size;
    uint16_t audio_format;     /* 1 = PCM */
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint8_t  data_id[4];       /* "data" */
    uint32_t data_size;
} wav_header_t;

/* ── двойной буфер и DMA ─────────────────────────────── */
#define BUF_SIZE    2048            /* 4 сектора SD, ~21ms при 48kHz/16bit/mono */

static uint8_t buf_a[BUF_SIZE];
static uint8_t buf_b[BUF_SIZE];
static gdma_desc_t desc[2];

/* ── sector_load для FAT32 ────────────────────────────── */
uint8_t sd_sector_buf[SD_SECTOR_SIZE];
unsigned loaded_sector = -1;

uint8_t * sector_load(unsigned sector)
{
    if (loaded_sector != sector) {
        loaded_sector = sector;
        sd_read_sector(&sd, sector, sd_sector_buf);
    }
    return sd_sector_buf;
}

void sector_mark_dirty(unsigned sector)
{
    (void)sector;
}

/* ── чтение файла по кластерной цепочке ──────────────── */
#define FAT_CLUSTERS_PER_SECTOR     (512 / sizeof(uint32_t))

static uint32_t next_cluster(const fat32_t * fat, uint32_t cluster)
{
    uint32_t sector = fat->fat_offset[0] + cluster / FAT_CLUSTERS_PER_SECTOR;
    uint32_t offset = cluster % FAT_CLUSTERS_PER_SECTOR;
    uint32_t * table = (uint32_t *)sector_load(sector);
    return table[offset];
}

static unsigned cluster_to_sector(const fat32_t * fat, uint32_t cluster, unsigned sec)
{
    return fat->sector_of_zero_cl + (cluster * fat->sectors_per_cluster) + sec;
}

/* ── потоковый читатель файла ─────────────────────────── */
typedef struct {
    const fat32_t * fat;
    uint32_t cluster;
    uint8_t sector_in_cluster;
    uint16_t byte_in_sector;
    uint32_t remaining;
} wav_reader_t;

static unsigned wav_read(wav_reader_t * r, uint8_t * dst, unsigned len)
{
    unsigned written = 0;
    while (written < len && r->remaining > 0) {
        unsigned sector = cluster_to_sector(r->fat, r->cluster, r->sector_in_cluster);
        uint8_t * data = sector_load(sector);

        unsigned avail = 512 - r->byte_in_sector;
        if (avail > r->remaining)
            avail = r->remaining;
        unsigned to_copy = len - written;
        if (to_copy > avail)
            to_copy = avail;

        str_cp(dst + written, data + r->byte_in_sector, to_copy);
        written += to_copy;
        r->remaining -= to_copy;
        r->byte_in_sector += to_copy;

        if (r->byte_in_sector >= 512) {
            r->byte_in_sector = 0;
            r->sector_in_cluster++;
            if (r->sector_in_cluster >= r->fat->sectors_per_cluster) {
                r->sector_in_cluster = 0;
                r->cluster = next_cluster(r->fat, r->cluster);
            }
        }
    }
    return written;
}

/* ── автоподбор делителей I2S ─────────────────────────── */
static void configure_clocks(const wav_header_t * hdr)
{
    /* sample_rate = 40000000 / (mclk_div * bck_div * 2 * bits) */
    uint32_t target = 40000000 / (hdr->sample_rate * 2 * hdr->bits_per_sample);

    unsigned best_m = 10;
    unsigned best_b = 8;
    unsigned best_err = 0xFFFFFFFF;

    for (unsigned m = 2; m <= 255; m++) {
        for (unsigned b = 2; b <= 63; b++) {
            unsigned product = m * b;
            unsigned err = (product > target) ? product - target : target - product;
            if (err < best_err) {
                best_err = err;
                best_m = m;
                best_b = b;
                if (err == 0)
                    goto done;
            }
        }
    }
done:

    i2s.mclk_div = best_m;
    i2s.bck_div = best_b;
    i2s.bits = hdr->bits_per_sample;
    i2s.channels = hdr->num_channels;

    es8311.bits = hdr->bits_per_sample;
    es8311.bclk_div = best_b / 2;
    if (es8311.bclk_div < 1)
        es8311.bclk_div = 1;

    unsigned real_sr = 40000000 / (best_m * best_b * 2 * hdr->bits_per_sample);
    dp("clk: mclk_div="); dpd(best_m, 3);
    dp(" bck_div="); dpd(best_b, 3);
    dp(" real_sr="); dpd(real_sr, 6); dp("Hz");
    dn();
}

/* ── заполнить буфер и вернуть сколько записали ───────── */
static unsigned fill_buf(wav_reader_t * r, uint8_t * buf, unsigned size)
{
    unsigned n = wav_read(r, buf, size);
    if (n < size)
        str_fill(buf + n, size - n, 0);
    return n;
}

/* ── main ─────────────────────────────────────────────── */
int main(void)
{
    dpn("wav player");

    gdma_init();

    init_i2c(&i2c_bus_cfg);
    dpn("[init] i2c ok");

    init_spi(&spi);
    dpn("[init] spi ok");

    xl9555_gpio_set(&amp_en, 0);
    init_xl9555_gpio(&amp_en);
    init_xl9555_gpio(&sd_pwr);

    /* SD power cycle */
    xl9555_gpio_set(&sd_pwr, 0);
    delay_ms(50);
    xl9555_gpio_set(&sd_pwr, 1);
    delay_ms(100);
    dpn("[init] sd power on");

    enum sd_type t = init_sd(&sd);
    if (t == SD_TYPE_NOT_INITIALISATED) {
        dpn("SD init FAIL");
        while (1) {}
    }
    dpn("[init] sd ok");

    fat32_t fat;
    if (!init_fat32(&fat)) {
        dpn("FAT32 init FAIL");
        while (1) {}
    }
    dpn("[init] fat32 ok");

    /* найти файл по имени */
    fat32_file_record_t f;
    if (!file_by_path(&fat, &f, file_path)) {
        dp("file not found: "); dpn(file_path);
        while (1) {}
    }
    dp("found: "); dp(file_path); dp(" size="); dpd(f.size, 10); dn();

    /* прочитать WAV заголовок */
    loaded_sector = -1;
    uint8_t * first = sector_load(cluster_to_sector(&fat, f.cluster, 0));
    wav_header_t hdr;
    str_cp(&hdr, first, sizeof(wav_header_t));

    if (!str_cmp(hdr.riff, "RIFF", 4) || !str_cmp(hdr.wave, "WAVE", 4)) {
        dpn("bad WAV: no RIFF/WAVE");
        while (1) {}
    }
    if (hdr.audio_format != 1) {
        dpn("bad WAV: not PCM");
        while (1) {}
    }

    dp("WAV: "); dpd(hdr.sample_rate, 6); dp("Hz ");
    dpd(hdr.bits_per_sample, 2); dp("bit ");
    dpd(hdr.num_channels, 1); dp("ch ");
    dpd(hdr.data_size, 10); dp("B"); dn();

    /* подобрать делители */
    configure_clocks(&hdr);

    /* потоковый читатель — старт после WAV заголовка */
    wav_reader_t reader = {
        .fat = &fat,
        .cluster = f.cluster,
        .sector_in_cluster = 0,
        .byte_in_sector = sizeof(wav_header_t),
        .remaining = hdr.data_size,
    };

    /* init I2S + ES8311 */
    init_i2s(&i2s);
    dpn("[init] i2s ok");

    dp("ES8311 chip_id = 0x"); dpx(es8311_read_chipid(&es8311), 1); dn();
    init_es8311(&es8311);
    dpn("[init] es8311 ok");

    xl9555_gpio_set(&amp_en, 1);
    dpn("[init] amp on");

    delay_ms(100);
    es8311_set_volume(&es8311, 0xBF);

    /* заполнить оба буфера */
    loaded_sector = -1;
    fill_buf(&reader, buf_a, BUF_SIZE);
    fill_buf(&reader, buf_b, BUF_SIZE);

    /* кольцо: desc[0] -> desc[1] -> desc[0] */
    desc[0] = (gdma_desc_t){ .size = BUF_SIZE, .length = BUF_SIZE,
        .owner = 1, .buf = buf_a, .next = &desc[1] };
    desc[1] = (gdma_desc_t){ .size = BUF_SIZE, .length = BUF_SIZE,
        .owner = 1, .buf = buf_b, .next = &desc[0] };

    /* старт DMA */
    i2s_tx_start(&i2s, &desc[0]);
    dpn("[play] started");

    /* потоковое воспроизведение: ping-pong */
    while (reader.remaining > 0) {
        /* ждём пока DMA доиграет buf_a */
        while (desc[0].owner) {}
        fill_buf(&reader, buf_a, BUF_SIZE);
        if (reader.remaining == 0) {
            desc[0].next = NULL;
            desc[0].eof = 1;
        }
        desc[0].owner = 1;

        if (reader.remaining == 0)
            break;

        /* ждём пока DMA доиграет buf_b */
        while (desc[1].owner) {}
        fill_buf(&reader, buf_b, BUF_SIZE);
        if (reader.remaining == 0) {
            desc[1].next = NULL;
            desc[1].eof = 1;
        }
        desc[1].owner = 1;
    }

    dpn("[play] done");
    while (1) {}
}
