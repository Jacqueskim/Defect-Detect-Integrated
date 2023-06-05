#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE_SIZE 100

typedef struct pair{
    int first;
    int second;
} Pair;

typedef struct queue {
    Pair data[MAX_QUEUE_SIZE];
    int front, rear;
    int size;
} Queue;

void init_queue(Queue *q);
void enqueue(Queue *q, int value);
void iterate_top(Queue *q);
Pair dequeue(Queue *q);
int is_empty(Queue *q);
int is_full(Queue *q);
int peek_top(Queue *q);
int peek_top_second(Queue* q);


#endif