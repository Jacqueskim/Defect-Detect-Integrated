#ifndef INTERUPTQUEUE_H
#define INTERUPTQUEUE_H

#define BASE_OFFSET 1500
#define ENCODER_PER_SCREEN_HEIGHT 250.0
#define ENCODER_BACK_LINE_OFFSET 500
#define QUEUEING_THRESHOLD 250


#define SIZE 100 // Maximum queue size

// Definition of the struct
typedef struct {
    int servoIndex;
    int encoderValue;
} iqrEventInfo;

// Definition of the queue
typedef struct {
    iqrEventInfo data[SIZE];
    int front;
    int rear;
    int count;
} EventQueue;

// Function declarations
EventQueue* createQueue();
int isFull(EventQueue* q);
int isEmpty(EventQueue* q);
void enqueue(EventQueue* q, iqrEventInfo data);
iqrEventInfo dequeue(EventQueue* q);
iqrEventInfo peek(EventQueue* q);

#endif