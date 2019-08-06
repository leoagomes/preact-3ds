#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <3ds/os.h>
#include <time.h>
#include <sys/time.h>

#include <iostream>

#include "duktape.h"

#include "js/duktape/extensions.hpp"
#include "js/duktape/files.hpp"
#include "js/api/console.hpp"
#include "js/api/event_loop.hpp"

#include "events.hpp"

#include "helpers.hpp"

void initialize();
void terminate();

int main(int argc, char* argv[]) {
    initialize();

    duk_context* context;
    events::loop* loop = new events::loop();

    context = duk_create_heap_default();
    if (context == nullptr) {
        printf("error with duktape\n");
        return 0;
    }

    // js::api::register_console(context);
    // js::api::register_event_loop(context, loop);

    // auto eval_result = duk_peval_file(context, "romfs:/index.js");
    // if (eval_result == DUK_EXEC_ERROR) {
    //     printf("error running script: %s\n", duk_to_string(context, -1));
    // }
    // duk_pop(context);

    auto req = std::make_shared<events::http::http>();
    req->on_abort = [] (events::http::http& context) {
        printf("request aborted.\n");
    };
    req->on_failure = [] (events::http::http& context, events::http::point_of_failure point) {
        printf("request failed: %d\n", static_cast<int>(point));
    };
    req->on_state_changed = [] (events::http::http& context, events::http::request_state state) {
        printf("request state changed... current state: %d\n", static_cast<int>(state));
    };
    req->on_timeout = [] (events::http::http& context) {
        printf("request timed out.\n");
    };
    if (req->open(events::http::method::get, "http://httpstat.us/200")) {
        printf("failed opening request.\n");
    }
    if (req->begin()) {
        printf("error beginning request.\n");
    }
    loop->register_request(req);

    loop->register_timer(std::make_shared<events::timer>(
        true,
        10000,
        [req] (u64 id, void* ud) {
            printf("aborting request.\n");
            req->abort();
        }
    ));

    // if left to out of scope destruction, httpcContextClose will be called
    // _after_ httpcExit(). This causes memory corruption and the OS crashes.
    // setting req to null releases the ownership of the request pointer
    // before running httpcExit
    req = nullptr;

    printf("entering loop...\n");

    while (aptMainLoop())
    {
        gspWaitForVBlank();
        gfxSwapBuffers();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break;

        loop->cycle();
    }

    delete loop;

    terminate();
    return 0;
}

void initialize() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    romfsInit();
    httpcInit(0x2000);
}

void terminate() {
    httpcExit();
    romfsExit();
    gfxExit();
}
