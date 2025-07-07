#pragma once
#include <stdint.h>

// for view variables
int metric_ecu_get_real(unsigned id);
unsigned metric_ecu_get_raw(unsigned id);

// for view bool flags
unsigned metric_ecu_get_bool(unsigned id);

// for load from dlc
void metric_ecu_data_ready(unsigned addr, const uint8_t * data, unsigned len);

// tables

#define METRIC_ECU_VAR_LIST(macro) \
/*        enum name         name            units  factor  point  addr len type    mul         sub     sign                   */               \
/*                           012345678901234 */ \
    macro(RPM,              "RPM",          0,     X1,     0,     0x00, 2, RMS,    0.25,       0       ), /* Обороты двигателя */ \
    macro(ECU_SPEED,        "SPEED ECU",    "KMh", X1,     0,     0x02, 1, RAW,    0,          0       ), /* Скорость */ \
    macro(ENGINE_T,         "ENGINE T",     "*C",  X1000,  2,     0x10, 1, TMP,    0,          0       ), /* Температура ОЖ(град) */ \
    macro(INTAKE_T,         "INTAKE T",     "*C",  X1000,  2,     0x11, 1, TMP,    0,          0       ), /* Температура входящего воздуха(град) */ \
    macro(INTAKE_P,         "INTAKE P",     "KPa", X1000,  2,     0x12, 1, RMS,    716,        5000    ), /* Датчик абсолютного давления(кПа) */ \
    macro(ATM_P,            "ATM P",        "KPa", X1000,  2,     0x13, 1, RMS,    716,        5000    ), /* Датчик атмосферного давления(кПа) */ \
    macro(THROTTLE,         "THROTTLE",     "%",   X1000,  2,     0x14, 1, RSM,    500,        24      ), /* Датчик положения ДЗ(%) */ \
    macro(OXY_1,            "OXY 1",        "V",   X1000,  2,     0x15, 1, RMS,    19.493177,  0       ), /* Напряжение управляющего ЛЗ (В) */ \
    macro(VOLTAGE_ECU,      "VOLTAGE ECU",  "V",   X1000,  2,     0x17, 1, RMS,    95.693779,  0       ), /* Напряжение сети (В) */ \
    macro(ALTERNATOR_LOAD,  "ALTERN. LOAD", "%",   X1000,  2,     0x18, 1, RMS,    392.156862, 0       ), /* Нагрузка на генератор ALT_FR (%) */ \
    macro(CURRENT,          "CURRENT",      "A",   X1000,  2,     0x19, 1, SRM,    394.150802, 77060   ), /* Электрическая нагрузка блок ELD (А) */ \
    macro(ERG_VALVE_V,      "ERG VALVE",    "V",   X1,     0,     0x1B, 1, RMS,    19.493177,  0       ), /* Датчик положения клапана EGR (В) */ \
    macro(CORR_SHORT,       "CORR. SHORT",  "%",   X1000,  2,     0x20, 1, RMS,    392.156862, 100000  ), /* Краткосрочная коррекция (%) */ \
    macro(CORR_LONG,        "CORR. LONG",   "%",   X1000,  2,     0x22, 1, RMS,    392.156862, 100000  ), /* Долгосрочная коррекция (%) */ \
    macro(INJECTION,        "INJECTION",    "MS",  X1000,  3,     0x24, 2, RMS,    4,          0       ), /* Время впрыска(мс) */ \
    macro(IDLING_ANGLE,     "IGN. ANGLE",   0,     X1000,  1,     0x26, 1, RSM,    500,        128     ), /* Угол Опережения Зажигания - IngAdv */ \
    macro(IDLING_LIMIT,     "IGN. LIMIT",   0,     X1000,  1,     0x27, 1, RSM,    500,        128     ), /* Ограничение УОЗ по ДД - IngLim */ \
    macro(AIR_VALVE_POS,    "AIR VALVE",    "%",   X1000,  2,     0x28, 1, RMS,    392.156862, 0       ), /* Клапан холостого хода (%) */ \
    macro(AIR_VALVE_CUR,    "AIR VALVE",    "mA",  X1,     0,     0x29, 1, RAW,    0,          0       ), /* Клапан холостого хода (мА) */ \
    macro(ERG_CTRL,         "ERG CONTROL",  "%",   X1000,  2,     0x2B, 1, RMS,    392.156862, 0       ), /* Управление клапаном EGR (%) */ \
    macro(ERG_VALVE_POS,    "ERG VALVE",    "%",   X1000,  2,     0x2C, 1, RMS,    392.156862, 0       ), /* Датчик положения клапана EGR (%) */ \
    macro(AIR_FUEL,         "AIR/FUEL",     0,     X1,     0,     0x2E, 1, RAW,    0,          0       ), /* Отношение воздух/топливо AF FB CMD */ \
    macro(PCS_EVAP,         "PCS EVAP",     "%",   X1000,  2,     0x2F, 1, RMS,    392.156862, 0       ), /* Клапан PCS EVAP (%) */ \
    macro(DETONATION,       "DETONATION",   0,     X1,     0,     0x3C, 1, RMS,    18.181818,  0       ), /* Датчик детонации */ \
    macro(FUEL_SYSTEM,      "FUEL SYSTEM",  0,     X1,     0,     0x9A, 1, RAW,    0,          0       ), /* Состояние топливной системы FSS */ \
    macro(ENGINE_LOAD,      "ENGINE LOAD",  "%",   X1000,  2,     0x9C, 1, RMS,    392.156862, 0       ), /* Нагрузка на двигатель (%) */ \
    macro(OXY_2,            "OXY 2",        "V",   X1,     0,     0xA0, 1, RMS,    19.493177,  0       ), /* Напряжение проверяющего ЛЗ (В) */ \
    macro(MISF_CYL,         "MisF_Cyl",     0,     X1,     0,     0xB6, 1, RAW,    0,          0       ), /* Пропуск зажигания MisF_Cyl */ \
    macro(CKP_LEARN,        "CKP Learn",    0,     X1,     0,     0xBC, 1, RAW,    0,          0       )  /* Обучение CKP_Learn */

