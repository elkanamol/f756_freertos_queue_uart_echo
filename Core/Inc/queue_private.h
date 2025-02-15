#ifndef QUEUE_PRIVATE_H
#define QUEUE_PRIVATE_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"

#define MAX_QUEUE_SIZE 16
#define MAX_MSG_SIZE 100

typedef struct {
    uint8_t data[MAX_QUEUE_SIZE][MAX_MSG_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    uint32_t maxSize;
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t spaceSemaphore;
    SemaphoreHandle_t itemSemaphore;
} CustomQueue_t;

// Function prototypes
/**
 * @brief Initializes a custom queue with the specified maximum size.
 *
 * @param queue Pointer to the custom queue to be initialized.
 * @param maxSize The maximum number of items the queue can hold.
 * @return pdPASS if the queue was successfully initialized, pdFAIL otherwise.
 */
BaseType_t queue_init(CustomQueue_t* queue, uint32_t maxSize);

/**
 * @brief Sends a message to the custom queue.
 *
 * @param queue Pointer to the custom queue to send the message to.
 * @param msg Pointer to the message to be sent.
 * @param msgSize The size of the message in bytes.
 * @return pdPASS if the message was successfully sent, pdFAIL otherwise.
 */
BaseType_t queue_send(CustomQueue_t* queue, const uint8_t* msg, uint32_t msgSize);

/**
 * @brief Receives a message from the custom queue.
 *
 * @param queue Pointer to the custom queue to receive the message from.
 * @param msg Pointer to the buffer to store the received message.
 * @param msgSize The size of the message buffer in bytes.
 * @return pdPASS if a message was successfully received, pdFAIL otherwise.
 */
BaseType_t queue_receive(CustomQueue_t* queue, uint8_t* msg, uint32_t msgSize);

/**
 * @brief Gets the current count of items in the custom queue.
 *
 * @param queue Pointer to the custom queue to get the item count from.
 * @return The current count of items in the queue.
 */
uint32_t queue_get_count(CustomQueue_t* queue);

#endif
