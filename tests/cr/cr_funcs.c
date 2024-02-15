#include <stdio.h>

#include "cr.h"

cr_create(some_acync_function, 2, {
    printf("this is some_acync_function. thread %d\n", cr_thread);

    cr_lock();

    printf("some_acync_function resumed !!!. thread %d\n", cr_thread);
})

cr_create(some_process, 2, {
    printf("this is some_process. thread %d\n", cr_thread);

    cr_subprocess(&some_acync_function, cr_thread);

    printf("some_process resumed !!!. thread %d\n", cr_thread);
})

int main()
{
    printf("cr create test\n");
    cr_ctx_t some_acync_ctx;

    cr_run(&some_acync_ctx, &some_process, 0);
    cr_run(&some_acync_ctx, &some_process, 1);

    cr_ctx_t * cr_ctx = &some_acync_ctx;

    printf("cr run and locked. ctx %p pos %p\n", cr_ctx, cr_ctx->current->state[0].pos);

    // func_print(&some_acync_function);


    cr_unlock(&some_acync_ctx, 0);

    cr_resume_if_ready(&some_acync_ctx, 0);

    cr_unlock(&some_acync_ctx, 1);

    cr_resume_if_ready(&some_acync_ctx, 1);

    ctx_print(cr_ctx);

    return 0;
}
