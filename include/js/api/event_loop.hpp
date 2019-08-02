#pragma once

#include "duktape.h"

namespace js::api {

#define EVENT_LOOP_STASH_ADDRESS "el/EventLoop"
#define EVENT_REPOSITORY_STASH_ADDRESS "el/repository"

void register_event_loop(duk_context* context);
};
