#pragma once

#include "duktape.h"

extern duk_context* js_context;

bool js_initialize();
void js_termiante();

bool js_register_all();
