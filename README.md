This is a reproduction on randomly freezing threads on libnx.

The thread pool implementation is taken from https://www.youtube.com/watch?v=eWTGtp3HXiw, but this also happens when using normal std::threads, pthreads and libnx threads. This is just the implementation I use in my app. (I tried the other ones in my app though, too, with the same random freezes.)

Instructions:

1. Load up hbmenu and press Y to get NetLoader
2. run `make clean all && while true; do nxlink -s *.nro*; done` (You might need to set `-a <switch IP>` on nxlink)
3. The app will run and dump stdio to your terminal
4. It will run and quit successfully if it did not freeze. So press Y on the switch to load the app again.
5. At some point it will freeze after spawning a new thread.
6. ??? This is where I am stuck. No idea how to debug this further.

Update:
 - Made the busy loop actually busy.
 - Loop threadpool creation and destruction to increase automation
 - Added a linux.elf target, that uses pthreads on linux. That runs without issues or freezes, so I am quite certain, that the freezes are either is related to HorizonOS, libnx or the switch hardware.
 - the switch froze after 154 and 206 loop iterations in my test runs. Twice it crashed completely (2168-0002). Running on FW 8.1.0. Tested with AMS 0.9.3 and SX OS 2.8.
 - compiled with `devkitA64 r13-4` on Arch Linux
