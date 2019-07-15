#pragma once

#include "duktape.h"

namespace js::api {
void register_console(duk_context* context);
};
