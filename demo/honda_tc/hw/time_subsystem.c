#include "date_time.h"
#include "rtc.h"
#include "storage.h"
#include "file_id_list.h"

#define DP_NAME "TIME"
#include "dp.h"

static int timezone_s = 0;

int time_zone_get(void)
{
    return timezone_s;
}

void time_zone_set(int tz)
{
    timezone_s = tz;
    storage_write_file(FILE_ID_TIMEZONE, &timezone_s, sizeof(tz));
}

void init_date_time_subsystem(void)
{
    unsigned len = 0;
    const int * tz_file = storage_search_file(FILE_ID_TIMEZONE, &len);
    if (tz_file) {
        timezone_s = *tz_file;
    }

    init_rtc();

    unsigned rtc_time_s = rtc_get_time_s();
    time_t t;
    date_t d;
    unsigned printable_tz = timezone_s;

    dp("rtc time: "); dpd(rtc_time_s, 10); dp(" timezone: ");

    char * tz_sign = "+";

    if (timezone_s < 0) {
        tz_sign = "-";
        printable_tz -= timezone_s;
    }

    dp(tz_sign); dpdz(printable_tz, 6);

    dp("    ");
    // dpn();

    rtc_time_s += timezone_s;
    time_from_s(&t, rtc_time_s);
    date_from_s(&d, rtc_time_s);
    dpd(d.d, 2); dp(" "); dpl(month_name(d.m), 4); dpd(d.y, 4);
    dp(" "); dpdz(t.h, 2); dp(":"); dpdz(t.m, 2); dp(":"); dpdz(t.s, 2);

    time_from_s(&t, printable_tz);
    dp(" [ "); dp(tz_sign); dpdz(t.h, 2); dp(":"); dpdz(t.m, 2); dp(" ]");
    dn();
}