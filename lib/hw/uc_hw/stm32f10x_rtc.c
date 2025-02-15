#include "periph_header.h"
#include "periph_pclk.h"
#include "rtc.h"
#include "dp.h"

// void db(char * s);

static void rtc_enter_configuration_mode(void)
{
    while ((RTC->CRL & RTC_CRL_RTOFF) == 0) {};
    RTC->CRL |= RTC_CRL_CNF;
}

static void rtc_exit_configuration_mode(void)
{
    RTC->CRL &= ~RTC_CRL_CNF;
    while ((RTC->CRL & RTC_CRL_RTOFF) == 0) {};
}

void init_rtc(void)
{
    pclk_ctrl(&(pclk_t)PCLK_BKP, 1);
    pclk_ctrl(&(pclk_t)PCLK_PWR, 1);

    PWR->CR |= PWR_CR_DBP;

    if ((RCC->BDCR & RCC_BDCR_RTCEN) == 0) {
        RCC->BDCR |= RCC_BDCR_BDRST;
        RCC->BDCR &= ~RCC_BDCR_BDRST;

        RCC->BDCR |= RCC_BDCR_LSEON;
        while ((RCC->BDCR & RCC_BDCR_LSERDY) == 0) {};
        RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
        db("LSE enabled\n\r");

//		RCC->CSR |= RCC_CSR_LSION;
//		while ((RCC->CSR & RCC_CSR_LSIRDY) == 0) {};
// 		RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
//		db("LSI enabled\n\r");

        RCC->BDCR |= RCC_BDCR_RTCEN;

        rtc_enter_configuration_mode();

        RTC->PRLH = 0;
        RTC->PRLL = 0x8000 - 1;

        rtc_exit_configuration_mode();

        RTC->CRL &= ~RTC_CRL_RSF;
        while ((RTC->CRL & RTC_CRL_RSF) == 0) {};
        db("RTC inited\n\r");
    } else {
        db("RTC was inited last boot\n\r");
    }

//	RTC->CRH |= RTC_CRH_SECIE;
//	RTC->CRL &= ~RTC_CRL_SECF;

//	init_gpio_n(LED, OUTPUT_PP);
//	set_gpio_n(LED, 1);

//	NVIC_EnableIRQ(RTC_IRQn);
}

//static u8 last_led = 1;
//
//void RTC_IRQHandler(void)
//{
//	if (last_led) {
//		last_led = 0;
//		set_gpio_n(LED, 0);
//	} else {
//		last_led = 1;
//		set_gpio_n(LED, 1);
//	}
//	RTC->CRL &= ~RTC_CRL_SECF;
//}

unsigned rtc_get_time_s(void)
{
    unsigned s = RTC->CNTH << 16;
    s += RTC->CNTL;
    return s;
}

void rtc_set_time_s(unsigned time_s)
{
    rtc_enter_configuration_mode();
    RTC->CNTH = time_s >> 16;
    RTC->CNTL = time_s;
    rtc_exit_configuration_mode();
}
