#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void object_detected(float x, float y);
void timerEventHandler();
void enqueueNewEvent(int servoNum, int encoderTime, int close);
    

void create_next_timer_event(int tmr_value);
void interuptSetup();


#endif /* FUNCTIONS_H */
