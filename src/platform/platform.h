#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

typedef struct PlatformWindow PlatformWindow;

typedef enum PlatformKey
{
    PLATFORM_KEY_UNKNOWN = 0,
    PLATFORM_KEY_ESCAPE,
    PLATFORM_KEY_ENTER,
    PLATFORM_KEY_UP,
    PLATFORM_KEY_DOWN,
    PLATFORM_KEY_LEFT,
    PLATFORM_KEY_RIGHT,
    PLATFORM_KEY_COUNT
} PlatformKey;

typedef struct PlatformKeyEvent
{
    PlatformKey key;
} PlatformKeyEvent;

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
        PlatformKeyEvent key;
    } data;
} PlatformEvent;

bool platform_init(void);

void platform_shutdown(void);

PlatformWindow* platform_window_create(const char* title, i32 width, i32 height, i32 display_index,
                                       i32 min_width, i32 min_height, bool resizable,
                                       bool fullscreen, bool vsync, const char* canvas_id);

void platform_window_destroy(PlatformWindow* window);

void platform_window_set_title(PlatformWindow* window, const char* title);

void platform_window_present(PlatformWindow* window, const u32* pixels, i32 width, i32 height);

bool platform_poll_event(PlatformEvent* event);

typedef bool (*PlatformFrameCallback)(void* user_data);

void platform_run_main_loop(PlatformFrameCallback frame_cb, void* user_data, i32 target_fps);

f64 platform_get_time_seconds(void);

u64 platform_get_perf_counter(void);

u64 platform_get_perf_frequency(void);

#endif // PLATFORM_PLATFORM_H
