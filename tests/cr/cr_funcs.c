#include <stdio.h>

#include "cr.h"

cr_create(some_acync_function, 2, {
    printf("this is some_acync_function. thread %d\n", cr_ctx->thread);

    cr_lock();

    printf("some_acync_function resumed !!!. thread %d\n", cr_ctx->thread);
})

cr_create(some_process, 2, {
    printf("this is some_process. thread %d\n", cr_ctx->thread);

    cr_subprocess(&some_acync_function, cr_ctx->thread);

    printf("some_process resumed !!!. thread %d\n", cr_ctx->thread);
})

cr_ctx_t some_acync_ctx_0;
cr_ctx_t some_acync_ctx_1;

int main()
{
    printf("cr create test\n");

    cr_run(&some_acync_ctx_0, &some_process, 0);
    cr_run(&some_acync_ctx_1, &some_process, 1);

    cr_unlock(&some_acync_ctx_1);
    cr_resume_if_ready(&some_acync_ctx_1);

    cr_unlock(&some_acync_ctx_0);

    cr_resume_if_ready(&some_acync_ctx_0);

    return 0;
}