#define METRIC_ECU_BOOL_LIST(macro) \
    /*    enum name          name           bit number*/ \
    /*                       012345678901234 */ \
    macro(STARTER,          "STARTER",      (0 * 8) + 0 /* 0x08:0 */ ), \
    macro(AC_BUTTON,        "AC BUTTON",    (0 * 8) + 1 /* 0x08:1 */ ), \
    macro(POWER_STEERING,   "POWER STEER.", (0 * 8) + 2 /* 0x08:2 */ ), \
    macro(BRAKES,           "BRAKES",       (0 * 8) + 3 /* 0x08:3 */ ), \
    macro(VTEC_SENSOR,      "VTEC SENSOR",  (0 * 8) + 7 /* 0x08:7 */ ), \
    macro(SCS,              "SCS",          (1 * 8) + 3 /* 0x09:3 */ ), \
    macro(VTEC_CTRL,        "VTEC CONTROL", (2 * 8) + 2 /* 0x0A:2 */ ), \
    macro(MAIN_RELAY,       "MAIN RELAY",   (3 * 8) + 0 /* 0x0B:0 */ ), \
    macro(AC_CTRL,          "AC CONTROL",   (3 * 8) + 1 /* 0x0B:1 */ ), \
    macro(O2_HEAT1,         "O2 HEAT 1",    (3 * 8) + 2 /* 0x0B:2 */ ), \
    macro(CHECK_ENGINE,     "CHECK ENGINE", (3 * 8) + 4 /* 0x0B:4 */ ), \
    macro(O2_HEAT2,         "O2 HEAT 2",    (3 * 8) + 7 /* 0x0B:7 */ ), \
    macro(ALTERNATOR_CTRL,  "ALTER. CTRL",  (4 * 8) + 0 /* 0x0C:0 */ ), \
    macro(FAN_CTRL,         "FAN CONTROL",  (4 * 8) + 1 /* 0x0C:1 */ ), \
    macro(INTAKE_CTRL,      "INTAKE CTRL",  (4 * 8) + 2 /* 0x0C:2 */ ), \
    macro(VTEC_E,           "VTEC E",       (4 * 8) + 3 /* 0x0C:3 */ ), \
    macro(ECONO,            "ECONO",        (4 * 8) + 7 /* 0x0C:7 */ ), \
    macro(AT_CONTOL,        "AT CONTOL",    (5 * 8) + 3 /* 0x0D:3 */ ), \
    macro(CLOSED_LOOP,      "CLOSED LOOP",  (7 * 8) + 3 /* 0x0F:3 */ )
