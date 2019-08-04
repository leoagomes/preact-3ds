#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <3ds/os.h>
#include <time.h>
#include <sys/time.h>

#include <iostream>
#include <memory>

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

    js::api::register_console(context);
    js::api::register_event_loop(context, loop);

    auto eval_result = duk_peval_file(context, "romfs:/index.js");
    if (eval_result == DUK_EXEC_ERROR) {
        printf("error running script: %s\n", duk_to_string(context, -1));
    }
    duk_pop(context);

    httpcContext http;

    auto open_result = httpcOpenContext(&http, HTTPC_METHOD_GET,
        "http://httpstat.us/200?sleep=7000", 0);
    if (R_FAILED(open_result)) {
        printf("error opening http context.\n");
    }
    if (R_FAILED(httpcAddRequestHeaderField(&http, "Accept", "application/json"))) {
        printf("error setting accept header field.\n");
    }
    if (R_FAILED(httpcBeginRequest(&http))) {
        printf("error beginning request.\n");
    }

    loop->register_timer(std::make_shared<events::timer>(
        false,
        1000,
        [&http] (u64 id, void* ud) {
            HTTPC_RequestStatus status;
            if (R_FAILED(httpcGetRequestState(&http, &status))) {
                printf("failed getting request state.\n");
                return;
            }
            printf("request status: %x\n", status);

            if (status == HTTPC_STATUS_DOWNLOAD_READY) {
                printf("download_ready\n");
            }
            if (status == HTTPC_STATUS_REQUEST_IN_PROGRESS) {
                printf("request in progress\n");
            }
        }
    ));

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

    httpcCloseContext(&http);

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
