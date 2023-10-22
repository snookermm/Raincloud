#include <klibc.h>
#include <context.h>

context_t *save_context;
context_t *load_context;

#define CONTEXT_REGISTER_COUNT (sizeof(context_t) / sizeof(uint64_t))

void __context_save(uint64_t *stack_top)
{
    if (save_context == NULL || stack_top == NULL) {
        return;
    }

    uint64_t *save_location = (uint64_t *)save_context;

    for (int i = 0; i <= CONTEXT_REGISTER_COUNT; i++)
    {
        if (i == 17)
        {
            stack_top++;
        }
        else
        {
            *save_location++ = *stack_top++;
        }
    }
}

void __context_load(uint64_t *stack_top)
{
    if (load_context == NULL || stack_top == NULL) {
        return;
    }

    uint64_t *load_location = (uint64_t *)load_context;

    for (int i = 0; i <= CONTEXT_REGISTER_COUNT; i++)
    {
        if (i == 17)
        {
            stack_top++;
        }
        else
        {
            *stack_top++ = *load_location++;
        }
    }
}
