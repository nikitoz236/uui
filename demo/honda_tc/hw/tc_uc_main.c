#include "config.h"
#include "rtc.h"
#include "str_val.h"
#include "dp.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

int main(void)
{
    //  000 Zero wait state, if 0 < SYSCLK≤ 24 MHz
    //  001 One wait state, if 24 MHz < SYSCLK ≤ 48 MHz
    //  010 Two wait states, if 48 MHz < SYSCLK ≤ 72 MHz
    FLASH->ACR |= FLASH_ACR_LATENCY * 2;
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    hw_rcc_apply_cfg(&hw_rcc_cfg);

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_PARTIALREMAP;
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    gpio_cfg_t led_pin_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_LOW,
        .type = GPIO_TYPE_PP,
        .pull = GPIO_PULL_NONE,
    };

    gpio_pin_t led_pin = {
        .port = GPIO_PORT_C,
        .pin = 13,
    };

    gpio_set_cfg(&led_pin, &led_pin_cfg);
    gpio_set_state(&led_pin, 0);

    usart_set_cfg(&debug_usart);

    usart_tx(&debug_usart, "Hey bitch!\r\n", 12);

    init_rtc();
    /*
    
    надо проверить ходят ли они ? сбрасываются ли без питания ? читать время - показывать.

    потом ставить время

    надо логгинг

    надо чтобы в уарте было дма

    надо уметь отправить на отправку в буффер асинхронно ченить еще, можно по буквам. можно прям блоком

        надо рингбуффер на дма. циркулярный режим все дела. 

    dp()
    dpn
    dpdn
    dpwdwn
    
    db(str, "rtc time:", dec, 6, rtc_s, rn );

    */

    unsigned rtc_last = 0;

    while (1) {
        unsigned rtc_s = rtc_get_time_s();
        if (rtc_last != rtc_s) {
            rtc_last = rtc_s;
            dp("rtc time: ");
            dpdz(rtc_s, 10);
            dn("\r\n");
        }
    }

    return 0;
}
