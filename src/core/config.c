#include "core/config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/utils.h"

#define CONFIG_LINE_CAPACITY     256
#define CONFIG_WINDOW_WIDTH      960
#define CONFIG_WINDOW_HEIGHT     540
#define CONFIG_MIN_WINDOW_WIDTH  640
#define CONFIG_MIN_WINDOW_HEIGHT 360
#define CONFIG_MAX_WINDOW_WIDTH  3840
#define CONFIG_MAX_WINDOW_HEIGHT 2160
#define CONFIG_MIN_TARGET_FPS    0
#define CONFIG_MAX_TARGET_FPS    1000

static bool config_parse_bool(const char* text, bool* value)
{
    if (str_equals(text, "true") || str_equals(text, "1"))
    {
        *value = true;
        return true;
    }

    if (str_equals(text, "false") || str_equals(text, "0"))
    {
        *value = false;
        return true;
    }

    return false;
}

static bool config_parse_i32(const char* text, i32* value)
{
    char* end;
    long parsed;

    errno = 0;
    parsed = strtol(text, &end, 10);

    if (end == text || *end != '\0' || errno == ERANGE || parsed < INT32_MIN || parsed > INT32_MAX)
    {
        return false;
    }

    *value = (i32)parsed;
    return true;
}

static void config_warning(const char* path, usize line_number, const char* key, const char* value)
{
    fprintf(stderr, "%s:%lu: invalid configuration: %s=%s\n", path, (unsigned long)line_number, key,
            value);
}

static bool config_read_i32(const char* path, usize line_number, const char* key, const char* value,
                            i32* out, i32 min, i32 max)
{
    i32 parsed;
    if (!config_parse_i32(value, &parsed) || parsed < min || parsed > max)
    {
        config_warning(path, line_number, key, value);
        return false;
    }
    *out = parsed;
    return true;
}

static void config_read_bool(const char* path, usize line_number, const char* key,
                             const char* value, bool* out)
{
    if (!config_parse_bool(value, out))
    {
        config_warning(path, line_number, key, value);
    }
}

void config_set_defaults(Config* config)
{
    if (config == NULL)
    {
        return;
    }

    strcpy(config->window_title, "Software Rasterizer");
    config->window_width = CONFIG_WINDOW_WIDTH;
    config->window_height = CONFIG_WINDOW_HEIGHT;
    config->window_display_index = 0;
    config->window_min_width = CONFIG_MIN_WINDOW_WIDTH;
    config->window_min_height = CONFIG_MIN_WINDOW_HEIGHT;
    config->window_max_width = 1920;
    config->window_max_height = 1080;
    config->target_fps = 60;
    config->fullscreen = false;
    config->resizable = true;
    config->vsync = true;
    config->show_fps = false;
    strcpy(config->canvas_id, "canvas");
}

