#include "platform/platform.h"

#ifndef __EMSCRIPTEN__

#include <stdlib.h>

#include <SDL3/SDL.h>

struct PlatformWindow
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    i32 texture_width;
    i32 texture_height;
};

static PlatformWindow* g_window = NULL;
static i32 g_display_fps = 0;

bool platform_init(void)
{
    return SDL_Init(SDL_INIT_VIDEO);
}

void platform_shutdown(void)
{
    SDL_Quit();
}

PlatformWindow* platform_window_create(const char* title, i32 width, i32 height, i32 display_index,
                                       i32 min_width, i32 min_height, bool resizable,
                                       bool fullscreen, bool vsync, const char* canvas_id)
{
    (void)canvas_id;
    SDL_WindowFlags flags = 0;

    if (resizable)
    {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    if (fullscreen)
    {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    PlatformWindow* win = malloc(sizeof(*win));
    if (win == NULL)
    {
        return NULL;
    }

    // Resolve the target display: clamp index to available displays.
    int display_count = 0;
    SDL_DisplayID* displays = SDL_GetDisplays(&display_count);
    SDL_DisplayID target_display =
        (displays != NULL && display_index < display_count) ? displays[display_index] : 0;
    SDL_free(displays);

    // SDL_WINDOWPOS_CENTERED_DISPLAY(0) falls back to the primary display.
    const int centered = SDL_WINDOWPOS_CENTERED_DISPLAY(target_display);

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, centered);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, centered);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, (Sint64)flags);
    win->window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (win->window == NULL)
    {
        free(win);
        return NULL;
    }

    SDL_SetWindowMinimumSize(win->window, min_width, min_height);

    const SDL_DisplayID display = SDL_GetDisplayForWindow(win->window);
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
    g_display_fps =
        (mode != NULL && mode->refresh_rate > 0.0f) ? (i32)(mode->refresh_rate + 0.5f) : 60;

    // Hint must be set before SDL_CreateRenderer so backends like Metal
    // create their swap chain with VSync enabled from the start.
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, vsync ? "1" : "0");

    win->renderer = SDL_CreateRenderer(win->window, NULL);
    if (win->renderer == NULL)
    {
        SDL_DestroyWindow(win->window);
        free(win);
        return NULL;
    }

    SDL_SetRenderVSync(win->renderer, vsync ? 1 : 0);

    win->texture = SDL_CreateTexture(win->renderer, SDL_PIXELFORMAT_XRGB8888,
                                     SDL_TEXTUREACCESS_STREAMING, width, height);
    if (win->texture == NULL)
    {
        SDL_DestroyRenderer(win->renderer);
        SDL_DestroyWindow(win->window);
        free(win);
        return NULL;
    }

    win->texture_width = width;
    win->texture_height = height;

    g_window = win;

    return win;
}

