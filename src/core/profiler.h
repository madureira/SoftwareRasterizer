#ifndef CORE_PROFILER_H
#define CORE_PROFILER_H

#ifndef ENABLE_PROFILING
#define ENABLE_PROFILING 0
#endif

#define PROFILER_INVALID_EVENT ((usize) - 1)

typedef struct ProfilerToken
{
    usize event_index;
} ProfilerToken;

void profiler_init(void);

void profiler_reset(void);

ProfilerToken profiler_begin(const char* category, const char* name);

void profiler_end(ProfilerToken token);

bool profiler_write_json(const char* file_path);

usize profiler_get_event_count(void);

usize profiler_get_dropped_event_count(void);

#if ENABLE_PROFILING

#define PROFILE_INIT() profiler_init()

#define PROFILE_RESET() profiler_reset()

#define PROFILE_BEGIN(category, name) profiler_begin((category), (name))

#define PROFILE_END(token) profiler_end((token))

#define PROFILE_WRITE(file_path) profiler_write_json((file_path))

#else // ENABLE_PROFILING

#define PROFILE_INIT() ((void)0)

#define PROFILE_RESET() ((void)0)

#define PROFILE_BEGIN(category, name) ((ProfilerToken){ PROFILER_INVALID_EVENT })

#define PROFILE_END(token) ((void)(token))

#define PROFILE_WRITE(file_path) ((bool)true)

#endif // ENABLE_PROFILING

#endif // CORE_PROFILER_H
