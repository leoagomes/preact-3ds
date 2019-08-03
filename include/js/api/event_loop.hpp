#pragma once

#include "duktape.h"

#include "events.hpp"

namespace js::api {

#define EVENT_REPOSITORY_STASH_ADDRESS "el/repository"
#define LOOP_OBJECT_STASH_ADDRESS "el/loop"

void register_event_loop(duk_context* context, events::loop* loop);
};
