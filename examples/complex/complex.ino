/*

ArduinoAsyncTask - complex

An complex example of using the ArduinoAsyncTask library to run tasks
Has more detailed comments about the library implementation and how to use it

*/

// Include the ArduinoAsyncTask library
#include <ArduinoAsyncTasks.h>


int calculateFactorial(int n){
    // Calculate the factorial of a number
    for (int i = n - 1; i > 1; i--){
        n *= i;
    }
    return n;
}

void setup(){
    // Start the serial communication
    Serial.begin(115200);
}

void loop(){
    // Delay for a second
    delay(1000);

    // Create in a loop 10 tasks
    for (int i = 0; i < 10; i++){
        // Create a new task with a return value and parameters
        AsyncTask<int(int)> task(calculateFactorial);

        // Set the callback function with `.then(...)` and start the task with `.run(...)`
        // The `i` value is copied and stored internally as a tuple, so it will be available when the task is executed
        task.then([](int result){
            // Print the result, note that we can print these values because the `Serial` has
            // multithread support, so it allows us to print from different threads
            Serial.printf("Result: %i \n", result);

            // We can't print also the `i` because:
            // - the scope of the lambda function is different from the loop
            // - by the time the task is executed, the loop has already finished (without the delay)
            // Serial.printf("Result: %i, i: %i \n", result, i);
            // Will cause an undefined behavior, because the `i` will be already out of scope
            // if you want to print the `i` you can make `calculateFactorial` return an struct with the result and the `i` value
        }).run(i);

        // Small delay to avoid the tasks to be executed at the same time, causing the output to be mixed
        // This is not necessary, but it makes the output more readable
        delay(250);
    }
}