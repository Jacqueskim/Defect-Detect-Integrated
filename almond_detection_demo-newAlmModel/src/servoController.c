#include <stdio.h>
#include <stdlib.h>
#include "servoController.h"

void init_servo_controller(ServoController *sc) {
    for (int i = 0; i < NUM_QUEUES; i++) {
        init_queue(&sc->queues[i]);
    }
}

int add_position_to_queue(ServoController *sc, int queueIndex, int position) {
    if (queueIndex >= 0 && queueIndex < NUM_QUEUES) {
         int topVal = peek_top(&sc->queues[queueIndex]);
         if(topVal < 0 || topVal < (position + QUEUEING_THRESHOLD)){
            enqueue(&sc->queues[queueIndex], position);
            return 1;
         } // otherwise it was from the same object
        
    } else {
        printf("Invalid queue index\n");
    }
    return 0;
}

int get_next_position(ServoController *sc, int queueIndex) {
    if (queueIndex >= 0 && queueIndex < NUM_QUEUES) {
        return dequeue(&sc->queues[queueIndex]);
    } else {
        printf("Invalid queue index\n");
        return -1;
    }
}

int is_queue_empty(ServoController *sc, int queueIndex) {
    if (queueIndex >= 0 && queueIndex < NUM_QUEUES) {
        return is_empty(&sc->queues[queueIndex]);
    } else {
        printf("Invalid queue index\n");
        return 1;
    }
}
void add_Object_To_Queue(ServoController *sc, float x, float y, int currentEncoderVal)
{
    int servo = servo_mapper(0.0,1.0,y);
    int newEncoderVal = currentEncoderVal;
    newEncoderVal += (int)BASE_OFFSET;
    newEncoderVal +=(int)(ENCODER_PER_SCREEN_HEIGHT * (x)); // depends on the camera orientation if we need to flip the y or not

    if(servo > 4) // back line
    {
        newEncoderVal += ENCODER_BACK_LINE_OFFSET;
    }
    int added = add_position_to_queue(sc,servo, newEncoderVal);
    if(added == 1){
        printf("Encoded an object at servo: %u, with an encoder offset of %u \n",servo,(newEncoderVal - currentEncoderVal));
    }else{
        printf("Did not queue due to thresholding");
    }
    
}
int *check_for_Encoder_Event(ServoController *sc, int encoderVal, int *numIndices) {
    int *indices = (int*) malloc(NUM_QUEUES * sizeof(int));
    *numIndices = 0;

    for (int i = 0; i < NUM_QUEUES; i++) {
        int topVal = peek_top(&sc->queues[i]);
        if (topVal < encoderVal && topVal > 0) {
            indices[*numIndices] = i;
            (*numIndices)++;
            dequeue(&sc->queues[i]);
        }
        
    }
    return indices;
}

int servo_mapper(float min_y, float max_y, float y){
    float diff = (max_y - min_y)/5;
    int range = (y - min_y)/diff;
    switch (range)
    {
    case 0:
        return 2;
    case 1:
        return 7;
    case 2:
        return 1;
    case 3:
        return 8;
    case 4:
        return 0;
    }
    
}