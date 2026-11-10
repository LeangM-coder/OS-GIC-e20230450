-------------- | Task 1| -----------

1. fork() makes a copy of the process. The parent gets the child's PID back (for me it was 4022). The child gets 0. If it fails it gives -1 and no child is made.

This is how the code knows who is who. If pid == 0 then it's the child, if pid > 0 then it's the parent.

2. The parent doesn't wait anymore so the output gets messy.

If the parent finishes first, it prints "done" and exits, and the shell prompt comes back while the child is still sleeping. Then the ls -la output shows up later after the prompt, which looks weird. The child also becomes an orphan and init/systemd adopts it, so its PPID turns into 1.

If the child finishes first but the parent is still running, the child becomes a zombie (Z in ps) because nobody collected its exit status.

So the order of output isn't guaranteed. With waitpid() the parent always prints its last lines after the child is totally done.

3. It replaces the child's program with ls -la. The PID stays the same but the old code is gone and ls runs instead. The "p" in execlp means it looks for ls in PATH so I don't need to write /bin/ls.

We don't see "execlp failed" because execlp only returns if something went wrong. When it works, the old program doesn't exist anymore, so the perror line never runs. Anything after execlp is only for the error case.

4. bash
 └── fork_demo  (PID 4021, parent)
      └── fork_demo  (PID 4022, child, PPID 4021)
During the 30 second sleep the child shows as fork_demo. After execlp it becomes ls -la but the PID is still 4022.

5. used command => ps -ef --forest | grep fork_demo
The columns are:

UID: who owns the process
PID: process ID
PPID: parent's PID (for the child this is 4021)
C: CPU usage, basically 0 because it's sleeping
STIME: when the process started
TTY: the terminal it's running in, like pts/0
TIME: total CPU time used (not how long it's been alive, I mixed this up at first)
CMD: the command, and --forest draws the tree lines here.

----------------| Task 2|-------------------

1. 

---------------| Task 3 |----------------

1. shm_open() creates or opens a shared memory object and gives back a file descriptor for it. The object lives in memory (on Linux it shows up under /dev/shm), not on the hard disk. It's identified by a name like OS-emmengleang instead of a normal file path. open() works on regular files that are stored on disk, so the data is saved there and normal file paths are used. The descriptor from shm_open() still has to be sized with ftruncate() and mapped with mmap() before it can be used.

2. mmap() maps the shared memory object into the process's own address space, so the process can use it through a pointer like normal memory. In my producer I used sprintf() on the pointer to write the message, and the consumer read it back with the same kind of pointer. Shared memory is faster because after the mapping is set up, the processes read and write the memory directly. With pipes or message queues, every message has to be copied from the sender into the kernel and then from the kernel to the receiver, and each of those steps needs a system call. Shared memory skips that.

3. The name is the only way the two programs find the same object. They are separate processes and don't share variables or pointers, so the name is how the consumer tells the system which shared memory to open. If the names are different, shm_open() in the consumer fails (because it uses O_RDONLY without O_CREAT) or points to a different object, so it would never see the producer's message. In my code both use OS-emmengleang.

4. shm_unlink() removes the shared memory object's name from the system. Once nothing is using it anymore, the memory is freed. If the consumer didn't call it, the object would stay in /dev/shm after both programs finished, because shared memory doesn't go away when the process exits. I saw this in my test, where ls -l /dev/shm/OS-emmengleang still showed the 4096 byte file after the producer finished. It would stay there until someone removes it with rm or the system reboots, and it would waste memory.

5. I ran the consumer after it had already unlinked the object, so there was nothing to open, which is the same situation as running it before the producer. The output was:

shm_open: No such file or directory
Hint: Did you run shm-producer first?

shm_open() returned -1 because the object doesn't exist yet. The consumer doesn't use O_CREAT, so it can't create one. Only the producer creates it. The program then printed the error and the hint and exited.

----------------------| Task 4 |--------------------

1. Message queue vs shared memory
	. Message queue: like a mailbox. One process sends a message, another receives it. The kernel handles the safety for us.
	. Shared memory: both processes use the same piece of memory. It is faster, but we must add our own locking.
	. Use a message queue for small messages. Use shared memory for big data or when speed matters.

2. POSIX requires it. The / tells the system it is a queue name. Without it, mq_open() fails.

3. It deletes the queue's name, so the queue is removed once everyone closes it. If we never call it, the queue stays in the system until reboot, and old messages may still be inside.

4. . If the receiver uses O_CREAT, it creates the queue and waits for a message.
   . If not, mq_open() fails because the queue does not exist yet.

5. . Senders: yes, many can send to the same queue
   . Receivers: yes, but each message goes to only one receiver (not everyone).
