/*
    execution queue
    передача потока управления в background контекст
*/

void eq_func_single(void (*func)(void));
void eq_func_idx(void (*func)(unsigned), unsigned idx);

void eq_process(void);
