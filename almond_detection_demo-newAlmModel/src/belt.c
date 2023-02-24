#include "belt.h"
#include "board.h"
#include "mxc_device.h"
#include "lp.h"
#include "mxc_delay.h"

void initializeBelt(){
    mxc_tmr_cfg_t tmr2; // to configure timer
    

    MXC_TMR_Shutdown(COUNTER_TIMER);


    tmr2.pres = TMR_PRES_32;
    tmr2.mode = TMR_MODE_COUNTER;
    tmr2.bitMode = TMR_BIT_MODE_32;
    tmr2.clock = COUNTER_CLOCK_SOURCE;
    tmr2.cmp_cnt = 0x40000;
    tmr2.pol = 1;

    if (MXC_TMR_Init(COUNTER_TIMER, &tmr2, false) != E_NO_ERROR) {
        printf("Failed Counter timer Initialization.\n");
        return;
    }
    const mxc_gpio_cfg_t optional_gpio_cfg_tmr0 = { MXC_GPIO0, (MXC_GPIO_PIN_8), MXC_GPIO_FUNC_ALT2,
                                       MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIO };
    MXC_GPIO_Config(&optional_gpio_cfg_tmr0);

    MXC_TMR_Start(COUNTER_TIMER);

}
void setBeltSpeed(uint8_t DUTY_CYCLE){
    mxc_tmr_cfg_t tmr; // to configure timer

    unsigned int periodTicks = MXC_TMR_GetPeriod(PWM_TIMER, PWM_CLOCK_SOURCE, 1, FREQ);
    unsigned int dutyTicks = periodTicks * DUTY_CYCLE / 100;
    MXC_TMR_Shutdown(PWM_TIMER);
    tmr.pres = TMR_PRES_1;
    tmr.mode = TMR_MODE_PWM;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = PWM_CLOCK_SOURCE;
    tmr.cmp_cnt = periodTicks;
    tmr.pol = 1;

    if (MXC_TMR_Init(PWM_TIMER, &tmr, 1) != E_NO_ERROR) {
        printf("Failed PWM timer Initialization.\n");
        return;
    }
    if(dutyTicks==0) dutyTicks=5;
    if (MXC_TMR_SetPWM(PWM_TIMER, dutyTicks) != E_NO_ERROR) {
        printf("Failed TMR_PWMConfig.\n");
        return;
    }
    MXC_TMR_Start(PWM_TIMER);

}
uint32_t getBeltPosition(){
    uint32_t count=MXC_TMR_GetCount(COUNTER_TIMER);
    //printf("Encoder method Count: %u\n",count);
    return count;
}
void moveBeltDistance(uint32_t inches,uint8_t speed){
    uint32_t startPosition=getBeltPosition();
    uint32_t finalPosition= inches*TICKS_PER_INCH+startPosition;
    setBeltSpeed(speed);
    uint32_t currentPosition=0;
    while(currentPosition<finalPosition){
        currentPosition=getBeltPosition();
        printf("Current: %u\n",currentPosition);
    }
    setBeltSpeed(0);
}