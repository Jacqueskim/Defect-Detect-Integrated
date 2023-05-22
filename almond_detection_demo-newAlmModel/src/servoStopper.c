#include "ServoStopper.h"



void init_servo_stopper(ServoStopper *stopper) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        stopper->servo_stopper[i] = -1;
    }
}

int stop_servo(ServoStopper *stopper, int servo_number, int encoderVal) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
       if(stopper->servo_stopper[i] > 0 && stopper->servo_stopper[i] < encoderVal){
            stopper->servo_stopper[i] = -1;
            return i;
       }
    }
    return -1;
}

void add_to_servo_count(ServoStopper *stopper, int servo_number, int count) {
    if (servo_number >= 0 && servo_number < ARRAY_SIZE) {
        stopper->servo_stopper[servo_number] = count + 250;
    }
}
int *check_for_Close_Event(ServoStopper *stopper, int encoderVal, int *numIndices){
     int *indices = (int*) malloc(ARRAY_SIZE * sizeof(int));
    *numIndices = 0;
    for(int i = 0; i < ARRAY_SIZE; i++){
        if(stopper->servo_stopper[i] < encoderVal && stopper->servo_stopper[i] > 0){
            indices[*numIndices] = i;
            (*numIndices)++;
            stopper->servo_stopper[i] = -1;
        }
    }
}
int getMinValue(ServoStopper* sc, int* index){
    int minV = 1000000;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int topVal = peek_top(&sc->servo_stopper[i]);
        if (topVal > 0 && topVal < minV) {
            minV = topVal;
            (*index) = i;
        }
    
    }
    return minV;
}