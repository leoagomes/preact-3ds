#include "events/http/pool.hpp"
#include "events/http.hpp"

#define POOL_STACK_SIZE 0x1000

extern "C" {
void thread_handler(void*);
};

namespace events::http {

pool::pool(std::size_t size) : threads(), active(true), failed(false) {
    create_semaphore();
    create_threads(size);
}

pool::~pool() {
}

void pool::create_semaphore() {
    auto result = svcCreateSemaphore(&semaphore, 0, INT32_MAX);
    if (R_FAILED(result))
        handle_failure();
}

void pool::create_threads(size_t count) {
    threads.reserve(threads.size() + count);

    s32 thread_priority;
    if (R_FAILED(svcGetThreadPriority(&thread_priority, CUR_THREAD_HANDLE)))
        return;

    for (auto i = 0; i < count; i++) {
        Thread thread = threadCreate(thread_handler, this, POOL_STACK_SIZE,
            thread_priority - 1, -2, true);
        threads.push_back(thread);
    }
}

void pool::handle_failure() {
    active = false;
}

void pool::mark_thread_failure(Thread id) {
    // TODO: implement
}

// getters & setters

bool pool::is_active() {
    return active;
}

bool pool::has_failed() {
    return failed;
}

Handle pool::get_semaphore() {
    return semaphore;
}

};

void thread_handler(void* _pool) {
    auto pool = static_cast<events::http::pool*>(_pool);
    auto current_thread = threadGetCurrent();

    while (pool->is_active()) {
        auto result = svcWaitSynchronization(pool->get_semaphore(), U64_MAX);
        if (R_FAILED(result)) {
            pool->mark_thread_failure(current_thread);
            return;
        }
        if (result) // timeout
            continue;
        if (!pool->is_active())
            return;

        
    }
}
