#pragma once

#include "duktape.h"

duk_int_t duk_pcompile_file(duk_context* context, const char* filename);

duk_int_t duk_peval_file(duk_context* context, const char* filename);
