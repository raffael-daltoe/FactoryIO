/*
 * main.h
 *
 *  Created on: 10 sept. 2023
 *      Author: lepoi
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#define SENSOR_TABLE_SIZE 2
// Device header
#include "stm32f0xx.h"
#define SIZE_DMA_CHANNEL_5 50
#define SIZE_TABLEAU 5
// BSP functions
#include "bsp.h"

// FreeRTOS headers
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"
#include "stream_buffer.h"
#include "string.h"
// Global functions
int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);

// Define the command_message_t type as an array of xx char
typedef uint8_t command_message_t[SIZE_DMA_CHANNEL_5];
#endif /* INC_MAIN_H_ */
