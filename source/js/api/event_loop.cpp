#include "js/api/event_loop.hpp"

#include "js/duktape/extensions.hpp"

using namespace js::api;

// forward delcarations

extern "C" {
duk_ret_t register_timer(duk_context* context);
duk_ret_t clear_timer(duk_context* context);
};

void timer_callback(u64 id, void* context_ptr);
void push_stash_and_repository(duk_context* context);
void pop_stash_and_repository(duk_context* context);
duk_int_t perform_callback_call(duk_context* context);

void js::api::register_event_loop(duk_context* context, events::loop* loop) {
    duk_push_heap_stash(context);
    auto stash_index = duk_get_top_index(context);

    // add loop to the stash
    duk_push_pointer(context, loop);
    duk_put_prop_string(context, stash_index, LOOP_OBJECT_STASH_ADDRESS);

    // create event loop object
    auto event_loop_index = duk_push_object(context);

    // create repository object
    auto repository_index = duk_push_object(context);
    // register it in event loop
    duk_dup(context, repository_index);
    duk_put_prop_string(context, event_loop_index, "repository");
    // register it in stash
    duk_put_prop_string(context, stash_index, EVENT_REPOSITORY_STASH_ADDRESS);

    // register event loop methods
    duk_function_list_entry methods[] = {
        { "registerTimer", register_timer, DUK_VARARGS },
        { "clearTimer", clear_timer, 1 },
        { nullptr, nullptr, 0 }
    };
    duk_put_function_list(context, event_loop_index, methods);

    // set event loop global
    duk_put_global_string(context, "EventLoop");

    duk_pop(context); // pop stash
}

duk_ret_t register_timer(duk_context* context) {
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
    auto callback_info_index = duk_stack_to_array(context, 2);

    // get heap stash
    duk_push_heap_stash(context);
    auto stash_index = duk_get_top_index(context);

    // get event repository
    auto repository_present = duk_get_prop_string(context, stash_index,
        EVENT_REPOSITORY_STASH_ADDRESS);
    if (!repository_present) {
        return duk_error(
            context,
            DUK_ERR_REFERENCE_ERROR,
            "expected to find the repository in the stash."
        );
    }
    auto repository_index = duk_get_top_index(context);

    // get loop from the stash
    auto loop_present = duk_get_prop_string(context, stash_index,
        LOOP_OBJECT_STASH_ADDRESS);
    if (!loop_present) {
        return duk_error(
            context,
            DUK_ERR_REFERENCE_ERROR,
            "expected to find a loop reference in the stash."
        );
    }

    void* loop_pointer = duk_require_pointer(context, -1);
    duk_pop(context); // remove loop from the stack

    auto* loop = static_cast<events::loop*>(loop_pointer);
    auto id = loop->register_timer(std::make_shared<events::timer>(
        one_shot,
        timeout,
        timer_callback,
        context
    ));

    // swap the repository and callback info
    // callback info must be at the top for put_prop_index to work
    duk_swap(context, repository_index, callback_info_index);
    std::swap(repository_index, callback_info_index);

    // store timer calback info in repository
    // repository[id] = [function, ...args]
    duk_put_prop_index(context, repository_index, id);

    duk_pop_2(context); // remove stash and repository from the stack

    duk_push_uint(context, static_cast<duk_uint_t>(id));

    return 1;
}

void timer_callback(u64 id, void* context_ptr) {
    auto* context = static_cast<duk_context*>(context_ptr);
    auto callback_info_id = static_cast<duk_uarridx_t>(id);

    push_stash_and_repository(context);

    // get event callback info array
    auto callback_info_present = duk_get_prop_index(context, -1,
        callback_info_id);
    if (!callback_info_present) {
        printf("ERROR: repository does not contain callback.\n");
        return;
    }

    // call ecmascript
    auto call_result = perform_callback_call(context);
    if (call_result == DUK_EXEC_ERROR) {
        printf("ERROR executing callback: %s\n", duk_to_string(context, -1));
    }

    // pop stash, repository and function result
    duk_pop_3(context);
}

void push_stash_and_repository(duk_context* context) {
    duk_push_heap_stash(context);
    duk_get_prop_string(context, -1, EVENT_REPOSITORY_STASH_ADDRESS);
}

duk_int_t perform_callback_call(duk_context* context) {
    auto top_is_array = duk_is_array(context, -1);

    if (!top_is_array) {
        return duk_error(
            context,
            DUK_ERR_TYPE_ERROR, 
            "invalid format for callback data."
        );
    }

    auto length = duk_get_length(context, -1);
    duk_expand_array_into_stack(context, -1);

    return duk_pcall(context, length - 1);
}

duk_ret_t clear_timer(duk_context* context) {
    auto id = duk_require_uint(context, 0);

    push_stash_and_repository(context);

    // remove id from repository
    duk_del_prop_index(context, -1, id);
    duk_pop(context); // remove repository from the stack

    // get loop from the stash
    auto loop_present = duk_get_prop_string(context, -1,
        LOOP_OBJECT_STASH_ADDRESS);
    if (!loop_present) {
        return duk_error(
            context,
            DUK_ERR_REFERENCE_ERROR,
            "expected loop object to exist in the stack."
        );
    }
    void* loop_pointer = duk_require_pointer(context, -1);
    duk_pop_2(context); // pop loop and stash

    auto* loop = static_cast<events::loop*>(loop_pointer);
    auto clear_timer_successful = loop->clear_timer(static_cast<u64>(id));

    duk_push_boolean(context, clear_timer_successful);

    return 1;
}
