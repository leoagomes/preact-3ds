#include "events/util.hpp"

namespace events::util {

u64 get_current_time() {
    return osGetTime();
}

};
