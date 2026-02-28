#!/usr/bin/env python3
"""
ESP32-S3 USB CDC/JTAG serial monitor.

Работает с встроенным USB-JTAG/CDC интерфейсом ESP32-S3 (303a:1001).

Ключевые особенности ESP32-S3 USB CDC:
  - dbg_usb_cdc_acm_tx() проверяет флаг in_token_rec_in_ep1_int_raw перед отправкой.
    Если хост не открыл порт — данные молча отбрасываются.
  - Поэтому порт нужно открыть ДО сброса платы, иначе первые dpn() будут потеряны.
  - Сброс: DTR=False (GPIO0=HIGH, normal boot), затем RTS=True->False (EN pulse).
    RTS=True переводит EN в LOW (сброс), RTS=False отпускает.
  - Если сбрасывать через esptool run — он ставит DTR=True (GPIO0=LOW) и плата
    уходит в режим загрузки (DOWNLOAD mode), а не запускает прошивку.

Использование:
  python3 esp32_monitor.py /dev/ttyACM0          # открыть, сбросить, читать
  python3 esp32_monitor.py /dev/ttyACM0 --no-reset  # только читать без сброса
  python3 esp32_monitor.py /dev/ttyACM0 --baud 115200

Выход: Ctrl+C
"""

import argparse
import serial
import sys
import time


def reset_esp32_normal_boot(ser: serial.Serial):
    """
    Аппаратный сброс ESP32-S3 в нормальный режим загрузки (не bootloader).

    GPIO0 (boot mode pin) управляется через DTR:
      DTR=False -> GPIO0=HIGH -> normal boot (SPI Flash)
      DTR=True  -> GPIO0=LOW  -> download/bootloader mode

    EN (reset pin) управляется через RTS:
      RTS=True  -> EN=LOW  (сброс активен)
      RTS=False -> EN=HIGH (работа)
    """
    ser.setDTR(False)   # GPIO0 = HIGH: normal boot
    time.sleep(0.05)
    ser.setRTS(True)    # EN = LOW: держим сброс
    time.sleep(0.1)
    ser.setRTS(False)   # EN = HIGH: отпускаем, плата стартует


def main():
    parser = argparse.ArgumentParser(
        description="ESP32-S3 USB CDC serial monitor с правильным сбросом"
    )
    parser.add_argument("port", help="Порт, например /dev/ttyACM0")
    parser.add_argument("--baud", type=int, default=115200, help="Baudrate (default: 115200)")
    parser.add_argument("--no-reset", action="store_true", help="Не сбрасывать плату")
    parser.add_argument("--timeout", type=float, default=0, help="Завершить после N секунд (0 = бесконечно)")
    args = parser.parse_args()

    try:
        ser = serial.Serial(args.port, args.baud, timeout=0.05)
    except serial.SerialException as e:
        print(f"Ошибка открытия порта {args.port}: {e}", file=sys.stderr)
        sys.exit(1)

    if not args.no_reset:
        # Порт уже открыт — теперь сбрасываем.
        # ESP32-S3 увидит открытый хост и не потеряет первые dpn().
        reset_esp32_normal_boot(ser)

    start = time.time()
    try:
        while True:
            chunk = ser.read(256)
            if chunk:
                sys.stdout.buffer.write(chunk)
                sys.stdout.buffer.flush()
            if args.timeout > 0 and (time.time() - start) >= args.timeout:
                break
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()


if __name__ == "__main__":
    main()
