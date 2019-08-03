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

    js::api::register_console(context);
    js::api::register_event_loop(context, loop);

    auto eval_result = duk_peval_file(context, "romfs:/index.js");
    if (eval_result == DUK_EXEC_ERROR) {
        printf("error running script: %s\n", duk_to_string(context, -1));
    }
    duk_pop(context);

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
}

void terminate() {
    romfsExit();
    gfxExit();
}
