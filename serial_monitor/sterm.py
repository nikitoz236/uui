import argparse
import serial
import sys
import termios
import tty
import select
import time

def stdin_has_data():
    return select.select([sys.stdin], [], [], 0)[0]

def kbd_read_char():
    if stdin_has_data():
        return sys.stdin.read(1)
    return None

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




def main():
    fd = sys.stdin.fileno()
    old = termios.tcgetattr(fd)

    parser = argparse.ArgumentParser(description="Программа для управления последовательным портом.")
    parser.add_argument('port', type=str, help="Путь к последовательному порту (например, COM3 или /dev/ttyUSB0).")
    parser.add_argument('baudrate', type=int, help="Скорость передачи данных, например 115200.")
    args = parser.parse_args()

    ser = open_serial_port(args.port, args.baudrate)
    if not ser:
        return

    try:
        tty.setcbreak(fd)   # <-- ВАЖНО: без Enter

        while True:
            ch = kbd_read_char()
            if ch:
                if ch == 'q':
                    break

            if ser.in_waiting > 0:
                data = ser.read(ser.in_waiting)  # Читаем все доступные байты
                print(data.decode('utf-8'), end='')  # Печатаем без перевода строки

            time.sleep(0.2)

    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old)



if __name__ == "__main__":
    main()
