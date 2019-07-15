#include "js/events/event_loop.hpp"

using namespace js::events;

event_loop::event_loop(duk_context* context) : context(context),
    timer_queue(), microtask_queue() {
}

event_loop::~event_loop() {
}
