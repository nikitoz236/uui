import argparse
import serial
import time
from pynput import keyboard

# Функции для управления DTR и RTS

def open_serial_port(port, baudrate):
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        if not ser.is_open:
            print(f"Не удалось открыть порт {port}")
            return None
        print(f"Порт {port} открыт на скорости {baudrate} бод")
        return ser
    except Exception as e:
        print(f"Ошибка при открытии порта {port}: {e}")
        return None

def control_dtr(ser, state):
    """Управление линией DTR."""
    if state:
        ser.setDTR(True)
    else:
        ser.setDTR(False)

def control_rts(ser, state):
    """Управление линией RTS."""
    if state:
        ser.setRTS(True)
    else:
        ser.setRTS(False)

def reset_esp32(ser):
    """Реализуем сброс ESP32 через DTR и RTS."""

    # Выключаем RTS и DTR
    control_rts(ser, False)  # Выключаем RTS
    time.sleep(0.1)           # Пауза, даем время устройству обработать команду
    control_dtr(ser, False)  # Выключаем DTR
    time.sleep(0.1)           # Пауза, даем время устройству обработать команду

    # Включаем RTS и DTR
    control_rts(ser, True)   # Включаем RTS
    time.sleep(0.1)           # Пауза
    control_dtr(ser, True)   # Включаем DTR
    time.sleep(0.1)           # Пауза, чтобы ESP32 стабилизировался

# Главная функция
def main():
    # Парсим аргументы командной строки
    parser = argparse.ArgumentParser(description="Программа для управления последовательным портом.")
    parser.add_argument('port', type=str, help="Путь к последовательному порту (например, COM3 или /dev/ttyUSB0).")
    args = parser.parse_args()

    # Открытие порта
    ser = open_serial_port(args.port, 115200)
    reset_esp32(ser)

if __name__ == "__main__":
    main()
