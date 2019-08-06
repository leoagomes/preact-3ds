#include "events/http.hpp"

#include "events/util.hpp"

#include <algorithm>
#include <cstdio>

namespace events::http {

HTTPC_RequestMethod convert_request_method(method http_method);

http::http() :
    _state(request_state::idle),
    previous_state(request_state::idle),
    content_size(0),
    download_size(0),
    done(false),
    failed(false),
    timeouted(false),
    aborted(false),
    timeout(0),
    timeout_time(0)
{
}

http::~http() {
    // TODO: check if cancelled contexts should not be closed
    httpcCloseContext(&context);
}

void http::update() {
    update(util::get_current_time());
}

void http::update(u64 current_time) {
    if (done) return;

    if (_state == request_state::ready) {
        download_response_page();
        // check state change
        // update state
        // return
    }

    if (_state == request_state::downloaded) {
        // call on loaded callback
        // terminate
    }

    // check timeout
    if (_state == request_state::timed_out ||
        should_timeout(current_time))
        return handle_timeout();

    // update request state
    _state = get_state();
    if (_state == request_state::failed)
        return handle_failure(point_of_failure::update_state);
    if (_state == request_state::timed_out)
        return handle_timeout();

    if (_state != previous_state)
        call_on_state_changed(_state);
    previous_state = _state;
}

bool http::should_timeout(u64 current_time) {
    return timeout_time != 0
        && current_time > timeout_time;
}

void http::handle_failure(point_of_failure point) {
    failed = true;
    call_on_failure(point);
    terminate();
}

void http::handle_timeout() {
    timeouted = true;
    call_on_timeout();
    httpcCancelConnection(&context);
    terminate();
}

void http::download_response_page() {
    Result result;
    u32 download_size, content_size, downloaded_size;
    size_t page_download_size = this->page_download_size;

    result = httpcGetDownloadSizeState(&context, &download_size, &content_size);
    if (R_FAILED(result)) {
        _state = request_state::failed;
        return;
    }

    if (content_size != 0)
        page_download_size = std::min<size_t>(page_download_size, content_size);

    size_t data_size = response_data.size();
    response_data.resize(data_size + page_download_size);

    result = httpcDownloadData(&context, response_data.data() + data_size,
        page_download_size, &downloaded_size);
    if (result == 0) {
        _state = request_state::downloaded;
        response_data.resize(data_size + downloaded_size);
    } else if (result != HTTPC_RESULTCODE_DOWNLOADPENDING &&
        result != 0) {
        _state = request_state::failed;
        return;
    }
}

void http::terminate() {
    done = true;
}

bool http::open(method method, const std::string& url, bool use_default_proxy) {
    auto httpc_method = convert_request_method(method);
    auto url_string = url.c_str();

    return R_FAILED(httpcOpenContext(
        &context,
        httpc_method,
        url_string,
        (u32)use_default_proxy
    ));
}

bool http::begin() {
    return R_FAILED(httpcBeginRequest(&context));
}

void http::abort() {
    aborted = true;
    call_on_abort();
    httpcCancelConnection(&context);
    terminate();
}

request_state http::get_state() {
    HTTPC_RequestStatus status;

    if (R_FAILED(httpcGetRequestState(&context, &status)))
        return request_state::failed;

    switch(status) {
    case 0x01: return request_state::idle;
    case 0x02: return request_state::started;
    case 0x05: return request_state::in_progress;
    case 0x07: return request_state::available;
    case 0x08: return request_state::ready;
    case 0x0A: return request_state::timed_out;
    default: return request_state::unknown;
    }
}

// callback wrappers

void http::call_on_failure(point_of_failure point) {
    if (on_failure)
        on_failure(*this, point);
}

void http::call_on_state_changed(request_state state) {
    if (on_state_changed)
        on_state_changed(*this, state);
}

void http::call_on_timeout() {
    if (on_timeout)
        on_timeout(*this);
}

void http::call_on_abort() {
    if (on_abort)
        on_abort(*this);
}

// getters & setters

const bool http::is_done() {
    return done;
}

const bool http::has_failed() {
    return failed;
}

const u64 http::get_timeout() {
    return timeout;
}

const u64 http::get_timeout_time() {
    return timeout_time;
}

void http::set_timeout(u64 timeout) {
    timeout = timeout;
    timeout_time = util::get_current_time() + timeout;
}

// other helpful functions

HTTPC_RequestMethod convert_request_method(method http_method) {
    switch (http_method) {
    case method::get: return HTTPC_METHOD_GET;
    case method::post: return HTTPC_METHOD_POST;
    case method::put: return HTTPC_METHOD_PUT;
    case method::delet: return HTTPC_METHOD_DELETE;
    default: return HTTPC_METHOD_GET;
    }
}

};

