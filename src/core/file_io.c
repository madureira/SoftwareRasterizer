#include "core/file_io.h"

#include <stdio.h>

long file_io_size(const char* path)
{
    if (path == NULL)
    {
        return -1;
    }

    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size;
}

bool file_io_read(const char* path, u8* buffer)
{
    if (path == NULL || buffer == NULL)
    {
        return false;
    }

    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    bool ok = size > 0 && (usize)fread(buffer, 1, (usize)size, file) == (usize)size;
    fclose(file);

    return ok;
}
