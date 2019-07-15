#include "js/events/timer.hpp"

using namespace js::events;
using std::string;

timer::timer(int id, uint64_t interval, uint64_t timestamp, string& callback_id,
    bool one_shot) : id(id), interval(interval), one_shot(one_shot) {
    update_activation(timestamp);
}

timer::~timer() {
}

void timer::update_activation(uint64_t timestamp) {
    next_activation = timestamp + interval;
}
