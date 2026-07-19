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
            Vec2 p = vec2_create((float32)x + 0.5f, (float32)y + 0.5f);

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
    window_present(state->window, state->pixels, WINDOW_WIDTH, WINDOW_HEIGHT);
    return !window_should_close(state->window);
}

int app_run(void)
{
    if (!platform_initialize())
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

    uint32* pixels = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32));
    if (pixels == NULL)
    {
        window_destroy(window);
        platform_shutdown();
        return 1;
    }

    // Background: dark gray
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        pixels[i] = 0x00222222;
    }

    Vec2 v0 = vec2_create(400.0f, 100.0f);
    Vec2 v1 = vec2_create(650.0f, 500.0f);
    Vec2 v2 = vec2_create(150.0f, 500.0f);

    draw_triangle(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, v0, v1, v2, 0x0000AAFF);

    AppState state = {.window = window, .pixels = pixels};

    platform_run_main_loop(frame, &state);

    // Reached on desktop only; Emscripten never returns from the loop.
    free(pixels);
    window_destroy(window);
    platform_shutdown();

    return 0;
}
