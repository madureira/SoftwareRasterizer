#include "app/app.h"

#include <stdlib.h>

#include "app/window.h"
#include "math/vec2.h"
#include "platform/platform.h"

#define WINDOW_TITLE  "Software Rasterizer"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

typedef struct AppState
{
    Window* window;
    uint32* pixels;
    int32 width;
    int32 height;
} AppState;

static float32 edge(Vec2 a, Vec2 b, Vec2 p)
{
    return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
}

static void draw_triangle(uint32* pixels, int width, int height, Vec2 v0, Vec2 v1, Vec2 v2, uint32 color)
{
    int x_min = (int)fminf(v0.x, fminf(v1.x, v2.x));
    int y_min = (int)fminf(v0.y, fminf(v1.y, v2.y));
    int x_max = (int)fmaxf(v0.x, fmaxf(v1.x, v2.x));
    int y_max = (int)fmaxf(v0.y, fmaxf(v1.y, v2.y));

    x_min = x_min < 0 ? 0 : x_min;
    y_min = y_min < 0 ? 0 : y_min;
    x_max = x_max >= width ? width - 1 : x_max;
    y_max = y_max >= height ? height - 1 : y_max;

    for (int y = y_min; y <= y_max; y++)
    {
        for (int x = x_min; x <= x_max; x++)
        {
            Vec2 p = vec2((float32)x + 0.5f, (float32)y + 0.5f);

            float32 w0 = edge(v1, v2, p);
            float32 w1 = edge(v2, v0, p);
            float32 w2 = edge(v0, v1, p);

            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
            {
                pixels[y * width + x] = color;
            }
        }
    }
}

static bool frame(void* arg)
{
    AppState* state = (AppState*)arg;
    window_poll_events(state->window);

    int32 w = window_get_width(state->window);
    int32 h = window_get_height(state->window);

    if (w != state->width || h != state->height)
    {
        uint32* resized = realloc(state->pixels, (size_t)w * (size_t)h * sizeof(uint32));
        if (resized == NULL)
        {
            return false;
        }
        state->pixels = resized;
        state->width = w;
        state->height = h;
    }

    const float32 rotation_speed = 1.0f; // radians per second

    float32 angle = (float32)(platform_get_time_seconds() * rotation_speed);
    float32 radius = (float32)(w < h ? w : h) * 0.25f;

    Vec2 center = vec2((float32)w * 0.5f, (float32)h * 0.5f);

    float32 a0 = angle;
    float32 a1 = angle + (2.0f * (float32)MATH_PI / 3.0f);
    float32 a2 = angle + (4.0f * (float32)MATH_PI / 3.0f);

    Vec2 v0 = vec2(center.x + radius * cosf(a0), center.y + radius * sinf(a0));
    Vec2 v1 = vec2(center.x + radius * cosf(a1), center.y + radius * sinf(a1));
    Vec2 v2 = vec2(center.x + radius * cosf(a2), center.y + radius * sinf(a2));

    for (int i = 0; i < w * h; i++)
    {
        state->pixels[i] = 0x00222222;
    }

    draw_triangle(state->pixels, w, h, v0, v1, v2, 0x0000AAFF);

    window_present(state->window, state->pixels, w, h);

    return !window_should_close(state->window);
}

int app_run(void)
{
    if (!platform_init())
    {
        return 1;
    }

    Window* window = window_create(
        &(WindowConfig){.title = WINDOW_TITLE, .width = WINDOW_WIDTH, .height = WINDOW_HEIGHT, .resizable = true});

    if (window == NULL)
    {
        platform_shutdown();
        return 1;
    }

    uint32* pixels = malloc((size_t)WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32));
    if (pixels == NULL)
    {
        window_destroy(window);
        platform_shutdown();
        return 1;
    }

    AppState state = {.window = window, .pixels = pixels, .width = WINDOW_WIDTH, .height = WINDOW_HEIGHT};

    platform_run_main_loop(frame, &state);

    free(state.pixels);
    window_destroy(window);
    platform_shutdown();

    return 0;
}
