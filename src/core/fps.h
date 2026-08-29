#ifndef CORE_FPS_H
#define CORE_FPS_H

typedef struct FpsCounter FpsCounter;

FpsCounter* fps_create(bool enabled);

void fps_destroy(FpsCounter* fps);

void fps_frame_tick(FpsCounter* fps);

void fps_begin_render(FpsCounter* fps);

void fps_show(FpsCounter* fps, u32* pixels, i32 width, i32 height);

#endif // CORE_FPS_H
