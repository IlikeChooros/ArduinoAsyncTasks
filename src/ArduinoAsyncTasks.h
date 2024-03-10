#pragma once

#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_STM32)
# ifndef INC_ARDUINO_FREERTOS_H
#  error "ArduinoAsyncTasks requires the Arduino_FreeRTOS_Library, please install it from the Library Manager. See the README file on Github for more information."
# endif
#endif

#if defined(ARDUINO_ARCH_AVR)
# error "ArduinoAsyncTasks is not supported on AVR boards, for troubleshooting see the README file on Github."
#endif

#include "AsyncTask.h"

using namespace async_tasks;