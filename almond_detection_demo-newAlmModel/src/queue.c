#include <stdio.h>
#include "queue.h"

void init_queue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

void enqueue(Queue *q, int value) {
    if (is_full(q)) {
        printf("Queue is full\n");
        return;
    }

    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->data[q->rear] = value;
    q->size++;
}

int dequeue(Queue *q) {
    if (is_empty(q)) {
        return -1;
    }

    int value = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return value;
}

int peek_top(Queue *q) {
    if (is_empty(q)) {
        return -1;
    }

    return q->data[q->front];
}

int is_empty(Queue *q) {
    return q->size == 0;
}

int is_full(Queue *q) {
    return q->size == MAX_QUEUE_SIZE;
}