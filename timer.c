/** \file timer.c
 * Timer for embedded project
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "timer.h"

static uint32_t systick;

/**
* Struct definition individual timer
*/

struct Timer{
    timerStatus_t status;       /**< timerStatus_t UNUSED, STOPPED, RUNNING, EXPIRED*/
    uint32_t startTime;         /**< time stamp of start time*/
    uint32_t period;            /**< Time in ms until expiry*/
    timerType_t callbackType;   /**< TIMER_POLL or TIMER_INTERRUPT*/
    action expiryFunction;      /**< Function pointer to function that runs on timer expiry.  Return value of this function must be enum expiryAction NONE or RESTART*/
};

/**
* Array of struct to hold all the timers
*/

static struct Timer timerList [MAX_TIMERS];


/** \brief Interrupt Handler
 *
 * \details To be run in 1 periodic 1ms interrupt.  Checks for expired timers of type TIMER_INTERRUPT and runs an expiry function if there is one.
 *
 */

void interruptHandler(){
    systick++;
    //comment below if you dont want to process timers in interrupt
    for (uint8_t i=0; i<MAX_TIMERS; i++){
        struct Timer *timerInfo = &timerList[i];
		if (timerInfo->status == TIMER_RUNNING && timerInfo->callbackType == TIMER_INTERRUPT){
            if (systick - timerInfo->startTime >= timerInfo->period){
                timerInfo->status = TIMER_EXPIRED;
                //check for cbfunction and run it
                if(timerInfo->expiryFunction != NULL){
                    enum expiryAction result = timerInfo->expiryFunction(i);
                    if (result == RESTART){
                        timerInfo->status = TIMER_RUNNING;
                        timerInfo->startTime += timerInfo->period;
                    }
                }
            }
        }
    }
}

/** \brief Timer hardware initialization
 *
 * \details MCU dependant code for starting a 1ms timer with interrupt
 *
 * \return timerRetVal_t (optional)
 *
 */


timerRetVal_t timer_init(){
    timerRetVal_t ret = 0;
    //setup hardware for a 1ms timer interrupt

    return ret;
}


/** \brief Process Timers - Run in main loop
 * \details Checks for expired timers and runs a function on expiry if one is set. Timers with type TIMER_POLL.
 */

void timer_run(){
    static uint32_t oldTime;
    uint32_t timeNow = timer_getTick();
    if (oldTime != timeNow){
        oldTime = timeNow;
        for (uint8_t i=0; i<MAX_TIMERS; i++){
			struct Timer *timerInfo = &timerList[i];
			if (timerInfo->status == TIMER_RUNNING && timerInfo->callbackType == TIMER_POLL){
                if ((timeNow - timerInfo->startTime) >= timerInfo->period){
                    timerInfo->status = TIMER_EXPIRED;
                    //check for cbfunction and run it
                    if(timerInfo->expiryFunction != NULL){
                        enum expiryAction result = timerInfo->expiryFunction(i);
						if (result == RESTART){
							timerInfo->status = TIMER_RUNNING;
							timerInfo->startTime += timerInfo->period;
						}
                    }
                }
			}
        }
    }
}

/** \brief Register a timer and set a function to run on timer expiry
 *
 * \param timerID   ID of registered timer is set - check for NULL before using
 * \param period    Time until expired
 * \param timerProcessTYpe  Select TIMER_POLL or TIMER_INTERRUPT.
 * \param expiry    Function pointer to function that runs on expiry.
 * \return          timerRetVal_t
 *
 */


timerRetVal_t timer_registerCB(uint8_t *timerID, uint32_t period, timerType_t timerProcessType, action expiry){
    int8_t result;
    result = timer_register(timerID, period);
    if (result == TIMER_SUCCESS){
        struct Timer *timerInfo = &timerList[*timerID];
        timerInfo->callbackType = timerProcessType;
        timerInfo->expiryFunction = expiry;
    }
    return result;
}

/** \brief Register a timer that does not have a function to run on expiry.  Defaults to TIMER_POLL
 *
 * \param timerID   ID of timer
 * \param period    time until timer expires
 * \return          timerRetVal_t
 *
 */

