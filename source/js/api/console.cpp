#include "js/api/console.hpp"

int console_log(duk_context* context) {
    int argument_count = duk_get_top(context);

    for (int arg = 0; arg < argument_count; arg++) {
        const char* argument_string = duk_safe_to_string(context, arg);
        printf("%s\t", argument_string);
    }
    putchar('\n');

    return 0;
}

void js::api::register_console(duk_context* context) {
    const duk_function_list_entry functions[] = {
        { "log", console_log, DUK_VARARGS },
        { nullptr, nullptr, 0 }
    };

    duk_push_object(context);
    duk_put_function_list(context, -1, functions);
    duk_put_global_string(context, "console");
}
