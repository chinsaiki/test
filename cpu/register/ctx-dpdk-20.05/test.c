#include "ctx.h"

int main()
{
    struct ctx new_ctx, curr_ctx;
    ctx_switch(&new_ctx, &curr_ctx); //会产生段错误
}
