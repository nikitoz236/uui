

    /*
        тут проблема что дочерний элемент не имеет calc и не влияет на item->f.s, там остается мусор


        крч бесит. непонятная залупа. непонятно зачем эти все расширения. или нужны. хуй его знает


        проблема такая. у тебя у самого главного родительского элемента перетирается форма. а не должна. он должен это осознавать и береч ее. и не понятно в какой момент второй вложеный такой же элемент будет
        понимать как расширять форму

        как это все должно работать. у нас есть двумерное дерево
        в кажом узле есть форма
        изначально в какойто момент корень имеет форму всего экрана

        дальше нужно понимать как работает каждый элемент
        либо он не зависит от дочерних и ему нужно все доступное пространство
        либо он получает от дочерних размеры и расширяет свою форму - так ли это ???

        представь себе компоновки

        тоесть можно сверху вниз уменьшать размеры. или наоборот наращивать


        допустим список переменных
            заголовок


===========================================
< metric debug >                values
  RPM                        1000               <- объект который растягивается по горизонтали но не растягивается по вертикали
  ...

    все это список который также растягивается по вертикали и горизонтали, но для себя ему важно знать высоту компонента


    есть либо привязка влево вправо в центре, либо весь доступный обьем



    какие есть виджеты
        часы
            нет заголовка, посути несколько показометров

        дашборд
            какието показометры както показывают. както параметрируются. нам пофиг на заголовок
            несколько ??? экранов, в каждом сетка, в каждой сетке сетка, произвольно
            будем читать конфиг из файла и будем создавать рекурсивное количество сеток по файлу. надо чтобы стрелки выбирали нормально плитки

        маршруты
            заголовок
                может быть список маршрутов, может быть история по топливу и по поездкам
            остальное список элементов, можно листать вверх вниз
            в каждом есть опция сбросить

        список переменных диагностика
            заголовок
                может быть значения и переключатели
            остальное список переменных, можно листать вверх вниз
            в каждом есть опция на весь экран

        дамп
            заголовок
                может быть ECU SRS ABS
            остальное - текстовое поле максимальных размеров

        настройки
            заголовок точно должен быть
                может быть время дата часовой пояс
                может быть настройка одометра

        Сервис
            логи
            файлы в файловой системе
            карта памяти информация
            о системе




        графики - текстовые поля которые меняют координаты и меняют текст, линиию дохуя чего. весь обьем но надо минимум чтото, но это поебать


    нет возможности открыть на весь экран новое окно. развернуть какойто пункт сохранив контекст






    либо весь обьем либо ограничен.

        все экраны это весь обьем
        заголовок отрезали - остальное под виджет
        окно открыли половину напримре - все внутри под виджет


        мы всегда работаем сверху вниз - от экрана отрезаем кусочки и дальше что осталось. такое всегда!

        что осталось это какието списки или сетки с меняющимся количеоством компонентов, часто однотипных



    для того чтобы переопределять окно у тебя должны появиться типы виджетов в дереве

    например не всегда нужны дочерние
    или не всегда нужны

    допустим главыный экран - это где часы

    нажимаем кнопку или появляется ответ от DLC - полностью заменяем окно чемто новым
    там есть заголовок ?ё



    у тебя 


    мне нужно сделать чтобы не пропадал размер окна. тоесть не должно быть присвоение размеров снизу вверх


    */



#include <stdio.h>

#include "emu_tc.h"
#include "emu_storage.h"
#include "dlc_emu.h"

#include "storage.h"
#include "date_time.h"
#include "rtc.h"

#include "dlc_poll.h"
#include "routes.h"


#include "widget_dlc_dump.h"
#include "widget_screen_switch.h"

#include "widget_selectable_list.h"
#include "widget_metric_list_item.h"
#include "widget_menu_screen.h"


#include "widget_dash.h"

static const ui_node_desc_t dash = {
    .widget = &widget_screen_switch,
    .cfg = &(widget_screen_switch_cfg_t){
        .screens_num = 2,
        .screens_list = (ui_node_desc_t[]){
            {
                .widget = &widget_dash
            },
        }
    }
};

static void draw(ui_element_t * el)
{
    ui_element_t * item = ui_tree_add(el, &dash, 0);
    ui_tree_element_draw(item);
    ui_tree_element_select(item, 1);
}

#include "event_list.h"

static unsigned process(ui_element_t * el, unsigned event)
{
    printf("process %d\n", event);
    if (event == EVENT_BTN_MODE) {
        ui_tree_delete_childs(el);
        ui_element_t * item = ui_tree_add(el, &node_widget_menu_screen, 0);
        ui_tree_element_draw(item);
        ui_tree_element_select(item, 1);
        return 1;
    }
    if (event == EVENT_BTN_BACK) {
        ui_tree_delete_childs(el);
        draw(el);
        return 1;
    }
    return 0;
}

const ui_node_desc_t ui = {
    .widget = &(widget_desc_t) {
        .draw = draw,
        .process_event = process
    }
};


unsigned br = 5;
void ui_set_brightness(unsigned br)
{
    printf("ui set br %d\r\n", br);
}

int timezone_s = 5 * 60 * 60;

int time_zone_get(void)
{
    return timezone_s;
}

void time_zone_set(int tz)
{
    timezone_s = tz;
}

int main()
{
    printf("test tc ui\r\n");

    init_rtc();
    rtc_set_time_s(date_time_to_s(&(date_t){ .y = 2025, .m = MONTH_MAR, .d = 19 }, &(time_t){ .h = 11, .m = 43, .s = 12 }));

    emu_storage_load();

    storage_init();
    storage_print_info();

    route_load();

    route_trip_start();
    emu_engine_ctrl(1);

    emu_ui_node(&ui);
    return 0;
}
