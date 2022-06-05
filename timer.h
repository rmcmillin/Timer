
#define MAX_TIMERS  5

typedef enum timerRetVal_t{
    TIMER_ERR_NO_FREE_ID = -3,
    TIMER_ERR_ID_OUT_OF_RANGE,
    TIMER_ERR_ID_UNREGISTERD,
    TIMER_SUCCESS, //0
}timerRetVal_t;

typedef enum TimerType{
    TIMER_POLL,
    TIMER_INTERRUPT
}timerType_t;

typedef enum TimerStatus{
    TIMER_UNUSED,
    TIMER_RUNNING,
    TIMER_EXPIRED,
    TIMER_STOPPED
}timerStatus_t;

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
