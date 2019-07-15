#pragma once

#include <vector>
#include <queue>
#include <list>
#include <memory>

#include "js/events/timer.hpp"
#include "js/events/microtask.hpp"

namespace js::events {
class event_loop {
private:
    duk_context* context;

    std::vector<std::unique_ptr<timer>> timer_queue;
    std::queue<std::unique_ptr<microtask>> microtask_queue;

public:
    event_loop(duk_context* context);
    ~event_loop();

};
