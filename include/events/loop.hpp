#pragma once

#include <vector>
#include <memory>

#include <3ds.h>

#include "events/timer.hpp"

namespace events {

class loop {
private:
    u64 now;
    u64 _timer_id;

    std::vector<std::shared_ptr<timer>> timers;

    u64 get_current_time();

    // timer queue helpers
    u64 next_timer_id();
    void push_timer_heap();
    void pop_timer_heap();
    void make_timer_heap();
    void add_timer(std::shared_ptr<timer> timer);
    std::shared_ptr<timer> pop_timer();
    void reposition_top_timer();

    void run_due_timers();

public:
    loop();
    ~loop();

    u64 register_timer(std::shared_ptr<timer> timer);
    bool clear_timer(u64 id);

    void cycle();
};

};
