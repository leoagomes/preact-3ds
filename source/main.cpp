#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <time.h>
#include <sys/time.h>

#include "duktape.h"

void initialize();
void terminate();

bool runThread = true;
Handle threadTimer;
Thread threadHandle;

void threadMain(void* arg) {
	while (runThread) {
		Result res = svcWaitSynchronization(threadTimer, U64_MAX);
		svcClearTimer(threadTimer);

		auto tick = svcGetSystemTick();

		timeval t;
		if (0 == gettimeofday(&t, nullptr)) {
			printf("microsecs: %ld; secs: %lld\n", t.tv_usec, t.tv_sec);
		}

		printf("Timer! %lld %llx\n", tick, tick);
	}
}

int main(int argc, char* argv[]) {
	initialize();

	printf("Creating timer...");
	svcCreateTimer(&threadTimer, RESET_ONESHOT);
	svcSetTimer(threadTimer, 0, 1000000000);
	printf(" OK!\n");

	printf("Starting thread...");
	threadHandle = threadCreate(threadMain, nullptr, 4 * 1024, 0x18, -2, true);
	printf(" OK!\n");

	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;
	}

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
