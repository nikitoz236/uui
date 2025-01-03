// интерфейс для работы с модулем rcc
// общий конфиг и управление тактированием периферии

// структура с общим конфигом
struct hw_rcc_cfg;
typedef struct hw_rcc_cfg hw_rcc_cfg_t;

// применение конфига блока rcc
void hw_rcc_apply_cfg(const hw_rcc_cfg_t * cfg);

// описание тактирования периферии
struct hw_pclk;
typedef struct hw_pclk hw_pclk_t;

// управление тактированием периферии
void hw_rcc_pclk_ctrl(const hw_pclk_t * pclk, unsigned state);

// функции для получения частоты
unsigned hw_rcc_f_hclk(void);                       // hclk - ядро
unsigned hw_rcc_f_pclk(const hw_pclk_t * pclk);           // периферия
unsigned hw_rcc_f_timer(const hw_pclk_t * pclk);          // таймеры
