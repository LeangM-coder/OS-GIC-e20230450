# Class Activity 3 — Socket Communication & Multithreading

- **Student Name:** [EM Mengleang]
- **Student ID:** [e20230450]
- **Date:** [16/11/2026]
----------------- | Task 1| ------------------

1. bind() gives the server socket a fixed address (IP + port 8080), so clients know where to find it. The client doesn't need this. The OS picks a random free port for it when it calls connect().

2. It returns a new socket (client_fd) for that one client.

	. server_fd only listens for new connections.
	. client_fd is used to read() and send() data with the client.

3. The client fails with:

connect: Connection refused

Nobody is listening on port 8080 yet, so the OS rejects it. That's why I used sleep 1 in the script.

4. It converts the port number to network byte order (big-endian). Computers store numbers in different byte orders, so without this the other side could read the wrong port. For example, 8080 could be read as a different number.

5.    SERVER                     CLIENT
   ------                     ------
   socket()                   socket()
   bind()
   listen()
   accept()  <--connection--  connect()
   read()    <--message-----  send()
   send()    --reply------->  read()
   close()                    close()

---------------------- | Task 2| ----------------------

1. A race condition is when two or more threads use the same data at the same time, and the result depends on who runs first.

shared_counter++ looks like one step, but it is really three:

	1. read the value
	2. add 1
	3. write it back

If two threads read the same value before either writes, they both write the same new number. One increment is lost. After many loops, the final counter is smaller than expected.

2. It lets only one thread into the locked part of the code at a time. Other threads have to wait until the first one calls pthread_mutex_unlock().

If the increment is inside the lock, the three steps (read, add, write) can't be mixed with another thread's. So no increments are lost and the result is correct.

3. main() doesn't wait for the threads. If main() finishes first, the whole program exits and the other threads are killed in the middle of their work. So the output can be missing or the counter can be much too small.

(I tested this by removing pthread_join() and running it. My result: write what you saw here.)

4. A process is a whole running program with its own memory. A thread is a smaller part inside a process. Threads are lighter and faster to create, and they talk to each other easily because they share memory.

Shared by all threads in a process:

	.code
	.global variables and the heap
	.open files

Private to each thread:

	.its own stack (local variables)
	.its own registers and program counter
	.its own thread ID

---------------------- | Task 3| -----------------

1. Extending Thread vs implementing Runnable
	. Extending Thread: my class is a thread. It's simple, but Java only allows one parent class, so I can't extend anything else.
	. Implementing Runnable: my class only holds the task (the run() code). Then I give it to a Thread or an executor to run.

2. The pool has a fixed size of 2 threads, so only 2 tasks can run at once. The other 4 tasks wait in a queue. When a thread finishes a task, it picks the next one from the queue. So all 6 tasks do run, just 2 at a time.

3. join() makes the calling thread (usually main) wait until that thread finishes.
Without it, main keeps going and doesn't wait. Any code after the thread starts (like a "done" message or a final result) can run too early, before the thread has finished. The output order can look mixed up or wrong. In Java the program usually still waits for normal threads to finish before exiting, unlike C, but the code in main no longer waits.

4, ExecutorService better than creating threads by hand because:
	. Reuses threads. Making a new thread for every task is slow and uses a lot of memory. A pool reuses the same ones.
	. Limits the number of threads. With thousands of tasks, hand-made threads can crash or slow down the whole program. A pool keeps the number under control.
	. Easier to manage. It has a task queue, shutdown(), and Future to get results back, so I don't have to write all of that myself.
 
## Task 4: Observing Threads

### Answers

1. **LWP column meaning:**

LWP means "Light Weight Process". On Linux, every thread is basically a
small process that shares memory with the others, and the LWP number is
that thread's own ID. The PID is the same for every thread in my program,
but each thread has a different LWP. The first thread (the main one) has
LWP equal to the PID. The `NLWP` column shows how many threads the process
has in total.

2. **/proc/PID/task/ count:**

I found **[N]** entries. At first I expected 4 (Alpha, Beta, Gamma, and
main), but it was higher. Each folder in `task/` is one thread, and the
JVM creates a bunch of its own threads (see question 3), so the number
doesn't match my 4 exactly.

