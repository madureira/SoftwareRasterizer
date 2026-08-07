#include "core/profiler.h"

#include <inttypes.h>
#include <stdio.h>

#include "platform/platform.h"

#ifndef PROFILER_MAX_EVENTS
#define PROFILER_MAX_EVENTS 131072
#endif

typedef struct ProfileEvent
{
    const char* category;
    const char* name;

    u64 timestamp_us;
    u64 duration_us;
} ProfileEvent;

static ProfileEvent g_events[PROFILER_MAX_EVENTS];

static u64 g_event_count;
static u64 g_dropped_event_count;

static u64 g_counter_origin;
static u64 g_counter_frequency;

static u64 profiler_now_us(void)
{
    u64 current_counter = platform_get_perf_counter();
    u64 elapsed_counter = current_counter - g_counter_origin;
    f64 elapsed_seconds = (f64)elapsed_counter / (f64)g_counter_frequency;

    return (u64)(elapsed_seconds * 1000000.0);
}

static void profiler_write_json_string(FILE* file, const char* text)
{
    const unsigned char* cursor;

    fputc('"', file);

    cursor = (const unsigned char*)text;

    while (*cursor != '\0')
    {
        switch (*cursor)
        {
        case '"':
            fputs("\\\"", file);
            break;

        case '\\':
            fputs("\\\\", file);
            break;

        case '\b':
            fputs("\\b", file);
            break;

        case '\f':
            fputs("\\f", file);
            break;

        case '\n':
            fputs("\\n", file);
            break;

        case '\r':
            fputs("\\r", file);
            break;

        case '\t':
            fputs("\\t", file);
            break;

        default:
        {
            if (*cursor < 0x20)
            {
                fprintf(file, "\\u%04x", (unsigned int)*cursor);
            }
            else
            {
                fputc((int)*cursor, file);
            }

            break;
        }
        }

        ++cursor;
    }

    fputc('"', file);
}

void profiler_init(void)
{
    profiler_reset();
}

void profiler_reset(void)
{
    g_event_count = 0;
    g_dropped_event_count = 0;

    g_counter_frequency = platform_get_perf_frequency();
    g_counter_origin = platform_get_perf_counter();
}

ProfilerToken profiler_begin(const char* category, const char* name)
{
    ProfilerToken token = { .event_index = PROFILER_INVALID_EVENT };

    if (g_event_count >= PROFILER_MAX_EVENTS)
    {
        ++g_dropped_event_count;
        return token;
    }

    token.event_index = g_event_count;
    ++g_event_count;

    ProfileEvent* event = &g_events[token.event_index];

    event->category = category;
    event->name = name;
    event->timestamp_us = profiler_now_us();
    event->duration_us = 0;

    return token;
}

void profiler_end(ProfilerToken token)
{
    if (token.event_index == PROFILER_INVALID_EVENT)
    {
        return;
    }

    if (token.event_index >= g_event_count)
    {
        return;
    }

    ProfileEvent* event = &g_events[token.event_index];
    u64 end_timestamp_us = profiler_now_us();

    if (end_timestamp_us >= event->timestamp_us)
    {
        event->duration_us = end_timestamp_us - event->timestamp_us;
    }
}

bool profiler_write_json(const char* file_path)
{
    FILE* file = fopen(file_path, "wb");

    if (file == NULL)
    {
        return false;
    }

    fputs("[\n", file);

    /*
     * Metadata: process name.
     */
    fputs("  {"
          "\"name\":\"process_name\","
          "\"ph\":\"M\","
          "\"pid\":1,"
          "\"tid\":0,"
          "\"args\":{\"name\":\"Software Rasterizer\"}"
          "}",
          file);

    /*
     * Metadata: main thread name.
     */
    fputs(",\n"
          "  {"
          "\"name\":\"thread_name\","
          "\"ph\":\"M\","
          "\"pid\":1,"
          "\"tid\":1,"
          "\"args\":{\"name\":\"Main Thread\"}"
          "}",
          file);

    for (usize index = 0; index < g_event_count; ++index)
    {
        const ProfileEvent* event = &g_events[index];

        fputs(",\n  {\"name\":", file);
        profiler_write_json_string(file, event->name);

        fputs(",\"cat\":", file);
        profiler_write_json_string(file, event->category);

        fprintf(file,
                ",\"ph\":\"X\""
                ",\"ts\":%" PRIu64 ",\"dur\":%" PRIu64 ",\"pid\":1"
                ",\"tid\":1"
                "}",
                event->timestamp_us, event->duration_us);
    }

    fputs("\n]\n", file);

    bool success = ferror(file) == 0;

    if (fclose(file) != 0)
    {
        success = false;
    }

    return success;
}

usize profiler_get_event_count(void)
{
    return g_event_count;
}

usize profiler_get_dropped_event_count(void)
{
    return g_dropped_event_count;
}
