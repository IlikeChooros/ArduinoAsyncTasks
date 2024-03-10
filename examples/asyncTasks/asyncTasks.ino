/*

ArduinoAsyncTask - asyncTasks

An example of using the ArduinoAsyncTask library to run tasks
Has more detailed comments about the library implementation and how to use it

*/

// Include the ArduinoAsyncTask library
#include <ArduinoAsyncTasks.h>

// The task, must be a void function
void calculateFactorial(int n){
    // The result of the factorial, should be a big number
    unsigned long long result = n;

    // Calculate the factorial of a number
    for (int i = n - 1; i > 1; i--){
        result *= i;
    }

    // If you want to see that these tasks are running in parallel,
    // you can add a random delay here, and you will see that the messages
    // are not printed in order
    delay(random(20, 200));

    // Print the result, if the message is mixed with other tasks,
    // it's because the tasks are running in parallel
    Serial.println("The factorial of " + String(n) + " is: " + String(result));
}

void setup(){
    // Start the serial communication
    Serial.begin(115200);
    
    // Create a new task with no parameters, the task is a lambda function
    // with no return value (void)
    AsyncTask<> task([](){
        Serial.println("Hello from a task!");
    });
    task();

    // Initialize random seed
    randomSeed(analogRead(A0));
}

void loop(){
    // Delay for a second
    delay(1000);

    Serial.println("\nCreating tasks...");

    // Create in a loop 25 tasks, each task will calculate the factorial of a number
    for (int i = 25; i > 0; i--){
        // Create a new task with an int as parameter
        AsyncTask<int> task(calculateFactorial);

        // Set the callback function with `.then(...)` and start the task with `.run(...)`
        // The `i` value is copied and stored internally as a tuple,
        // so it will be available when the task is executed
        task(i);

        // Small delay to avoid the tasks to be executed at the same time,
        // causing the output to be mixed. This is not necessary, 
        // but it makes the output more readable
        // delay(50);
    }
}