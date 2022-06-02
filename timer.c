#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "timer.h"

static uint32_t systick;

struct Timer{
    timerStatus_t status;
    uint32_t startTime;
    uint32_t period;
    timerType_t callbackType;
    action expiryFunction;
};

static struct Timer timerList [MAX_TIMERS];

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

timerRetVal timer_init(){
    //setup hardware for a 1ms timer interrupt

    return 0;
}

//run in main loop to poll for expired timers
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

timerRetVal timer_registerCB(uint8_t *timerID, uint32_t period, timerType_t timerProcessType, action expiry){
    int8_t result;
    result = timer_register(timerID, period);
    if (result == TIMER_SUCCESS){
        struct Timer *timerInfo = &timerList[*timerID];
        timerInfo->callbackType = timerProcessType;
        timerInfo->expiryFunction = expiry;
    }
    return result;
}

timerRetVal timer_register(uint8_t *timerID, uint32_t period){
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

timerRetVal timer_free(uint8_t timerID){
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

timerRetVal timer_start(uint8_t timerID){
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

timerRetVal timer_setPeriod(uint8_t timerID, uint32_t period){
    if (timerID > 0 && timerID < MAX_TIMERS){
        struct Timer *timerInfo = &timerList[timerID];
        if (timerInfo->status == TIMER_UNUSED){
            return TIMER_ERR_ID_UNREGISTERD;
        }
        timerInfo->period = period;
    }
    return TIMER_ERR_ID_OUT_OF_RANGE;
}

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

uint32_t timer_timeRemaining (uint8_t timerID){
    if (timerID >= 0 && timerID < MAX_TIMERS){
        struct Timer *timerInfo = &timerList[timerID];
        return (timerInfo->startTime + timerInfo->period) - timer_getTick();
    }
    return 0; //error
}

uint32_t timer_getTick(){
    return systick;
}
