#include "events/timer.hpp"

namespace events {

bool timer_min_heap_compare(std::shared_ptr<timer> a, std::shared_ptr<timer> b) {
    return a->get_next_activation() > b->get_next_activation();
}

};
