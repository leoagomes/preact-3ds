#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "duktape.h"

void initialize();
void terminate();

extern "C" duk_ret_t native_print(duk_context* ctx) {
	const char* string;

	string = duk_get_string(ctx, -1);
	printf("%s\n", string);

	return 0;
}

char* read_file(const char* filename) {
	FILE* file;
	char* contents;
	long size;

	file = fopen(filename, "r");
	if (file == nullptr) {
		return nullptr;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	contents = (char*)calloc(size + 1, 1);
	if (contents == nullptr) {
		fclose(file);
		return nullptr;
	}

	fread(contents, 1, size, file);
	fclose(file);
	return contents;
}

void eval_file(duk_context* ctx, const char* filename) {
	char* contents = read_file(filename);
	duk_peval_string_noresult(ctx, contents);
	free(contents);
}

int main(int argc, char* argv[]) {
	initialize();

	duk_context* context = duk_create_heap_default();
	if (context == nullptr) {
		printf("error creating duktape context.\n");
		terminate();
		return 0;
	}

	duk_push_c_function(context, native_print, 1);
	duk_put_global_string(context, "print");

	eval_file(context, "romfs:/index.js");

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
	consoleInit(GFX_BOTTOM, NULL);
	romfsInit();
}

void terminate() {
	romfsExit();
	gfxExit();
}
