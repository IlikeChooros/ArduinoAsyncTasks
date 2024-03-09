/*

ArduinoAsyncTask - Callbacks Example

This example demonstrates how to use the ArduinoAsyncTask library to run a simple
asynchronous task and use a callback function to receive the result.

*/

// Include the ArduinoAsyncTask library
#include <ArduinoAsyncTasks.h>

// Define a simple task function, simply add two numbers
int anotherTask(int a, int b){
    return a + b;
}




void setup(){
    // Start the serial communication
    Serial.begin(115200);

    // Create a new task
    AsyncTask<void()> task([](){
        // This is the task function
        // It will be called in a separate thread
        // You can do any work here
        // For this example, we will just print a message
        Serial.println("Hello from the setup task!");
    });

    // Start the task asynchronously, you can also use .run()
    task();
}

void loop(){
    // Delay for a second
    delay(1000);

    // Create a new task, this time with a return value and parameters
    // The task will add two numbers and return the result, so the
    // callback will receive the result as a parameter
    AsyncTask<int(int, int)> task(anotherTask);

    // Set the callback function with `.then(...)` and start the task with `.run(...)`
    task.then([](int result){
        Serial.println("Result: " + String(result));
    }).run(9, 10);

}