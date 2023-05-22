#include <stdio.h>
#include <stdlib.h>
#include "interuptQueue.h"

// Initialize a new queue
EventQueue* createQueue() {
    EventQueue* q = (EventQueue*) malloc(sizeof(EventQueue));
    if(q == NULL) {
        printf("Error! unable to create a new queue\n");
        exit(-1);
    }

    q->front = 0;
    q->rear = SIZE - 1; // So that after first enqueue, rear becomes 0.
    q->count = 0;

    return q;
}

// Check if the queue is full
int isFull(EventQueue* q) {
    return q->count == SIZE;
}

// Check if the queue is empty
int isEmpty(EventQueue* q) {
    return q->count == 0;
}

// Add an element to the queue
void enqueue(EventQueue* q, iqrEventInfo data) {
    if(isFull(q)) {
        printf("Error! EventQueue is full\n");
        return;
    }

    q->rear = (q->rear + 1) % SIZE;
    q->data[q->rear] = data;
    q->count++;
}

// Remove an element from the queue
iqrEventInfo dequeue(EventQueue* q) {
    if(isEmpty(q)) {
        printf("Error! EventQueue is empty\n");
        exit(-1);
    }

    iqrEventInfo data = q->data[q->front];
    q->front = (q->front + 1) % SIZE;
    q->count--;

    return data;
}

// Check the front value of the queue
iqrEventInfo peek(EventQueue* q) {
    if(isEmpty(q)) {
        printf("Error! EventQueue is empty\n");
        exit(-1);
    }

    return q->data[q->front];
}
