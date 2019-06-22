#include "js/js.hpp"
#include "js/basics.hpp"

duk_context* context = nullptr;

bool js_initialize() {
    context = duk_create_heap_default();
    return context != nullptr;
}

void js_terminate() {
    if (context == nullptr) return;

    duk_destroy_heap(context);
}

bool js_register_all() {
    
}
