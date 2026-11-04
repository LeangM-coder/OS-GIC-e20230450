--------------- | Task1 | -----------------
1. read() returns the number of bytes it read.

More than 0 = bytes read
0 = end of file
-1 = error

fgets() reads one line (until newline) and returns a pointer to the buffer, or NULL at the end. It also adds '\0' at the end. read() does not add '\0', it only gives raw bytes.

2.  One read() may not read everything. The file can be bigger than the buffer, or read() can return fewer bytes than I asked for. So I call it again and again in a loop.

It stops when read() returns 0 (end of file). It also stops if it returns -1 (error). 

--------------- |Task 2| -------------------
1. readdir() returns a pointer to struct dirent (or NULL at the end of the directory). Main fields:

d_ino – inode number of the file
d_name – the file name (a string)
d_type – type of entry (file, directory, link...)
d_off / d_reclen – position and length of the entry (internal use)

Most of the time I only use d_name.

2. stat() fills a struct stat. Besides size (st_size) it gives:

st_mode – file type and permissions
st_ino – inode number
st_nlink – number of hard links
st_uid / st_gid – owner and group
st_atime / st_mtime / st_ctime – last access, last modify, last status change time
st_blocks / st_blksize – disk blocks used and block size

3. write() sends raw bytes, not numbers. If I give it an int, it writes the binary bytes of that number, not the digits I want to see on screen. For example, 65 would print as the letter A, not 65.

snprintf() converts the number into a text string (like "65") in a buffer. Then I use write() with that buffer and its length.

------------------- | Task 3| ----------------------

1. When I ran strace -c, the library version made X system calls and the system call version made Y. So the library version made more. I was a bit surprised, because the program does the same job in both versions.

2. In the library version I saw some calls that I never wrote in my code:

brk – makes the heap bigger, I think this is for the buffer that the library makes with malloc
mmap – maps memory, used when loading the C library
access – checks if a file exists, the loader does this at the start
fstat – gets info about a file, so the library can decide how to buffer it.
these calls load the C library and prepare memory and buffers. The system call version does not need them because it talks to the kernel directly and does not use the library.

3. strace showed N write() call(s). At first I expected one write() for each fprintf(), but that is not what happened. The reason is buffering. fprintf() keeps the text in a buffer in memory and only calls write() when the buffer is full, when the program ends, or when I use fflush(). So there are fewer write() calls than fprintf() calls 

4. A system call is a request to the kernel, like read() or write(). Only the kernel can do that work.

A library function, like fprintf(), is just normal code in the C library. It does some work by itself (like formatting text and buffering), and when it needs the OS, it calls a system call inside.

In strace I could see it: my one fprintf() was not a system call, but it led to a write() system call underneath, plus some extra setup calls. So library functions are built on top of system calls and make them easier to use.


