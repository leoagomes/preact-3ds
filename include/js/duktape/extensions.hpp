#pragma once

#include "duktape.h"

/*
 * Transforms elements from an index of the stack up until its top into an array.
 */
duk_idx_t duk_stack_to_array(duk_context* context, duk_idx_t starting_index);

/*
 * Expands an array, pushing all of its elements to the stack.
 */
void duk_expand_array_into_stack(duk_context* context, duk_idx_t array_index);
