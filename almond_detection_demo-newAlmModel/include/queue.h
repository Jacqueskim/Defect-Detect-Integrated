#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE_SIZE 100

typedef struct {
    int data[MAX_QUEUE_SIZE];
    int front, rear;
    int size;
} Queue;

void init_queue(Queue *q);
void oldEnqueue(Queue *q, int value);
int oldDequeue(Queue *q);
int is_empty(Queue *q);
int is_full(Queue *q);
int peek_top(Queue *q);

#endif