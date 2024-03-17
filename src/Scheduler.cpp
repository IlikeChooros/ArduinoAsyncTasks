#include "Scheduler.h"

BEGIN_TASKS_NAMESPACE

int Scheduler::_instance_count = 0;

// execute a task, and return the time until the next execution in seconds
double Scheduler::_executeTask(struct _ScheduledTask& task){
  if(difftime(mktime(_now), mktime(&task.nextExecution)) >= 0){
    // make a copy of the task, and run it
    AsyncTask<>(task.task).run();
    task.nextExecution = task.schedule.schedule(_now);
  } 
  // return the time until the next execution in seconds
  return difftime(mktime(&task.nextExecution), mktime(_now));
}

void Scheduler::_taskRunner(void* param){
  /*
  
  Main task for the `Scheduler` class.

  This task will run all the tasks in the `Scheduler`'s task list, and sleep
  until the next task is ready to be executed.
  
  */
  Scheduler* scheduler = static_cast<Scheduler*>(param);
  
  for(;;){
    double minTime = INT_MAX;
    auto startTicks = xTaskGetTickCount();
    double sleepTime = 0;

    {
      // lock the mutex
      Lock lock(scheduler->_taskData->_mutex);

      // iterate over all the tasks and execute them
      for(auto it = scheduler->_tasks.begin(); it != scheduler->_tasks.end(); it++){
        minTime = std::min(minTime, scheduler->_executeTask(*it));
      }

      // get the time to sleep
      sleepTime = minTime * 1000 / portTICK_PERIOD_MS;
      // update the time if the scheduler is set to update it
      if (scheduler->_self_update){
        time_t now = mktime(scheduler->_now);
        now += static_cast<time_t>((minTime + (xTaskGetTickCount() - startTicks) * portTICK_PERIOD_MS / 1000));
        scheduler->_now = localtime(&now);
      }
    }
    // sleep for the minimum time until the next task
    vTaskDelay(sleepTime);
    
  }
}

Scheduler::Scheduler():
  _taskData(nullptr), _now(nullptr), _tasks(), _params(), _self_update(false) 
{
  if(_instance_count == 0){
    _instance_count++;
  } else {
    throw std::runtime_error("Only one instance of Scheduler is allowed");
  }
}

Scheduler::~Scheduler(){
  if(_taskData){
    Lock lock(_taskData->_mutex);
    vTaskDelete(_taskData->_handle);
  }
  _instance_count--;
}

Scheduler& Scheduler::setParams(const TaskParams& params){
  _params = params;
  return *this;
}

Scheduler& Scheduler::addTask(const AsyncTask<>& task, ScheduleParams schedule){
  // Add a task to the list of tasks, user might have called `run` before adding tasks
  // so we must check if the taskData is initialized and use the mutex, same for the
  // other setter methods
  if (_taskData){
    Lock lock(_taskData->_mutex);
    _tasks.emplace_back(task, schedule);
  } else {
    _tasks.emplace_back(task, schedule);
  }
  return *this;
}

Scheduler& Scheduler::addTask(std::function<void(void)> task, const ScheduleParams& schedule){
  if (_taskData){
    Lock lock(_taskData->_mutex);
    _tasks.emplace_back(AsyncTask<>(task), schedule);
  } else {
    _tasks.emplace_back(AsyncTask<>(task), schedule);
  }
  return *this;
}

Scheduler& Scheduler::updateNow(struct tm* now){
  if (_taskData) {
    Lock lock(_taskData->_mutex);
    _now = now;
  } else {
    _now = now;
  }
  return *this;
}

Scheduler& Scheduler::setSelfTimeUpdate(bool self_update){
  if(_taskData){
    Lock lock(_taskData->_mutex);
    _self_update = self_update;
  } else {
    _self_update = self_update;
  }
  return *this;
}

void Scheduler::run(struct tm* now){

  // if the scheduler is already running, return
  if (_taskData != nullptr){
    return;
  }

  if (now == nullptr){
    time_t t = time(nullptr);
    _now = localtime(&t);
    _self_update = true;
  } else {
    _now = now;
  }
  
  _taskData.reset(new _TaskData());
  
  if(_params.usePinnedCore){
    xTaskCreatePinnedToCore(
      _taskRunner, "Scheduler", _params.stackSize, this, tskIDLE_PRIORITY, &_taskData->_handle, _params.core
    );
  } else{
    xTaskCreate(
      _taskRunner, "Scheduler", _params.stackSize, this, tskIDLE_PRIORITY, &_taskData->_handle
    );
  }
}

void Scheduler::stop(){
  if (_taskData == nullptr){
    return;
  }
  Lock lock(_taskData->_mutex);
  vTaskDelete(_taskData->_handle);
  _taskData.reset();
}

void Scheduler::pause(){
  if (_taskData == nullptr || _taskData->_signal != _TaskSignal::RUN){
    return;
  }
  vTaskSuspend(_taskData->_handle);
  _taskData->_signal = _TaskSignal::PAUSE;
}

void Scheduler::resume(){
  if (_taskData == nullptr || _taskData->_signal != _TaskSignal::PAUSE){
    return;
  }
  _taskData->_signal = _TaskSignal::RUN;
  vTaskResume(_taskData->_handle);
}

END_TASKS_NAMESPACE