#pragma once

#include <list>

#include "./AsyncTask.h"
#include "./schedules.h"
#include "./lock.h"

BEGIN_TASKS_NAMESPACE

struct _ScheduledTask{
  // task is the task to be executed
  AsyncTask<> task;
  // schedule for the task
  ScheduleParams schedule;
  // nextExecution is used to store the next time the task should be executed
  tm nextExecution; 

  _ScheduledTask(): 
    task(AsyncTask<>()), schedule(ScheduleParams()) {}

  _ScheduledTask(const AsyncTask<>& task, const ScheduleParams& schedule):
    task(task), schedule(schedule) {}

  _ScheduledTask(const _ScheduledTask& other):
    task(other.task), schedule(other.schedule), nextExecution(other.nextExecution) {}
};

/*

## Scheduler

The Scheduler class is used to schedule tasks to be executed at a certain time. 


### Example

```cpp

Scheduler scheduler;
// add a task to the scheduler, that prints "Hello, World!" every second
scheduler.addTask(AsyncTask<>([](){
  Serial.println("Hello, World!");
}), ScheduleParams().every(1, TimeUnit::Seconds));


// run the scheduler
scheduler.run();
```
*/
class Scheduler
{
  static int _instance_count;
  std::unique_ptr<_TaskData> _taskData;
  tm *_now;
  std::list<struct _ScheduledTask> _tasks;
  TaskParams _params;
  bool _self_update;

  double _executeTask(struct _ScheduledTask& task);

  static void _taskRunner(void* param);

  public:
  // Create a new scheduler
  // @throws std::runtime_error if the scheduler is already created, 
  // only one instance of `Scheduler` is allowed
  Scheduler();
  ~Scheduler();

  /**
   * @brief Set the parameters for the scheduler task
   * @param params The parameters to be set
   * @return *this
  */
  Scheduler& setParams(const TaskParams& params);

  /**
   * @brief Set the scheduler to update the time itself, default is false
   * @param self_update Whether the scheduler should update the time itself
   * @return *this
  */
  Scheduler& setSelfTimeUpdate(bool self_update = false);

  /**
   * @brief Add a task to the scheduler
   * @param task The task to be added
   * @param schedule The schedule of the task
   * @return *this
  */
  Scheduler& addTask(const AsyncTask<>& task, ScheduleParams schedule);

  /**
   * @brief Add a task to the scheduler
   * @param task The task to be added
   * @param schedule The schedule of the task
   * @return *this
  */
  Scheduler& addTask(std::function<void(void)> task, const ScheduleParams& schedule);

  /**
   * @brief Update the time of the scheduler
   * @param now The current time
   * @return *this
  */
  Scheduler& updateNow(struct tm* now);

  /**
   * @brief Run the scheduler
   * @param now The current time used for the schedules, 
   * if not provided the current time will be used
  */
  void run(struct tm* now = nullptr);

  /**
   * @brief Stop the scheduler, killing all the tasks
  */
  void stop();

  /**
   * @brief Pause the scheduler, you can resume the scheduler using `resume()`
  */
  void pause();

  /**
   * @brief Resume the scheduler, after
  */
  void resume();
};

END_TASKS_NAMESPACE