bool config_load(Config* config, const char* path)
{
    FILE* file;
    char line[CONFIG_LINE_CAPACITY];
    usize line_number = 0;

    if (config == NULL || path == NULL)
    {
        return false;
    }

    config_set_defaults(config);

    file = fopen(path, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Could not open configuration file: %s\n", path);

        return false;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char* text;
        char* separator;
        char* key;
        char* value;

        line_number++;

        if (strlen(line) == CONFIG_LINE_CAPACITY - 1 && line[CONFIG_LINE_CAPACITY - 2] != '\n')
        {
            fprintf(stderr, "%s:%lu: line too long, skipping\n", path, (unsigned long)line_number);
            int c;
            while ((c = fgetc(file)) != '\n' && c != EOF)
            {
            }
            continue;
        }

        text = str_trim(line);

        /*
         * Ignore empty lines and comments.
         */
        if (*text == '\0' || *text == '#' || *text == ';')
        {
            continue;
        }

        /*
         * Ignore INI sections.
         */
        if (*text == '[')
        {
            usize length = strlen(text);

            if (length < 2 || text[length - 1] != ']')
            {
                fprintf(stderr, "%s:%lu: invalid section declaration\n", path,
                        (unsigned long)line_number);
            }

            continue;
        }

        separator = strchr(text, '=');

        if (separator == NULL)
        {
            fprintf(stderr, "%s:%lu: expected key=value\n", path, (unsigned long)line_number);

            continue;
        }

        *separator = '\0';

        key = str_trim(text);
        value = str_trim(separator + 1);

        if (*key == '\0' || *value == '\0')
        {
            config_warning(path, line_number, key, value);
            continue;
        }

        if (str_equals(key, "title"))
        {
            usize length = strlen(value);

            if (length >= sizeof(config->window_title))
            {
                config_warning(path, line_number, key, value);
                continue;
            }

            memcpy(config->window_title, value, length + 1);
        }
        else if (str_equals(key, "width"))
        {
            config_read_i32(path, line_number, key, value, &config->window_width,
                            CONFIG_MIN_WINDOW_WIDTH, CONFIG_MAX_WINDOW_WIDTH);
        }
        else if (str_equals(key, "height"))
        {
            config_read_i32(path, line_number, key, value, &config->window_height,
                            CONFIG_MIN_WINDOW_HEIGHT, CONFIG_MAX_WINDOW_HEIGHT);
        }
        else if (str_equals(key, "display_index"))
        {
            config_read_i32(path, line_number, key, value, &config->window_display_index, 0, 16);
        }
        else if (str_equals(key, "min_width"))
        {
            config_read_i32(path, line_number, key, value, &config->window_min_width,
                            CONFIG_MIN_WINDOW_WIDTH, CONFIG_MAX_WINDOW_WIDTH);
        }
        else if (str_equals(key, "min_height"))
        {
            config_read_i32(path, line_number, key, value, &config->window_min_height,
                            CONFIG_MIN_WINDOW_HEIGHT, CONFIG_MAX_WINDOW_HEIGHT);
        }
        else if (str_equals(key, "max_width"))
        {
            config_read_i32(path, line_number, key, value, &config->window_max_width,
                            CONFIG_MIN_WINDOW_WIDTH, CONFIG_MAX_WINDOW_WIDTH);
        }
        else if (str_equals(key, "max_height"))
        {
            config_read_i32(path, line_number, key, value, &config->window_max_height,
                            CONFIG_MIN_WINDOW_HEIGHT, CONFIG_MAX_WINDOW_HEIGHT);
        }
        else if (str_equals(key, "target_fps"))
        {
            config_read_i32(path, line_number, key, value, &config->target_fps,
                            CONFIG_MIN_TARGET_FPS, CONFIG_MAX_TARGET_FPS);
        }
        else if (str_equals(key, "fullscreen"))
        {
            config_read_bool(path, line_number, key, value, &config->fullscreen);
        }
        else if (str_equals(key, "resizable"))
        {
            config_read_bool(path, line_number, key, value, &config->resizable);
        }
        else if (str_equals(key, "vsync"))
        {
            config_read_bool(path, line_number, key, value, &config->vsync);
        }
        else if (str_equals(key, "show_fps"))
        {
            config_read_bool(path, line_number, key, value, &config->show_fps);
        }
        else if (str_equals(key, "canvas_id"))
        {
            usize length = strlen(value);

            if (length == 0 || length >= sizeof(config->canvas_id))
            {
                config_warning(path, line_number, key, value);
                continue;
            }

            memcpy(config->canvas_id, value, length + 1);
        }
        else
        {
            fprintf(stderr, "%s:%lu: unknown configuration key: %s\n", path,
                    (unsigned long)line_number, key);
        }
    }

    if (ferror(file))
    {
        fprintf(stderr, "Error while reading configuration file: %s\n", path);

        fclose(file);
        return false;
    }

    fclose(file);

    if (config->window_min_width > config->window_max_width)
    {
        fprintf(stderr, "%s: min_width (%d) exceeds max_width (%d), resetting to max_width\n", path,
                config->window_min_width, config->window_max_width);
        config->window_min_width = config->window_max_width;
    }

    if (config->window_min_height > config->window_max_height)
    {
        fprintf(stderr, "%s: min_height (%d) exceeds max_height (%d), resetting to max_height\n",
                path, config->window_min_height, config->window_max_height);
        config->window_min_height = config->window_max_height;
    }

    if (config->window_width > config->window_max_width)
    {
        fprintf(stderr, "%s: width (%d) exceeds max_width (%d), resetting to max_width\n", path,
                config->window_width, config->window_max_width);
        config->window_width = config->window_max_width;
    }

    if (config->window_height > config->window_max_height)
    {
        fprintf(stderr, "%s: height (%d) exceeds max_height (%d), resetting to max_height\n", path,
                config->window_height, config->window_max_height);
        config->window_height = config->window_max_height;
    }

    return true;
}