3. **Extra Java threads:**

Because the 3 threads I made are only part of the picture. The extra ones are:
	. the **main** thread, which runs my `main()` method
	. **garbage collector** threads, which clean up memory
	. **JIT compiler** threads (C1 and C2), which speed up the code while it runs
	. JVM helper threads like Reference Handler, Finalizer, Signal Dispatcher and Common-Cleaner.
So the total is my 3 + main + the JVM's own threads. I could see these
in `jstack` too.

4. **Linux vs Windows thread viewing:**

Linux shows more. With `ps -eLf` I can see every thread on its own line
with its LWP, and `/proc/<PID>/task/` has a folder for each thread with
its status and more info. Task Manager on Windows only shows one number
for how many threads a process has (in the Details tab, after adding the
Threads column). It doesn't list the threads or their names. To see
individual threads on Windows I had to use `jps` and `jstack`, which are
Java tools, not Windows ones. It also took me a while to figure out
that I needed two windows to catch the program while it was running.

## Reflection

**What I found most interesting**

The thing that surprised me most was how many threads a "simple" program really has. I only created 3 threads (Alpha, Beta, Gamma), but the JVM had a lot more running in the background, like garbage collection and the JIT compiler. I never wrote any code for those, but they were there. Before this lab I thought a thread was just something I create with `new Thread()`.

I also found it interesting that on Linux every thread shows up as its own entry (with its own LWP), but on Windows Task Manager only gives me a number. I had to use `jps` and `jstack` to actually see the threads. Honestly, getting `jstack` to work was the hardest part for me. I kept running it on the PID of `jps` itself, and it took me a few tries to understand that the program has to be running at the same time in another window.

Seeing the output from Alpha, Beta, and Gamma mixed together also made concurrency feel real. The order changes depending on timing, so I can't assume which thread prints first.

**How understanding threads at the OS level helps me**

Now I understand that a Java thread is not just an object, it's something the operating system schedules. That means:

- Threads share memory, so if two of them change the same variable I need to be careful (synchronization). I haven't practiced this much yet, but I know why it matters now.
- Creating too many threads costs memory and CPU time, because the OS has to switch between them. More threads doesn't always mean faster.
- A thread stuck in `sleep()` or `join()` is not using the CPU, and `jstack` showed me that with `TIMED_WAITING` and `WAITING`. This helps me debug a program that looks frozen.
- Because the OS decides when each thread runs, I should never depend on the order of threads in my program. If order matters, I have to use something like `join()`.

I still don't feel fully confident with concurrency, but now when I see a thread problem I know where to look (`jstack`, `ps`, `/proc`) instead of just guessing.












### Linux — `ps -eLf` Output

PS C:\Users\WindowsML\javatest> java ThreadDemo
Main thread (ID: 1): starting...
PID: 9624 ? observe threads with ps or htop

[Beta] Count: 1 (Thread ID: 21)
[Alpha] Count: 1 (Thread ID: 20)
[Gamma] Count: 1 (Thread ID: 22)
[Beta] Count: 2 (Thread ID: 21)
[Alpha] Count: 2 (Thread ID: 20)
[Gamma] Count: 2 (Thread ID: 22)
[Gamma] Count: 3 (Thread ID: 22)
[Alpha] Count: 3 (Thread ID: 20)
[Beta] Count: 3 (Thread ID: 21)
[Gamma] Count: 4 (Thread ID: 22)
[Alpha] Count: 4 (Thread ID: 20)
[Beta] Count: 4 (Thread ID: 21)
[Alpha] Count: 5 (Thread ID: 20)
[Gamma] Count: 5 (Thread ID: 22)
[Beta] Count: 5 (Thread ID: 21)
[Alpha] Finished!
[Gamma] Finished!
[Beta] Finished!

Main thread: all threads finished.


### Linux — Kernel Threads (commands)


└─$ ps -eLf | grep threads_observe                           
kali       14319    7407   14319  0    1 10:57 pts/2    00:00:00 grep --color=auto threads_observe


$ ps -T -p 7407                            
    PID    SPID TTY          TIME CMD
   7407    7407 pts/2    00:00:00 zsh




