#pragma once

#include <list>


#include "AsyncTask.h"
#include "schedules.h"

BEGIN_TASKS_NAMESPACE

struct _ScheduledTask{
  AsyncTask<> task;
  std::function<struct tm(struct tm*)> schedule;
  struct tm lastExecution;
};

class Scheduler
{
  std::list<struct _ScheduledTask> _tasks;

  static void _executeTask(struct _ScheduledTask& task, struct tm* now);
  public:
  Scheduler();
  ~Scheduler();

  /**
   * @brief Add a task to the scheduler
   * @param task The task to be added
   * @param schedule The schedule of the task
  */
  Scheduler& addTask(const AsyncTask<>& task, const ScheduleLike& schedule);

  /**
   * @brief Run the scheduler
   * @param now The current time, used for the schedules
  */
  void run(struct tm* now);

  /**
   * @brief Stop the scheduler
  */
  void stop();

  /**
   * @brief Pause the scheduler
  */
  void pause();

  /**
   * @brief Resume the scheduler
  */
  void resume();
};

END_TASKS_NAMESPACE
