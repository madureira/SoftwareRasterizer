#ifndef PLATFORM_KEYBOARD_MAPPING_H
#define PLATFORM_KEYBOARD_MAPPING_H

#include "platform/keyboard.h"

#ifndef __EMSCRIPTEN__

#include <SDL3/SDL.h>

static PlatformKey sdl_scancode_to_platform_key(SDL_Scancode sc)
{
    switch (sc)
    {
    case SDL_SCANCODE_ESCAPE:
        return PLATFORM_KEY_ESCAPE;
    case SDL_SCANCODE_RETURN:
        return PLATFORM_KEY_ENTER;
    case SDL_SCANCODE_BACKSPACE:
        return PLATFORM_KEY_BACKSPACE;
    case SDL_SCANCODE_TAB:
        return PLATFORM_KEY_TAB;
    case SDL_SCANCODE_SPACE:
        return PLATFORM_KEY_SPACE;
    case SDL_SCANCODE_UP:
        return PLATFORM_KEY_UP;
    case SDL_SCANCODE_DOWN:
        return PLATFORM_KEY_DOWN;
    case SDL_SCANCODE_LEFT:
        return PLATFORM_KEY_LEFT;
    case SDL_SCANCODE_RIGHT:
        return PLATFORM_KEY_RIGHT;
    default:
        return PLATFORM_KEY_UNKNOWN;
    }
}

#else // __EMSCRIPTEN__

#include "core/utils.h"

static PlatformKey dom_code_to_platform_key(const char* code)
{
    if (str_equals(code, "Escape"))
    {
        return PLATFORM_KEY_ESCAPE;
    }

    if (str_equals(code, "Enter"))
    {
        return PLATFORM_KEY_ENTER;
    }

    if (str_equals(code, "Backspace"))
    {
        return PLATFORM_KEY_BACKSPACE;
    }

    if (str_equals(code, "Tab"))
    {
        return PLATFORM_KEY_TAB;
    }

    if (str_equals(code, "Space"))
    {
        return PLATFORM_KEY_SPACE;
    }

    if (str_equals(code, "ArrowUp"))
    {
        return PLATFORM_KEY_UP;
    }

    if (str_equals(code, "ArrowDown"))
    {
        return PLATFORM_KEY_DOWN;
    }

    if (str_equals(code, "ArrowLeft"))
    {
        return PLATFORM_KEY_LEFT;
    }

    if (str_equals(code, "ArrowRight"))
    {
        return PLATFORM_KEY_RIGHT;
    }

    return PLATFORM_KEY_UNKNOWN;
}

#endif // __EMSCRIPTEN__

#endif // PLATFORM_KEYBOARD_MAPPING_H
