#include "app/app.h"

#include <stdlib.h>
#include <string.h>

#include "app/window.h"
#include "math/vec2f.h"
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

static inline float32 edge(Vec2f a, Vec2f b, Vec2f p)
{
    return vec2f_cross(vec2f_sub(b, a), vec2f_sub(p, a));
}

static void draw_triangle(uint32* pixels, int width, int height, Vec2f v0, Vec2f v1, Vec2f v2,
                          uint32 color)
{
    int x_min = (int)fminf(v0.x, fminf(v1.x, v2.x));
    int y_min = (int)fminf(v0.y, fminf(v1.y, v2.y));
    int x_max = (int)fmaxf(v0.x, fmaxf(v1.x, v2.x));
    int y_max = (int)fmaxf(v0.y, fmaxf(v1.y, v2.y));

    x_min = x_min < 0 ? 0 : x_min;
    y_min = y_min < 0 ? 0 : y_min;
    x_max = x_max >= width ? width - 1 : x_max;
    y_max = y_max >= height ? height - 1 : y_max;

    const Vec2f p0 = vec2f((float32)x_min + 0.5f, (float32)y_min + 0.5f);

    const float32 w0_init = edge(v1, v2, p0);
    const float32 w1_init = edge(v2, v0, p0);
    const float32 w2_init = edge(v0, v1, p0);

    const float32 w0_step_x = -(v2.y - v1.y), w0_step_y = v2.x - v1.x;
    const float32 w1_step_x = -(v0.y - v2.y), w1_step_y = v0.x - v2.x;
    const float32 w2_step_x = -(v1.y - v0.y), w2_step_y = v1.x - v0.x;

    float32 row_w0 = w0_init;
    float32 row_w1 = w1_init;
    float32 row_w2 = w2_init;

    for (int y = y_min; y <= y_max; y++)
    {
        float32 w0 = row_w0;
        float32 w1 = row_w1;
        float32 w2 = row_w2;

        bool span_started = false;

        for (int x = x_min; x <= x_max; x++)
        {
            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
            {
                pixels[y * width + x] = color;
                span_started = true;
            }
            else if (span_started)
            {
                break;
            }

            w0 += w0_step_x;
            w1 += w1_step_x;
            w2 += w2_step_x;
        }

        row_w0 += w0_step_y;
        row_w1 += w1_step_y;
        row_w2 += w2_step_y;
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

    Vec2f center = vec2f((float32)w * 0.5f, (float32)h * 0.5f);

    float32 a0 = angle;
    float32 a1 = angle + (2.0f * (float32)MATH_PI / 3.0f);
    float32 a2 = angle + (4.0f * (float32)MATH_PI / 3.0f);

    Vec2f base = vec2f(radius, 0.0f);

    Vec2f v0 = vec2f_add(center, vec2f_rotate(base, a0));
    Vec2f v1 = vec2f_add(center, vec2f_rotate(base, a1));
    Vec2f v2 = vec2f_add(center, vec2f_rotate(base, a2));

    const uint32 dark_grey = 0x22222222;
    const uint32 blue = 0x0000AAFF;

    // Clear framebuffer using byte-fill.
    memset(state->pixels, dark_grey & 0xFF, (size_t)w * h * sizeof(uint32));

    draw_triangle(state->pixels, w, h, v0, v1, v2, blue);

    window_present(state->window, state->pixels, w, h);

    return !window_should_close(state->window);
}

int app_run(void)
{
    if (!platform_init())
    {
        return 1;
    }

    Window* window = window_create(&(WindowConfig){
        .title = WINDOW_TITLE, .width = WINDOW_WIDTH, .height = WINDOW_HEIGHT, .resizable = true});

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

    // clang-format off
    AppState state = {
        .window = window,
        .pixels = pixels,
        .width = WINDOW_WIDTH,
        .height = WINDOW_HEIGHT
    };
    // clang-format on

    platform_run_main_loop(frame, &state);

    free(state.pixels);
    window_destroy(window);
    platform_shutdown();

    return 0;
}
