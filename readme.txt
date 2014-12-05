So the build instructions are as follows.
$ gcc -o bb bb.c -std=gnu99 -lpthread

This was tested on a linux OS.

This creates a simulated queue and produces and consumes simultaneously from them both using threads.

.bb [sleeptime] [prodthread] [consthread]