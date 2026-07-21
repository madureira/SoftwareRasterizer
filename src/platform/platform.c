#include "platform/platform.h"

#ifndef __EMSCRIPTEN__

#include <stdlib.h>

#include <SDL3/SDL.h>

struct PlatformWindow
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int32 texture_width;
    int32 texture_height;
};

bool platform_init(void)
{
    return SDL_Init(SDL_INIT_VIDEO);
}

void platform_shutdown(void)
{
    SDL_Quit();
}

PlatformWindow* platform_window_create(const char* title, int32 width, int32 height, bool resizable)
{
    SDL_WindowFlags flags = 0;

    if (resizable)
    {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    PlatformWindow* win = malloc(sizeof(*win));
    if (win == NULL)
    {
        return NULL;
    }

    win->window = SDL_CreateWindow(title, width, height, flags);
    if (win->window == NULL)
    {
        free(win);
        return NULL;
    }

    win->renderer = SDL_CreateRenderer(win->window, NULL);
    if (win->renderer == NULL)
    {
        SDL_DestroyWindow(win->window);
        free(win);
        return NULL;
    }

    win->texture =
        SDL_CreateTexture(win->renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (win->texture == NULL)
    {
        SDL_DestroyRenderer(win->renderer);
        SDL_DestroyWindow(win->window);
        free(win);
        return NULL;
    }

    win->texture_width = width;
    win->texture_height = height;

    return win;
}

void platform_window_destroy(PlatformWindow* window)
{
    if (window == NULL)
    {
        return;
    }

    SDL_DestroyTexture(window->texture);
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
    free(window);
}

void platform_window_set_title(PlatformWindow* window, const char* title)
{
    if (window == NULL)
    {
        return;
    }

    SDL_SetWindowTitle(window->window, title);
}

void platform_window_present(PlatformWindow* window, const uint32* pixels, int32 width, int32 height)
{
    if (window == NULL || pixels == NULL)
    {
        return;
    }

    if (width != window->texture_width || height != window->texture_height)
    {
        SDL_DestroyTexture(window->texture);
        window->texture =
            SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (window->texture == NULL)
        {
            return;
        }

        window->texture_width = width;
        window->texture_height = height;
    }

    SDL_UpdateTexture(window->texture, NULL, pixels, width * (int)sizeof(uint32));
    SDL_RenderTexture(window->renderer, window->texture, NULL, NULL);
    SDL_RenderPresent(window->renderer);
}

bool platform_poll_event(PlatformEvent* event)
{
    SDL_Event sdl_event;

    if (event == NULL)
    {
        return false;
    }

    if (!SDL_PollEvent(&sdl_event))
    {
        return false;
    }

    switch (sdl_event.type)
    {
    case SDL_EVENT_QUIT:
        event->type = PLATFORM_EVENT_QUIT;
        break;

    case SDL_EVENT_WINDOW_RESIZED:
        event->type = PLATFORM_EVENT_WINDOW_RESIZED;
        event->data.window.width = sdl_event.window.data1;
        event->data.window.height = sdl_event.window.data2;
        break;

    case SDL_EVENT_KEY_DOWN:
        event->type = PLATFORM_EVENT_KEY_DOWN;
        break;

    case SDL_EVENT_KEY_UP:
        event->type = PLATFORM_EVENT_KEY_UP;
        break;

    default:
        event->type = PLATFORM_EVENT_NONE;
        break;
    }

    return true;
}

void platform_run_main_loop(PlatformFrameCallback frame_cb, void* user_data)
{
    while (frame_cb(user_data))
    {
    }
}

float64 platform_get_time_seconds(void)
{
    return (float64)SDL_GetTicks() / 1000.0;
}

#else // __EMSCRIPTEN__

#include <stdlib.h>

#include <emscripten.h>
#include <emscripten/html5.h>

// Implemented in platform_web.js, linked via --js-library.
extern void platform_js_set_title(const char* title);
extern void platform_js_present(const uint32* pixels, int32 width, int32 height);

typedef struct
{
    PlatformFrameCallback cb;
    void* user_data;
} EmscriptenLoopState;

struct PlatformWindow
{
    int32 width;
    int32 height;
};

static EmscriptenLoopState g_loop_state;

bool platform_init(void)
{
    return true;
}

void platform_shutdown(void)
{
}

PlatformWindow* platform_window_create(const char* title, int32 width, int32 height, bool resizable)
{
    (void)title;
    (void)resizable;

    PlatformWindow* win = malloc(sizeof(*win));
    if (win == NULL)
    {
        return NULL;
    }

    win->width = width;
    win->height = height;

    emscripten_set_canvas_element_size("#canvas", width, height);

    return win;
}

void platform_window_destroy(PlatformWindow* window)
{
    free(window);
}

void platform_window_set_title(PlatformWindow* window, const char* title)
{
    (void)window;
    platform_js_set_title(title);
}

void platform_window_present(PlatformWindow* window, const uint32* pixels, int32 width, int32 height)
{
    if (window == NULL || pixels == NULL)
    {
        return;
    }

    platform_js_present(pixels, width, height);
}

bool platform_poll_event(PlatformEvent* event)
{
    if (event == NULL)
    {
        return false;
    }

    event->type = PLATFORM_EVENT_NONE;
    return false;
}

static void emscripten_frame_wrapper(void)
{
    if (!g_loop_state.cb(g_loop_state.user_data))
    {
        emscripten_cancel_main_loop();
    }
}

void platform_run_main_loop(PlatformFrameCallback frame_cb, void* user_data)
{
    g_loop_state.cb = frame_cb;
    g_loop_state.user_data = user_data;
    emscripten_set_main_loop(emscripten_frame_wrapper, 0, 1);
}

float64 platform_get_time_seconds(void)
{
    return emscripten_get_now() / 1000.0;
}

#endif // __EMSCRIPTEN__
