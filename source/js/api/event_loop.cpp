#include "js/api/event_loop.hpp"

#include "js/events/event_loop.hpp"
#include "js/duktape/extensions.hpp"

using namespace js::api;

extern "C" {
duk_ret_t set_timer(duk_context* context);
};

void js::api::register_event_loop(duk_context* context) {
    // push heap stash (will be used multiple times)
    duk_push_heap_stash(context);
    auto stash_index = duk_get_top_index(context);

    // create event loop object
    auto event_loop_index = duk_push_object(context);

    // create event repository
    auto repository_index = duk_push_bare_object(context);

    // add repository to the stash
    duk_dup(context, repository_index);
    duk_put_prop_string(context, stash_index, EVENT_REPOSITORY_STASH_ADDRESS);
    // add repository to the event loop object
    duk_put_prop_string(context, repository_index, "repository");

    // add event loop to the stash (prevents it from being collected)
    duk_push_heap_stash(context);
    duk_dup(context, event_loop_index);
    duk_put_prop_string(context, -2, EVENT_LOOP_STASH_ADDRESS);

    // register event loop methods
    duk_function_list_entry methods[] = {
        { "setTimer", set_timer, DUK_VARARGS },
        { NULL, NULL, 0 }
    };
    duk_put_function_list(context, event_loop_index, methods);

    // TODO: add finalizer

    // set event loop global
    duk_put_global_string(context, "EventLoop");

    // remove stash from the stack
    duk_pop(context);
}

duk_ret_t set_timer(duk_context* context) {
    /* setTimer(
     *  oneShot: bool, // whether the timer is oneShot or not (setInterval vs setTimeout)
     *  timeout: number, // the amount of millisseconds to wait
     *  callback: function(...), // the function to call back
     *  ...arguments // the arguments to pass the callback when it is activated
     * )
     */
    auto argument_count = duk_get_top(context);

    if (argument_count < 3) {
        return duk_error(context, DUK_ERR_ERROR,
                         "invalid number of arguments: %d. "
                         "Expected at least: oneShot, timeout and callback.",
                         argument_count);
    }

    bool one_shot = duk_to_boolean(context, 0);
    auto timeout = duk_to_uint(context, 1);
    auto array_index = duk_stack_to_array(context, 2);

    // get event repository
    duk_push_heap_stash(context);
    duk_get_prop_string(context, -1, EVENT_REPOSITORY_STASH_ADDRESS);

    // move repository before [callback, ...arguments] array
    duk_insert(context, array_index);
    auto repository_index = array_index++;

    duk_pop(context); // remove heap stash from the stack

    // TODO:
    // create event identifier
    // add it to repository
    // register event in event loop object
    // drop repository

    return 0;
}
