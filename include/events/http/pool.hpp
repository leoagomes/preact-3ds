#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <queue>

#include <3ds.h>

namespace events::http {

#define DEFAULT_HTTP_POOL_SIZE 2

class pool {
private:
    Thread main_thread;
    std::vector<Thread> threads;
    bool active, failed;
    Handle semaphore;

    void create_semaphore();
    void create_threads(size_t count);

    void handle_failure();

public:
    pool(size_t size = DEFAULT_HTTP_POOL_SIZE);
    ~pool();

    void mark_thread_failure(Thread id);

    bool is_active();
    bool has_failed();
    Handle get_semaphore();
};

};
