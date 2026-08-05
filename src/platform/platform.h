#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

typedef struct PlatformWindow PlatformWindow;

typedef enum PlatformEventType
{
    PLATFORM_EVENT_NONE,
    PLATFORM_EVENT_QUIT,
    PLATFORM_EVENT_WINDOW_RESIZED,
    PLATFORM_EVENT_KEY_DOWN,
    PLATFORM_EVENT_KEY_UP
} PlatformEventType;

typedef struct PlatformWindowEvent
{
    i32 width;
    i32 height;
} PlatformWindowEvent;

typedef struct PlatformEvent
{
    PlatformEventType type;

    union
    {
        PlatformWindowEvent window;
    } data;
} PlatformEvent;

bool platform_init(void);

void platform_shutdown(void);

PlatformWindow* platform_window_create(const char* title, i32 width, i32 height, bool resizable,
                                       bool fullscreen, bool vsync, const char* canvas_id);

void platform_window_destroy(PlatformWindow* window);

void platform_window_set_title(PlatformWindow* window, const char* title);

void platform_window_present(PlatformWindow* window, const u32* pixels, i32 width, i32 height);

bool platform_poll_event(PlatformEvent* event);

typedef bool (*PlatformFrameCallback)(void* user_data);

void platform_run_main_loop(PlatformFrameCallback frame_cb, void* user_data, i32 target_fps);

f64 platform_get_time_seconds(void);

#endif // PLATFORM_PLATFORM_H
