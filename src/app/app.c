#include "app/app.h"
#include "app/window.h"
#include "platform/platform.h"

#include <stdlib.h>

#define WINDOW_TITLE  "Software Rasterizer"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

typedef struct AppState
{
    Window* window;
    uint32* pixels;
} AppState;

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

    /* Solid red — XRGB8888: 0x00RRGGBB */
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        pixels[i] = 0x00FF0000;
    }

    AppState state = {.window = window, .pixels = pixels};

    platform_run_main_loop(frame, &state);

    /* Reached on desktop only; Emscripten never returns from the loop. */
    free(pixels);
    window_destroy(window);
    platform_shutdown();

    return 0;
}
