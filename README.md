# ArduinoAsyncTasks

ArduinoAsyncTasks is a simple and lightweight library for running asynchronous tasks on Arduino. It supports lambda functions and is easy to use. This library is particularly useful for managing multiple tasks or processes that need to run concurrently.
Best for multicores boards like ESP32 or Arduino Due.

## Features

- Run tasks asynchronously
- Use lambda functions as tasks
- Lightweight and easy to use
- Allows custom parameters for tasks

## Installation

If you are using the Arduino board, you will have to install the `FreeRTOS` library before installing the `ArduinoAsyncTasks` library. 

To install the FreeRTOS library, follow these steps:
  1. In the Arduino IDE, navigate to Tools > Manage Libraries.
  2. Search for `FreeRTOS` and click Install.

If you are using the ESP32 board, you don't need to install the `FreeRTOS` library, because it is already included in the board's core.

To install the ArduinoAsyncTasks library, follow these steps:
  1. Download the [latest release]() as a .ZIP file.
  2. In the Arduino IDE, navigate to Sketch > Include Library > Add .ZIP Library.
  3. At the top of your sketch, include the library with `#include <ArduinoAsyncTasks.h>`.
  4. You're ready to go!

## Usage

To use the ArduinoAsyncTasks library, you need to create an instance of the `AsyncTask` class and pass a lambda function to its constructor. The lambda function will be the task that is run asynchronously.

Here's a basic example:

```cpp
#include <ArduinoAsyncTasks.h>

void setup() {
  // Start the serial monitor
  Serial.begin(9600);
}

void loop() {
  // Delay for 1 second
  delay(1000);

  for (int i = 0; i < 10; i++) {
    // Create an asynchronous task with a lambda function and integer parameter
    AsyncTask<int> task([](int i) {
      Serial.println("Task " + String(i) + " is running asynchronously");
    });

    // Run the task asynchronously with the integer parameter
    task(i);
  }
}
```

In this example, the `AsyncTask` runs the lambda function asynchronously, allowing the main `loop()` function to continue running without waiting for the task to finish.

## More Information

For more information, see the [source code](https://github.com/IlikeChooros/ArduinoAsyncTasks/tree/main/src) or the [examples](https://github.com/IlikeChooros/ArduinoAsyncTasks/tree/main/examples) in the GitHub repository.

The library uses the [FreeRTOS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html#tasks), so if you want to play around with `TaskParams` or learn more about how tasks are managed, you should check out the FreeRTOS documentation.

## Troubleshooting

"Error: Include FreeRTOS.h before including this file, for troubleshooting see the README file on Github."

If you see this error, it means that you need to install the `FreeRTOS` library. You can do this by navigating to Tools > Manage Libraries and searching for `FreeRTOS`. Then, click Install.

Then, you need to include the `FreeRTOS.h` file before including the `ArduinoAsyncTasks.h` file in your sketch.

```cpp

#include <FreeRTOS.h>
#include <ArduinoAsyncTasks.h>

```


## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue on the [GitHub repository](https://github.com/IlikeChooros/ArduinoAsyncTasks).