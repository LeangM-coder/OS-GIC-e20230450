#include <windows.h>
#include <string.h>

int main(void) {
    const char *message = "Hello from Operating Systems class!\n";
    DWORD bytes_written;

    // 1. Create or open output.txt for writing
    HANDLE file = CreateFileA(
        "output.txt",
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file == INVALID_HANDLE_VALUE) {
        return 1;
    }

    // 2. Write the message to the file
    BOOL success = WriteFile(
        file,
        message,
        (DWORD)strlen(message),
        &bytes_written,
        NULL
    );

    // 3. Close the file
    CloseHandle(file);

    return success ? 0 : 1;
}
