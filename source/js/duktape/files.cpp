#include "js/duktape/files.hpp"

#include "helpers.hpp"

#include <cstdio>

duk_int_t duk_pcompile_file(duk_context* context, duk_int_t flags, const char* filename) {
    std::string source = helpers::read_file(filename);

    duk_push_string(context, source.c_str());
    duk_push_string(context, filename);

    return duk_pcompile(context, 0);
}

duk_int_t duk_peval_file(duk_context* context, const char* filename) {
    auto compilation_result = duk_pcompile_file(context, 0, filename);

    if (compilation_result != 0) return compilation_result;

    return duk_pcall(context, 0);
}
