# ArduinoAsyncTasks

ArduinoAsyncTasks is a simple and lightweight library for running asynchronous tasks on Arduino.
It supports lambda functions and is easy to use.

## Installation

1. Download the library from the [GitHub repository](https://github.com/IlikeChooros/ArduinoAsyncTasks).
2. In the Arduino IDE, navigate to Sketch > Include Library > Add .ZIP Library.
3. At the top of your sketch, include the library with `#include <ArduinoAsyncTasks.h>`.

## Usage

Here's a basic example of how to use the library:

```cpp
#include <ArduinoAsyncTasks.h>

void setup() {
  // Create a task with default parameters
  AsyncTask<void()> task([]() {
    // Task function
    delay(1000);
    Serial.println("Task completed");
  });

  // Run the task
  task();
}

void loop() {
  // Your main code here
}
```

In this example, the task function simply waits for 1 second and then prints "Task completed" to the serial monitor.

## Task Parameters

You can customize the parameters of a task using the `TaskParams` struct:

```cpp
TaskParams params;
params.stackSize = 8192; // Set the stack size to 8192
params.priority = 1; // Set the task priority to 1
params.name = "MyTask"; // Set the task name to "MyTask"

AsyncTask<void(int, int)> task(params, [](int a, int b) {
  Serial.printf("%i + %i = %i\n", a, b, int(a + b));
});
```

The available parameters are:

- `stackSize`: The stack size for the task. Default is 4096.
- `priority`: The priority of the task. Default is `tskIDLE_PRIORITY`.
- `name`: The name of the task. Default is "Task".
- `usePinnedCore`: Whether to pin the task to a specific core. Default is false.
- `core`: The core to pin the task to (0 or 1). Default is 0.

## Callbacks

You can also specify a callback function to be called when the task completes:

```cpp
AsyncTask<int(std::string, int)> task([](std::string str, int a) {
  Serial.printf("Task with parameters: %s, %i", str.c_str(), a);
  return a + 10;
}, [](int b) {
  // Callback function
  Serial.println("Task completed with parameter: %i\n", b);
});

task("Hello world!", 10);
```
In this example, the callback function simply prints the result of the task to the serial monitor.


## More Information

For more information, see the [source code](https://github.com/IlikeChooros/ArduinoAsyncTasks/tree/main/src) or the [examples](https://github.com/IlikeChooros/ArduinoAsyncTasks/tree/main/examples) in the GitHub repository.
The library uses the [FreeRTOS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html#tasks), if you want to play around with `TaskParameters` you should check out the docs.
