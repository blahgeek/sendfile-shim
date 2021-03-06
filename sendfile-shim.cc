#include <algorithm>
#include <cstddef>
#include <cstdio>

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

static constexpr size_t kMaxChunkSize = 4096;

extern "C"
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count) {
    if (offset) {
        off_t seek_ret = lseek(in_fd, *offset, SEEK_SET);
        if (seek_ret == (off_t)-1) {
            return -1;
        }
    }

    bool has_error = false;
    size_t bytes_processed = 0;
    char buf[kMaxChunkSize];
    while (bytes_processed < count) {
        size_t chunk_size = std::min(count - bytes_processed, kMaxChunkSize);
        ssize_t read_size = read(in_fd, buf, chunk_size);
        if (read_size == -1) {
            has_error = true;
            break;
        }
        bytes_processed += read_size;

        ssize_t written_size = 0;
        while (written_size < read_size) {
            ssize_t write_result = write(out_fd, buf + written_size, read_size - written_size);
            if (write_result <= 0) {
                has_error = true;
                break;
            }
            written_size += write_result;
        }
        if (has_error) {
            break;
        }

        if (read_size < chunk_size) {
            // no error, but stop here
            break;
        }
    }

    int old_errno = errno;
    if (offset) {
        // If offset is not NULL, then sendfile() does not modify the file offset of in_fd
        lseek(in_fd, -((off_t)bytes_processed), SEEK_CUR);
        *offset += bytes_processed;
    }
    errno = old_errno;

    return bytes_processed;
}

extern "C"
ssize_t sendfile64(int out_fd, int in_fd, off_t *offset, size_t count) {
    return sendfile(out_fd, in_fd, offset, count);
}