void platform_window_destroy(PlatformWindow* window)
{
    if (window == NULL)
    {
        return;
    }

    g_window = NULL;
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

void platform_window_present(PlatformWindow* window, const u32* pixels, i32 width, i32 height)
{
    if (window == NULL || pixels == NULL)
    {
        return;
    }

    if (width != window->texture_width || height != window->texture_height)
    {
        SDL_DestroyTexture(window->texture);
        window->texture = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_XRGB8888,
                                            SDL_TEXTUREACCESS_STREAMING, width, height);
        if (window->texture == NULL)
        {
            return;
        }

        window->texture_width = width;
        window->texture_height = height;
    }

    SDL_UpdateTexture(window->texture, NULL, pixels, width * (int)sizeof(u32));
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
        if (sdl_event.key.scancode == SDL_SCANCODE_ESCAPE && g_window != NULL
            && (SDL_GetWindowFlags(g_window->window) & SDL_WINDOW_FULLSCREEN))
        {
            SDL_SetWindowFullscreen(g_window->window, false);
        }
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

static void platform_frame_sleep(f64 target_frame_time, f64 frame_start)
{
    const f64 SPIN_THRESHOLD = 0.002;
    for (;;)
    {
        const f64 remaining = target_frame_time - (platform_get_time_seconds() - frame_start);
        if (remaining <= 0.0)
        {
            break;
        }
        if (remaining > SPIN_THRESHOLD)
        {
            SDL_DelayNS((Uint64)((remaining - SPIN_THRESHOLD) * 1e9));
        }
    }
}

void platform_run_main_loop(PlatformFrameCallback frame_cb, void* user_data, i32 target_fps)
{
    // When no explicit cap is set, fall back to the display refresh rate so
    // the loop doesn't spin uncapped even if the backend VSync is unreliable.
    const i32 effective_fps = (target_fps > 0) ? target_fps : g_display_fps;
    const f64 target_frame_time = (effective_fps > 0) ? (1.0 / (f64)effective_fps) : 0.0;
    for (;;)
    {
        const f64 frame_start = platform_get_time_seconds();
        if (!frame_cb(user_data))
        {
            break;
        }
        if (target_frame_time > 0.0)
        {
            platform_frame_sleep(target_frame_time, frame_start);
        }
    }
}

f64 platform_get_time_seconds(void)
{
    return (f64)SDL_GetPerformanceCounter() / (f64)SDL_GetPerformanceFrequency();
}

u64 platform_get_perf_counter(void)
{
    return SDL_GetPerformanceCounter();
}

u64 platform_get_perf_frequency(void)
{
    return SDL_GetPerformanceFrequency();
}

#else // __EMSCRIPTEN__

#include <stdlib.h>
#include <string.h>

#include <emscripten.h>
#include <emscripten/html5.h>

// Implemented in platform_web.js, linked via --js-library.
extern i32 platform_js_get_window_width(void);
extern i32 platform_js_get_window_height(void);
extern void platform_js_set_title(const char* title);
extern void platform_js_init_canvas(const char* id);
extern void platform_js_request_fullscreen(void);
extern void platform_js_present(const u32* pixels, i32 width, i32 height);

#define PLATFORM_CANVAS_ID_MAX 64
static char g_canvas_selector[PLATFORM_CANVAS_ID_MAX + 2]; // '#' + id + '\0'

typedef struct
{
    PlatformFrameCallback cb;
    void* user_data;
} EmscriptenLoopState;

struct PlatformWindow
{
    i32 width;
    i32 height;
};

static EmscriptenLoopState g_loop_state;
static f64 g_frame_time_sec = 0.0;

static i32 g_canvas_width = 0;
static i32 g_canvas_height = 0;
static bool g_resize_pending = false;

static EM_BOOL on_window_resize(int event_type, const EmscriptenUiEvent* ui_event, void* user_data)
{
    (void)event_type;
    (void)user_data;

    g_canvas_width = (i32)ui_event->windowInnerWidth;
    g_canvas_height = (i32)ui_event->windowInnerHeight;
    g_resize_pending = true;

    return EM_TRUE;
}

bool platform_init(void)
{
    return true;
}

void platform_shutdown(void)
{
}

PlatformWindow* platform_window_create(const char* title, i32 width, i32 height, i32 display_index,
                                       i32 min_width, i32 min_height, bool resizable,
                                       bool fullscreen, bool vsync, const char* canvas_id)
{
    (void)width;
    (void)height;
    (void)display_index;
    (void)min_width;
    (void)min_height;
    (void)resizable;
    (void)vsync;

    g_canvas_selector[0] = '#';
    strncpy(g_canvas_selector + 1, canvas_id, PLATFORM_CANVAS_ID_MAX);
    g_canvas_selector[PLATFORM_CANVAS_ID_MAX + 1] = '\0';

    platform_js_init_canvas(canvas_id);
    platform_js_set_title(title);

    if (fullscreen)
    {
        platform_js_request_fullscreen();
    }

    PlatformWindow* win = malloc(sizeof(*win));
    if (win == NULL)
    {
        return NULL;
    }

    const i32 actual_width = platform_js_get_window_width();
    const i32 actual_height = platform_js_get_window_height();

    win->width = actual_width;
    win->height = actual_height;

    g_canvas_width = actual_width;
    g_canvas_height = actual_height;
    g_resize_pending = true;

    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, 0, on_window_resize);

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

void platform_window_present(PlatformWindow* window, const u32* pixels, i32 width, i32 height)
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

    if (g_resize_pending)
    {
        event->type = PLATFORM_EVENT_WINDOW_RESIZED;
        event->data.window.width = g_canvas_width;
        event->data.window.height = g_canvas_height;
        g_resize_pending = false;
        return true;
    }

    event->type = PLATFORM_EVENT_NONE;
    return false;
}

static void emscripten_frame_wrapper(void)
{
    // Snapshot time once at rAF entry so every platform_get_time_seconds()
    // call within a frame returns the same value — no intra-frame drift.
    g_frame_time_sec = emscripten_get_now() / 1000.0;
    if (!g_loop_state.cb(g_loop_state.user_data))
    {
        emscripten_cancel_main_loop();
    }
}

void platform_run_main_loop(PlatformFrameCallback frame_cb, void* user_data, i32 target_fps)
{
    (void)target_fps;
    g_loop_state.cb = frame_cb;
    g_loop_state.user_data = user_data;
    g_frame_time_sec = emscripten_get_now() / 1000.0;
    emscripten_set_main_loop(emscripten_frame_wrapper, 0, 1);
}

f64 platform_get_time_seconds(void)
{
    return g_frame_time_sec;
}

u64 platform_get_perf_counter(void)
{
    return (u64)(emscripten_get_now() * 1000.0);
}

u64 platform_get_perf_frequency(void)
{
    return 1000000ULL;
}

#endif // __EMSCRIPTEN__
