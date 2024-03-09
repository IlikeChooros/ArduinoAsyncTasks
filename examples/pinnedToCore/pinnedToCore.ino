/*

ArduinoAsyncTask - Pinned To Core Example

This example demonstrates how to use the ArduinoAsyncTask library to run a simple
asynchronous task on a specific core.

*/

// Include the ArduinoAsyncTask library
#include <ArduinoAsyncTasks.h>


void setup(){
    // Start the serial communication
    Serial.begin(115200);
}

void loop(){
    // Delay for a second
    delay(1000);

    // Create a new task with a return value and parameters
    AsyncTask<int(int, int)> task([](int a, int b){
        // This is the task function
        // It will be called in a separate thread
        // You can do any work here
        // For this example, we will just add two numbers and return the result
        return a + b;
    });

    // Set the custom parameters
    TaskParams params;
    // Set the task name (optional)
    params.name = "Pinned Task";
    // Set `usePinnedCore` to `true` to run the task on a specific core, by default it is `false`
    params.usePinnedCore = true;
    // Set the core to run the task on, by default it is `0`
    params.core = 1;

    // Set the custom parameters with `setParams(...)` and callback function with `.then(...)`
    task.setParams(params).then([](int result){
        Serial.println("Result: " + String(result));
    });

    // Start the task with the parameters
    task(9, 10);
}