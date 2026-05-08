/*
    execution queue

    передача потока управления в background контекст.
    кольцевой буфер на EQ_SIZE элементов (см. eq.c).

    eq_func_single(f)    - положить отложенный вызов f()
    eq_func_idx(f, idx)  - положить отложенный вызов f(idx)
    eq_process()         - в background достаёт и вызывает один
                           элемент, возвращает 1 если что-то вызвал.
                           вызывать в цикле пока не вернёт 0.

    при переполнении новые элементы молча отбрасываются.
*/

void eq_func_single(void (*func)(void));
void eq_func_idx(void (*func)(unsigned), unsigned idx);

unsigned eq_process(void);
