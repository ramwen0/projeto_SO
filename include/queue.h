#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

// Queue Node structure
typedef struct QueueNode {
    void *data;
    struct QueueNode *next;
} QueueNode;

// Queue structure
typedef struct {
    QueueNode *front;
    QueueNode *rear;
    size_t size;
} Queue;

// Queue Operations
Queue* createQueue();
void enqueue(Queue *queue, void *data);
void* dequeue(Queue *queue);
int isEmpty(Queue *queue);
size_t queueSize(Queue *queue);
void deleteQueue(Queue *queue);

void* getQueueNodeAt(Queue *queue, size_t index);
int removeNodeAt(Queue *queue, size_t index);
int removeNodeByData(Queue *queue, void *data);

#endif /* QUEUE_H */
