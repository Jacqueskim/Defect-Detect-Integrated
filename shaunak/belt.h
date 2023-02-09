#ifndef BELT_H
#define BELT_H
#define PWM_CLOCK_SOURCE MXC_TMR_32K_CLK // \ref mxc_tmr_clock_t
#define FREQ 100 // (Hz)
#define PWM_TIMER MXC_TMR4 // must change PWM_PORT and PWM_PIN if changed#define I2C_FREQ 100000 // 100kHZ

#define COUNTER_TIMER MXC_TMR0 // must change PWM_PORT and PWM_PIN if changed
#define COUNTER_CLOCK_SOURCE MXC_TMR_32K_CLK // \ref mxc_tmr_clock_t
#define TICKS_PER_INCH 145

#include <stdint.h>
#include <stdio.h>
void initializeBelt();
void setBeltSpeed(uint8_t DUTY_CYCLE);
uint32_t getBeltPosition();
void moveBeltDistance(uint32_t inches,uint8_t speed);
#endif 