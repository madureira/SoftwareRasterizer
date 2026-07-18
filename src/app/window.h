#ifndef APP_WINDOW_H
#define APP_WINDOW_H

typedef struct Window Window;

typedef struct WindowConfig
{
    const char *title;
    int32 width;
    int32 height;
    bool resizable;
} WindowConfig;

Window *window_create(const WindowConfig *config);

void window_destroy(Window *window);

void window_poll_events(Window *window);

bool window_should_close(const Window *window);

int32 window_get_width(const Window *window);

int32 window_get_height(const Window *window);

void window_set_title(Window *window, const char *title);

void window_present(Window *window, const uint32 *pixels, int32 width, int32 height);

#endif
