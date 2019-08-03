#pragma once

#include <functional>
#include <memory>

#include <3ds.h>

namespace events {

typedef std::function<void(u64, void*)> timer_callback_t;

class timer {
private:
    u64 id;
    u64 interval, next_activation;
    bool one_shot;
    timer_callback_t callback;
    void* userdata;

public:
    timer(
        bool one_shot,
        u64 interval,
        timer_callback_t callback,
        void* userdata
    ) : interval(interval), one_shot(one_shot), callback(callback),
        userdata(userdata) {}
    ~timer() {}

    u64 get_id() { return id; }
    // DO NOT USE THIS FUNCTION
    void set_id(u64 id) { this->id = id; }

    u64 get_interval() { return interval; }
    u64 get_next_activation() { return next_activation; }
    void set_next_activation(u64 next_activation) {
        this->next_activation = next_activation;
    }
    timer_callback_t& get_callback() { return callback; }
    void* get_userdata() { return userdata; }
    bool is_one_shot() { return one_shot; }

    void execute() { callback(id, userdata); }
};

bool timer_min_heap_compare(std::shared_ptr<timer> a, std::shared_ptr<timer> b);

};
