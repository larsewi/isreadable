# blocking-thread-timeout
Example on how you can timeout a blocking thread

### Did finnish in time example:
```
$ time ./prog 4
MAIN: Initializing mutex
MAIN: Initializing condition
MAIN: Locking mutex
MAIN: Initializing thread attributes
MAIN: Setting thread detach state
MAIN: Creating thread
MAIN: Destroying thread attributes
MAIN: Calculating timeout interval
MAIN: Waiting for thread to finish (timeout: 4 sec)
THREAD: Locking mutex
THREAD: Unlocking mutex
THREAD: Blocking for 3 sec
THREAD: Signalling main thread
THREAD: Returning from thread routine
MAIN: Thread finished in time; computation evaluated to true
MAIN: Unlocking mutex
./prog 4  0.00s user 0.00s system 0% cpu 3.010 total
```

### Did not finnish in time example:
```
$ time ./prog 1
MAIN: Initializing mutex
MAIN: Initializing condition
MAIN: Locking mutex
MAIN: Initializing thread attributes
MAIN: Setting thread detach state
MAIN: Creating thread
MAIN: Destroying thread attributes
MAIN: Calculating timeout interval
MAIN: Waiting for thread to finish (timeout: 1 sec)
THREAD: Locking mutex
THREAD: Unlocking mutex
THREAD: Blocking for 3 sec
MAIN: Thread did not finish in time
./prog 1  0.00s user 0.00s system 0% cpu 1.279 total
```
