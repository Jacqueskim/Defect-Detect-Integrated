#ifndef SERVOSTOPPER_H
#define SERVOSTOPPER_H

#define ARRAY_SIZE 9
#define SERVO_CLOSE_OFFSET 10000



typedef struct {
    int servo_stopper[ARRAY_SIZE];
} ServoStopper;

void init_servo_stopper(ServoStopper *stopper);
int stop_servo(ServoStopper *stopper, int servo_number, int encoderVal);
void add_to_servo_count(ServoStopper *stopper, int servo_number, int count);
int *check_for_Close_Event(ServoStopper *stopper, int encoderVal, int *numIndices);

#endif // SERVOSTOPPER_H