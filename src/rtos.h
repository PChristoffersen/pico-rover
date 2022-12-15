#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#include <message_buffer.h>
#include <stream_buffer.h>
#pragma GCC diagnostic pop


#define TEST_TASK_PRIORITY				    ( tskIDLE_PRIORITY + 10UL )


#define USBD_TASK_PRIORITY  				( configMAX_PRIORITIES - 1UL )

#define RECEIVER_TASK_PRIORITY              ( configMAX_PRIORITIES - 2UL )
#define ENCODER_TASK_PRIORITY               ( configMAX_PRIORITIES - 2UL )
#define IMU_TASK_PRIORITY   		        ( configMAX_PRIORITIES - 5UL )

#define RECEIVERLISTENER_TASK_PRIORITY      ( tskIDLE_PRIORITY + 10UL )
#define RECEIVER_LOWER_TASK_PRIORITY        ( tskIDLE_PRIORITY + 10UL )
#define ROS_TASK_PRIORITY                   ( tskIDLE_PRIORITY + 10UL )

#define ADC_TASK_PRIORITY   		        ( tskIDLE_PRIORITY + 4UL )
#define INA_TASK_PRIORITY   		        ( tskIDLE_PRIORITY + 4UL )

#define LED_TASK_PRIORITY  				    ( tskIDLE_PRIORITY + 3UL )
#define DISPLAY_TASK_PRIORITY  				( tskIDLE_PRIORITY + 2UL )
#define LED_ANIMATION_TASK_PRIORITY    		( tskIDLE_PRIORITY + 2UL )
#define LED_BLINK_TASK_PRIORITY				( tskIDLE_PRIORITY + 1UL )

