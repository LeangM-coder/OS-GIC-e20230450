
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>    // snprintf()

int main(void) {
    char buffer[512];

    // 1. Open current directory
    DIR *dir = opendir(".");
    if (dir == NULL) {
        return 1;
    }

    // 2. Print header using write()
    const char *header = "Name\t\tSize (bytes)\n";
    write(1, header, strlen(header));

    // 3. Loop through directory entries
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        struct stat file_info;

        // 4. Get file information
        if (stat(entry->d_name, &file_info) == -1) {
            continue;
        }

        // 5. Format and print the entry
        int length = snprintf(
            buffer,
            sizeof(buffer),
            "%s\t\t%lld\n",
            entry->d_name,
            (long long)file_info.st_size
        );

        if (length > 0 && length < (int)sizeof(buffer)) {
            write(1, buffer, (size_t)length);
        }
    }

    // 6. Close directory
    closedir(dir);

    return 0;
}
