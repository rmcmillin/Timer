# Timer
 Timer for embedded projects

## How to Use
### Setup
Set the number of timers required for the project in **[timer.h](timer.h?raw=1)**
As the library is meant for embedded projects, it does not use dynamic memory and memory for timers is set at compile time.

```c
//Maximum numer of timers that can be used. 
#define MAX_TIMERS  5
```

Note: This was used for an AVR MCU so the following line is included in the .c file
```c
#include <avr/io.h>
```
Change to stdint.h if required
```c
#include <stdint.h>
```

Finish the function timer_init() with MCU specific code for a 1ms periodic interrupt

In the interrupt vector, run the function interruptHandler().  This checks for expired timers and runs expiry functions for interrupt type timers.

In the main loop, run timer_run().  This checks for expired timers that are poll type.

If registering a function to run on expiry, ensure it returns enum expiry action

```c
enum expiryAction myFunction (uint8_t timerID){
    //Do something here when timer expires
    return RESTART;
}
```

### Register
Register for an ID by calling the function timer_registerCB or timer_register.  timer_registerCB is used for polling mode and timer_registerCB is processed in the interrupts.
```c
uint8_t myID;

if (timer_register(&myID, 1000) != TIMER_SUCCESS){  
  //error
}
```
Ensure you check the return value of the function before using myID.  This ID is used to access a unique timer using the provided functions

timer_free() - sets a timer to unused
timer_start() - starts a timer as long as the period is greater than 0
timer_setPeriod() - sets the time in ms until timer expires
timer_isExpired() - returns 1 if expired otherwise 0
timer_isRunning() - returns 1 if runnning otherwise 0
timer_timeRemaining() - calculates time until expiry


## License
This library is free software; you can redistribute it and/or modify it under
the terms of the MIT license. See [LICENSE](LICENSE) for details.

