openocd -f interface/stlink-v2-1.cfg -f target/stm32f1x.cfg -c "program $1 verify 0x8000000; reset run; exit"
