#include "ui_tree.h"

extern const widget_desc_t __widget_time_settings;
extern const widget_desc_t __widget_date_settings;
extern const widget_desc_t __widget_time_zone_settings;

typedef struct {
    const int * timezone_s;
    void (*set_timezone)(int timezone_s);
} __widget_time_zone_settings_cfg_t;
