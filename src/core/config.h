#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

#define CONFIG_WINDOW_TITLE_CAPACITY 128
#define CONFIG_CANVAS_ID_CAPACITY    64

typedef struct Config
{
    char window_title[CONFIG_WINDOW_TITLE_CAPACITY];
    i32 window_width;
    i32 window_height;
    i32 window_display_index;
    i32 window_min_width;
    i32 window_min_height;
    i32 window_max_width;
    i32 window_max_height;
    i32 target_fps;
    bool fullscreen;
    bool resizable;
    bool vsync;
    bool show_fps;
    char canvas_id[CONFIG_CANVAS_ID_CAPACITY];
} Config;

void config_set_default(Config* config);

bool config_load(Config* config, const char* path);

#endif // CORE_CONFIG_H
