

/** \file timer.h
 * Timer for embedded projects
 *
 */

/*! \mainpage Timer
 *
 * \section Description
 *
 * This library provides up to [MAX_TIMERS] unique timers with 1ms resolution. Before using a timer, it must first be registered and check for non NULL return value of the ID.
 *
 * \note This library does not use dynamic memory as it is intended for embedded projects.  All memory is assigned at compile time.  Use [MAX_TIMERS] to set the amount of memory used.
 *
 * \section How To Use
 *
 * Update [MAX_TIMERS] to the number of timers that are required for the project.
 *
 * Register with the module by calling timer_registerCB for interrupt type timers or timer_register for polling type timers.
 *
 * Finish code in timer_init() to setup a 1ms timer with interrupts - specific to your MCU
 *
 * Call interruptHandler() in the interrupt that was just created
 *
 * Add timer_run() to your main loop to poll for expired timers
 *
 *
 *
 */

#define MAX_TIMERS  5

/**
 * \enum timerRetVal_t
 * \brief return codes of functions
 */

typedef enum timerRetVal_t{
    TIMER_ERR_NO_FREE_ID = -3,
    TIMER_ERR_ID_OUT_OF_RANGE,
    TIMER_ERR_ID_UNREGISTERD,
    TIMER_SUCCESS, //0
}timerRetVal_t;

/**
 * \enum timerType_t
 * \brief timer type - check expiry in and run expiry function in interrupt or polling mode
 */

typedef enum TimerType{
    TIMER_POLL,
    TIMER_INTERRUPT
}timerType_t;

/**
 * \enum timerStatus_t
 * \brief Timer status
 */

typedef enum TimerStatus{
    TIMER_UNUSED,
    TIMER_RUNNING,
    TIMER_EXPIRED,
    TIMER_STOPPED
}timerStatus_t;

/**
 * \enum expiryActino
 * \brief Function that runs on expiry must return with one of these values
 */

enum expiryAction {
	NONE,
	RESTART,
};

typedef enum expiryAction (*action)(uint8_t timerID);

timerRetVal_t timer_init();
void interruptHandler();

void timer_run();
timerRetVal_t timer_registerCB(uint8_t *timerID, uint32_t period, timerType_t timerProcessType, action expiry);
timerRetVal_t timer_register(uint8_t *timerID, uint32_t period);
timerRetVal_t timer_free(uint8_t timerID);
timerRetVal_t timer_start(uint8_t timerID);
timerRetVal_t timer_setPeriod(uint8_t timerID, uint32_t period);
timerRetVal_t timer_free(uint8_t timerID);
int8_t timer_isExpired(uint8_t timerID);
int8_t timer_isRunning(uint8_t timerID);
uint32_t timer_timeRemaining (uint8_t timerID);

uint32_t timer_getTick();
