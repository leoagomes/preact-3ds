#pragma once

#include <cstdint>
#include <string>

#include "duktape.h"

namespace js::events {
class timer {
private:
    int id;
    uint64_t interval, next_activation;
    std::string callback_id;
    bool one_shot, active;

    timer(duk_context* context, int id, uint64_t interval, uint64_t timestamp,
        std::string& callback_id, bool one_shot);
    ~timer();

    void update_activation(uint64_t timestamp);
    void activate();
};
};


