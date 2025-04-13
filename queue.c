#include "include/queue.h"

/**
 * Creates a new empty queue.
 */
Queue* createQueue() {
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    if (queue == NULL) {
        return NULL;
    }
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

/**
 * Adds an element to the end of the queue.
 */
void enqueue(Queue *queue, void *data) {
    QueueNode *newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        return;
    }
    newNode->data = data;
    newNode->next = NULL;

    if (queue->rear == NULL) {  // Queue is empty
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

/**
 * Removes and returns the front element of the queue.
 */
void* dequeue(Queue *queue) {
    if (queue == NULL || queue->front == NULL) {
        return NULL;
    }

    QueueNode *temp = queue->front;
    void *data = temp->data;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    queue->size--;
    return data;
}

/**
 * Checks if the queue is empty.
 */
int isEmpty(Queue *queue) {
    return queue->front == NULL;
}

/**
 * Returns the number of elements in the queue.
 */
size_t queueSize(Queue *queue) {
    return queue->size;
}

/**
 * Deletes the entire queue and frees all allocated memory.
 */
void deleteQueue(Queue *queue) {
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}

/**
 * Retrieves the data of the node at a given index.
 * Returns NULL if the index is out of bounds.
 */
void* getQueueNodeAt(Queue *queue, size_t index) {
    if (index >= queue->size) {
        return NULL; // Index out of bounds
    }

    QueueNode *current = queue->front;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

/**
 * Removes a node from the queue at a given index.
 * Returns 1 on success, 0 if index is out of bounds.
 */
int removeNodeAt(Queue *queue, size_t index) {
    if (index >= queue->size || isEmpty(queue)) {
        return 0; // Out of bounds or empty queue
    }

    QueueNode *current = queue->front;
    QueueNode *prev = NULL;

    for (size_t i = 0; i < index; i++) {
        prev = current;
        current = current->next;
    }

    if (prev == NULL) {  // Removing front node
        queue->front = current->next;
    } else {
        prev->next = current->next;
    }

    if (current == queue->rear) {  // Removing last node
        queue->rear = prev;
    }

    free(current);
    queue->size--;
    return 1;  // Success
}

/**
 * Removes a node from the queue by its data pointer.
 * Returns 1 if the node was found and removed, 0 otherwise.
 */
int removeNodeByData(Queue *queue, void *data) {
    if (queue == NULL || data == NULL || queue->front == NULL) {
        return 0;
    }

    QueueNode *current = queue->front;
    QueueNode *prev = NULL;

    while (current != NULL) {
        if (current->data == data) {  // Match found
            if (prev == NULL) {  // Removing front node
                queue->front = current->next;
            } else {
                prev->next = current->next;
            }

            if (current == queue->rear) {  // Removing last node
                queue->rear = prev;
            }

            free(current);
            queue->size--;
            return 1;  // Success
        }
        prev = current;
        current = current->next;
    }

    return 0; // Not found
}
