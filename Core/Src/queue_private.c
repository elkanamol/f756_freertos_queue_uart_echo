#include "queue_private.h"
#include <stdint.h>
#include "string.h"


BaseType_t queue_init(CustomQueue_t* queue, uint32_t maxSize) {
    if (maxSize > MAX_QUEUE_SIZE) {
        return pdFAIL;
    }
    
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->maxSize = maxSize;
    
    queue->mutex = xSemaphoreCreateMutex();
    queue->spaceSemaphore = xSemaphoreCreateCounting(maxSize, maxSize);
    queue->itemSemaphore = xSemaphoreCreateCounting(maxSize, 0);
    
    return (queue->mutex && queue->spaceSemaphore && queue->itemSemaphore) ? pdPASS : pdFAIL;
}


BaseType_t queue_send(CustomQueue_t* queue, const uint8_t* msg, uint32_t msgSize) {
    if (msgSize > MAX_MSG_SIZE) {
        return pdFAIL;
    }

    // Wait for space to be available
    if (xSemaphoreTake(queue->spaceSemaphore, portMAX_DELAY) != pdTRUE) {
        return pdFAIL;
    }

    // Get exclusive access
    if (xSemaphoreTake(queue->mutex, portMAX_DELAY) != pdTRUE) {
        xSemaphoreGive(queue->spaceSemaphore);
        return pdFAIL;
    }

    // Copy message to queue
    memcpy(queue->data[queue->tail], msg, msgSize);
    queue->tail = (queue->tail + 1) % queue->maxSize;
    queue->count++;

    xSemaphoreGive(queue->mutex);
    xSemaphoreGive(queue->itemSemaphore);

    return pdPASS;
}


BaseType_t queue_receive(CustomQueue_t* queue, uint8_t* msg, uint32_t msgSize) {
    if (msgSize > MAX_MSG_SIZE) {
        return pdFAIL;
    }

    // Wait for an item to be available
    if (xSemaphoreTake(queue->itemSemaphore, portMAX_DELAY) != pdTRUE) {
        return pdFAIL;
    }

    // Get exclusive access
    if (xSemaphoreTake(queue->mutex, portMAX_DELAY) != pdTRUE) {
        xSemaphoreGive(queue->itemSemaphore);
        return pdFAIL;
    }

    // Copy message from queue
    memcpy(msg, queue->data[queue->head], msgSize);
    // Clear the message in the queue
    memset(queue->data[queue->head], 0, msgSize);
    queue->head = (queue->head + 1) % queue->maxSize;
    queue->count--;

    xSemaphoreGive(queue->mutex);
    xSemaphoreGive(queue->spaceSemaphore);

    return pdPASS;
}


uint32_t queue_get_count(CustomQueue_t* queue) {
    uint32_t count;
    
    xSemaphoreTake(queue->mutex, portMAX_DELAY);
    count = queue->count;
    xSemaphoreGive(queue->mutex);
    
    return count;
}
