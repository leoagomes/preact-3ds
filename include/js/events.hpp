#pragma once

#include "duktape.h"

namespace js::events {
void initialize(duk_context* context);
void process_events(duk_context* context);
};
