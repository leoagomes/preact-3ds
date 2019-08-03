#include "events/loop.hpp"

#include <algorithm>

namespace events {

loop::loop() : _timer_id(1), timers() {
}

loop::~loop() {}

u64 loop::get_current_time() { return osGetTime(); }

void loop::cycle() {
    now = get_current_time();
    run_due_timers();
}

u64 loop::next_timer_id() { return _timer_id++; }
void loop::push_timer_heap() {
    std::push_heap(timers.begin(), timers.end(), timer_min_heap_compare);
}
void loop::pop_timer_heap() {
    std::pop_heap(timers.begin(), timers.end(), timer_min_heap_compare);
}
void loop::make_timer_heap() {
    std::make_heap(timers.begin(), timers.end(), timer_min_heap_compare);
}
void loop::add_timer(std::shared_ptr<timer> timer) {
    timers.push_back(timer);
    push_timer_heap();
}
void loop::pop_timer() {
    pop_timer_heap();
    timers.pop_back();
}
void loop::reposition_top_timer() {
    pop_timer_heap();
    push_timer_heap();
}

u64 loop::register_timer(std::shared_ptr<timer> timer) {
    auto id = next_timer_id();
    u64 next_activation = get_current_time() + timer->get_interval();

    timer->set_id(id);
    timer->set_next_activation(next_activation);

    add_timer(timer);

    return id;
}
bool loop::clear_timer(u64 id) {
    for (auto it = timers.begin(); it != timers.end(); it++) {
        auto timer = *it;

        if (timer->get_id() == id) {
            timers.erase(it);
            make_timer_heap();
            return true;
        }
    }

    return false;
}

void loop::run_due_timers() {
    std::shared_ptr<timer> timer;

    while (!timers.empty()
        && (timer = timers.front()) != nullptr
        && timer->get_next_activation() < now) {

        timer->execute();

        if (timer->is_one_shot()) {
            pop_timer();
        } else {
            auto next_activation = now + timer->get_interval();
            timer->set_next_activation(next_activation);
            reposition_top_timer();
        }
    }
}

};
