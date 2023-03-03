#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include "queue.h"
#include "PCA9685.h"

#define NUM_QUEUES 9
#define BASE_OFFSET 1700
#define ENCODER_PER_SCREEN_HEIGHT 500.0
#define ENCODER_BACK_LINE_OFFSET 400
#define QUEUEING_THRESHOLD 150


typedef struct servoController {
    Queue queues[NUM_QUEUES];
} ServoController;

void init_servo_controller(ServoController *sc);
int add_position_to_queue(ServoController *sc, int queueIndex, int position);
int get_next_position(ServoController *sc, int queueIndex);
int is_queue_empty(ServoController *sc, int queueIndex);
void add_Object_To_Queue(ServoController *sc, float x, float y, int currentEncoderVal);
int *check_for_Encoder_Event(ServoController *sc, int encoderVal, int *numIndices);
int servo_mapper(float min_x, float max_x, float x);
#endif