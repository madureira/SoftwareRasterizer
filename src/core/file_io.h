#ifndef CORE_FILE_IO_H
#define CORE_FILE_IO_H

long file_io_size(const char* path);

bool file_io_read(const char* path, u8* buffer);

#endif // CORE_FILE_IO_H