timerRetVal_t timer_register(uint8_t *timerID, uint32_t period){
    for (uint8_t i=0; i<MAX_TIMERS; i++){
        struct Timer *timerInfo = &timerList[i];
        if (timerInfo->status == TIMER_UNUSED){
            timerInfo->period = period;
            timerInfo->callbackType = TIMER_POLL;
            *timerID = i;
            timer_start(*timerID);
            return TIMER_SUCCESS;
        }
    }

    return TIMER_ERR_NO_FREE_ID;
}

/** \brief Free a Timer ID
 *  \details    Set a timer to unused so that the ID can be reused elsewhere
 * \param   timerID ID of timer to free
 * \return timerRetVal_t
 *
 */

timerRetVal_t timer_free(uint8_t timerID){
    if (timerID >= 0 && timerID < MAX_TIMERS){
        struct Timer *timerInfo = &timerList[timerID];
        if (timerInfo->status == TIMER_UNUSED){
            return TIMER_ERR_ID_UNREGISTERD;
        }
        timerInfo->startTime = 0;
        timerInfo->period = 0;
        timerInfo->status = TIMER_UNUSED;
        return TIMER_SUCCESS;
    }
    return TIMER_ERR_ID_OUT_OF_RANGE;
}

/** \brief Start Timer
 *
 * \param timerID   ID of timer to start
 * \return timerRetVal_t
 *
 */


timerRetVal_t timer_start(uint8_t timerID){
    if (timerID >= 0 && timerID < MAX_TIMERS){
        struct Timer *timerInfo = &timerList[timerID];
        timerInfo->startTime = timer_getTick();
        if (timerInfo->period > 0){
            timerInfo->status = TIMER_RUNNING;
        }
        else{
            timerInfo->status = TIMER_STOPPED;
        }
        return TIMER_SUCCESS;
    }
    return TIMER_ERR_ID_OUT_OF_RANGE;
}

/** \brief Set the period of the timer
 *
 * \param timerID   ID of timer to set the period of
 * \param period    period in ms
 * \return timerRetVal_t
 *
 */

timerRetVal_t timer_setPeriod(uint8_t timerID, uint32_t period){
    if (timerID > 0 && timerID < MAX_TIMERS){
        struct Timer *timerInfo = &timerList[timerID];
        if (timerInfo->status == TIMER_UNUSED){
            return TIMER_ERR_ID_UNREGISTERD;
        }
        timerInfo->period = period;
    }
    return TIMER_ERR_ID_OUT_OF_RANGE;
}

/** \brief Timer Expiry Checl
 *
 * \param timerID   timer to check
 * \return 1: expired, otherwise 0
 *
 */


int8_t timer_isExpired(uint8_t timerID){
    if (timerID >= 0 && timerID < MAX_TIMERS){
        struct Timer *timerInfo = &timerList[timerID];
        if (timerInfo->status == TIMER_EXPIRED){
            return 1;
        }
        else{
            return 0;
        }
    }
    return TIMER_ERR_ID_OUT_OF_RANGE;
}

/** \brief Check if timer is running
 *
 * \param timerID   ID of timer to check
 * \return returns 1 if RUNNING otherwise 0
 *
 */

int8_t timer_isRunning(uint8_t timerID){
    if (timerID >= 0 && timerID < MAX_TIMERS){
        struct Timer *timerInfo = &timerList[timerID];
        if (timerInfo->status == TIMER_RUNNING){
            return 1;
        }
        else{
            return 0;
        }
    }
    return TIMER_ERR_ID_OUT_OF_RANGE;
}

/** \brief Check time remaining
 *
 * \param timerID   ID of timer to check remaining time
 * \return ms until expiry
 *
 */


uint32_t timer_timeRemaining (uint8_t timerID){
    if (timerID >= 0 && timerID < MAX_TIMERS){
        struct Timer *timerInfo = &timerList[timerID];
        return (timerInfo->startTime + timerInfo->period) - timer_getTick();
    }
    return 0; //error
}

/** \brief get system tick count
 *
 * \return current tick count
 *
 */

uint32_t timer_getTick(){
    return systick;
}
