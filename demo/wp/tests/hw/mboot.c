#include <stdint.h>

#define WDG_KR_KEY_RELOAD           0xAAAA
#define BOOTLOADER_SIGNATURE        "mr6cuG"
const volatile char __attribute__((section (".section_signature"))) bootloader_signature[12] = BOOTLOADER_SIGNATURE;
const volatile uint32_t __attribute__((section (".section_flag"))) bootloader_flag = 0xAABBCCDD;
