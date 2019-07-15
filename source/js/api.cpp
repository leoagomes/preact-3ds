#include "js/api.hpp"

#include "js/api/console.hpp"

void js::register_api(duk_context* context) {
    js::api::register_console(context);
}
