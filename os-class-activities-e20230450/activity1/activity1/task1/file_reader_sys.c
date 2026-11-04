#include <fcntl.h>
#include <unistd.h>

int main() {
    char buffer[256];

    // 1. Open "output.txt" for reading
    int fd = open("output.txt", O_RDONLY);
    if (fd == -1) {
        return 1;
    }

    // 2. Read and 3. Write content to the terminal in a loop
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        if (write(1, buffer, bytes_read) == -1) {
            close(fd);
            return 1;
        }
    }

    // 4. Close the file
    close(fd);

    return bytes_read == -1 ? 1 : 0;
}
