#include "core/window.h"

#include <stdlib.h>
#include <string.h>

#include "platform/platform.h"

struct Window
{
    PlatformWindow* platform_window;
    i32 width;
    i32 height;
    bool should_close;
    bool keys[PLATFORM_KEY_COUNT];
};

Window* window_create(const WindowConfig* config)
{
    Window* window;

    if (config == NULL)
    {
        return NULL;
    }

    window = malloc(sizeof(*window));

    if (window == NULL)
    {
        return NULL;
    }

    window->platform_window =
        platform_window_create(config->title, config->width, config->height, config->display_index,
                               config->min_width, config->min_height, config->resizable,
                               config->fullscreen, config->vsync, config->canvas_id);

    if (window->platform_window == NULL)
    {
        free(window);
        return NULL;
    }

    window->width = config->width;
    window->height = config->height;
    window->should_close = false;
    memset(window->keys, 0, sizeof(window->keys));

    return window;
}

void window_destroy(Window* window)
{
    if (window == NULL)
    {
        return;
    }

    platform_window_destroy(window->platform_window);
    free(window);
}

void window_poll_events(Window* window)
{
    if (window == NULL)
    {
        return;
    }

    PlatformEvent event;
    while (platform_poll_event(&event))
    {
        switch (event.type)
        {
        case PLATFORM_EVENT_QUIT:
            window->should_close = true;
            break;

        case PLATFORM_EVENT_WINDOW_RESIZED:
            window->width = event.data.window.width;
            window->height = event.data.window.height;
            break;

        case PLATFORM_EVENT_KEY_DOWN:
            if (event.data.key.key != PLATFORM_KEY_UNKNOWN)
            {
                window->keys[event.data.key.key] = true;
            }
            break;

        case PLATFORM_EVENT_KEY_UP:
            if (event.data.key.key != PLATFORM_KEY_UNKNOWN)
            {
                window->keys[event.data.key.key] = false;
            }
            break;

        default:
            break;
        }
    }
}

bool window_should_close(const Window* window)
{
    return window != NULL && window->should_close;
}

i32 window_get_width(const Window* window)
{
    return window != NULL ? window->width : 0;
}

i32 window_get_height(const Window* window)
{
    return window != NULL ? window->height : 0;
}

void window_set_title(Window* window, const char* title)
{
    if (window == NULL)
    {
        return;
    }

    platform_window_set_title(window->platform_window, title);
}

void window_present(Window* window, const u32* pixels, i32 width, i32 height)
{
    if (window == NULL || pixels == NULL)
    {
        return;
    }

    platform_window_present(window->platform_window, pixels, width, height);
}

bool window_is_key_down(const Window* window, PlatformKey key)
{
    if (window == NULL || key == PLATFORM_KEY_UNKNOWN || key >= PLATFORM_KEY_COUNT)
    {
        return false;
    }
    return window->keys[key];
}
