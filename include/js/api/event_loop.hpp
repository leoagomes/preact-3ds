#pragma once

#include "duktape.h"

namespace js::api {
void register_event_loop(duk_context* context);
};
