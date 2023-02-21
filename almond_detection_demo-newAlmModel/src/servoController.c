#include <stdio.h>
#include <stdlib.h>
#include "servoController.h"

void init_servo_controller(ServoController *sc) {
    for (int i = 0; i < NUM_QUEUES; i++) {
        init_queue(&sc->queues[i]);
    }
}

void add_position_to_queue(ServoController *sc, int queueIndex, int position) {
    if (queueIndex >= 0 && queueIndex < NUM_QUEUES) {
        enqueue(&sc->queues[queueIndex], position);
    } else {
        printf("Invalid queue index\n");
    }
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
    int servo = (int)x*9.0;
    int newEncoderVal = currentEncoderVal;
    newEncoderVal += ENCODER_PER_INCH * LENGTH_OF_BELT_FROM_CAMERA_INCHES;
    newEncoderVal += ENCODER_PER_SCREEN_HEIGHT * (1-y); // depends on the camera orientation if we need to flip the y or not

    if(servo % 2 == 1) // back line
    {
        newEncoderVal += ENCODER_BACK_LINE_OFFSET;
    }
    add_position_to_queue(sc,servo, newEncoderVal);
}
int *check_for_Encoder_Event(ServoController *sc, int encoderVal, int *numIndices) {
    int *indices = (int*) malloc(NUM_QUEUES * sizeof(int));
    *numIndices = 0;

    for (int i = 0; i < NUM_QUEUES; i++) {
        if (!is_queue_empty(sc, i)) {
            int topVal = peek_top(&sc->queues[i]);
            if (topVal > encoderVal) {
                indices[*numIndices] = i;
                (*numIndices)++;
                dequeue(&sc->queues[i]);
            }
        }
    }
    return indices;
}