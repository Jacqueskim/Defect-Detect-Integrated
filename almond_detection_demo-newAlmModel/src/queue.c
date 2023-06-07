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
    Pair p;
    p.first = value;
    p.second = 1;
    
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->data[q->rear] = p;
    q->size++;
}

Pair dequeue(Queue *q) {
    if (is_empty(q)) {
        Pair x;
        x.first = -1;
        x.second = 1;
        return x;
    }

    Pair value = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return value;
}
void iterate_top(Queue *q){
     if (is_empty(q)) {
        return;
    }
    q->data[q->front].second = q->data[q->front].second + 1;
}

int peek_top(Queue *q) {
    if (is_empty(q)) {
        return -1;
    }

    return q->data[q->front].first;
}

int peek_top_second(Queue* q){
    if(is_empty(q)){
        return 1;
    }
    return q->data[q->front].second;
}

int is_empty(Queue *q) {
    return q->size == 0;
}

int is_full(Queue *q) {
    return q->size == MAX_QUEUE_SIZE;
}