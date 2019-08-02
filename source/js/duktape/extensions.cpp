#include "js/duktape/extensions.hpp"

/*
 * Transforms elements from an index of the stack up until its top into an array.
 */
duk_idx_t duk_stack_to_array(duk_context* context, duk_idx_t starting_index) {
    // get _actual_ index... supports negative indices.
    starting_index = duk_require_normalize_index(context, starting_index);
    duk_idx_t array_object_index = starting_index;

    duk_push_array(context); // create array
    duk_insert(context, starting_index); // place it in the given index

    auto top = duk_get_top_index(context); // index of last element

    for (auto stack_index = top; stack_index > array_object_index; stack_index--) {
        // index of element inside array
        auto element_array_index = stack_index - array_object_index - 1;

        // put element in array
        duk_put_prop_index(context, array_object_index, element_array_index);
    }

    // return array index (which is a bit redundant, but well)
    return array_object_index;
}

/*
 * Expands an array, pushing all of its elements to the stack.
 */
void duk_expand_array_into_stack(duk_context* context, duk_idx_t array_index) {
    // get _actual_ index... supports negative indices.
    array_index = duk_require_normalize_index(context, array_index);

    // get array length
    auto array_length = duk_get_length(context, array_index);

    for (auto index = 0; index < array_length; index++) {
        // push array element to the stack
        duk_get_prop_index(context, array_index, index);
    }

    // remove array from the stack
    duk_remove(context, array_index);
}
