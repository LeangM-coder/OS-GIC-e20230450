#include <fcntl.h>    // open(), O_WRONLY, O_CREAT, O_TRUNC
#include <unistd.h>   // write(), close()
#include <string.h>   // strlen()

int main() {
    // 1. Open/create "output.txt"
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        return 1;
    }

    // 2. Write the message to the file
    char message[] = "Hello from Operating Systems class!\n";
    write(fd, message, strlen(message));

    // 3. Close the file
    close(fd);

    // 4. Print a success message to the terminal
    char success[] = "File created successfully!\n";
    write(STDOUT_FILENO, success, strlen(success));

    return 0;
}
