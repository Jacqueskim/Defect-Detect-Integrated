#ifndef SERVOSTOPPER_H
#define SERVOSTOPPER_H

#define ARRAY_SIZE 9




typedef struct {
    int servo_stopper[ARRAY_SIZE];
} ServoStopper;

void init_servo_stopper(ServoStopper *stopper);
int stop_servo(ServoStopper *stopper, int servo_number, int encoderVal);
void add_to_servo_count(ServoStopper *stopper, int servo_number, int count);

#endif // SERVOSTOPPER_H