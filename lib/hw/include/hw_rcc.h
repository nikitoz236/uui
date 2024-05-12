// интерфейс для работы с модулем rcc
// общий конфиг и управление тактированием периферии

// структура с общим конфигом
struct hw_rcc_cfg;

// тип шины периферии
enum pclk_bus;

// описание тактирования периферии
struct hw_pclk;

// применение конфига блока rcc
void hw_rcc_apply_cfg(struct hw_rcc_cfg * cfg);

// управление тактированием периферии
void hw_rcc_pclk_ctrl(struct hw_pclk * pclk, unsigned state);

// функции для получения частоты
unsigned hw_rcc_f_hclk(void);                   // hclk - ядро
unsigned hw_rcc_f_pclk(enum pclk_bus);          // периферия
unsigned hw_rcc_f_timer(enum pclk_bus);         // таймеры
