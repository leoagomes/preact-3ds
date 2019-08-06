#pragma once

#include <string>
#include <functional>
#include <vector>

#include <3ds.h>

namespace events::http {

enum class request_state {
    invalid = 0x00,
    idle = 0x01,
    started = 0x02,
    in_progress = 0x05,
    available = 0x07,
    ready = 0x08,
    downloaded = 0x09,
    timed_out = 0x0A,
    unknown,
    failed = 0xFF
};

enum class method {
    get = HTTPC_METHOD_GET,
    post = HTTPC_METHOD_POST,
    head = HTTPC_METHOD_HEAD,
    put = HTTPC_METHOD_PUT,
    delet = HTTPC_METHOD_DELETE,
};

enum class point_of_failure {
    update_state
};

class http;

typedef std::function<void(http&, request_state)> state_changed_callback_t;
typedef std::function<void(http&, point_of_failure)> failure_callback_t;
typedef std::function<void(http&)> callback_t;

class http {
private:
    httpcContext context;
    request_state _state, previous_state;
    u32 content_size, download_size;
    bool done, failed, timeouted, aborted;
    u64 timeout, timeout_time;
    std::vector<u8> response_data;

    // callback wrappers
    void call_on_failure(point_of_failure point);
    void call_on_state_changed(request_state state);
    void call_on_timeout();
    void call_on_abort();

    // helpers
    void handle_failure(point_of_failure point);
    bool should_timeout(u64 current_time);
    void handle_timeout();
    void terminate();
    void download_response_page();

public:
    size_t page_download_size = 0x1000;

    failure_callback_t on_failure;
    state_changed_callback_t on_state_changed;
    callback_t on_timeout;
    callback_t on_abort;

    http();
    ~http();

    void update(); // gets current time from the os
    void update(u64 current_time);

    bool open(method method, const std::string& url,
        bool use_default_proxy = false);
    bool begin();
    void abort();

    request_state get_state();

    // getters & setters

    const bool is_done();
    const bool has_failed();

    const u64 get_timeout();
    const u64 get_timeout_time();
    void set_timeout(u64 timeout);
};

};
