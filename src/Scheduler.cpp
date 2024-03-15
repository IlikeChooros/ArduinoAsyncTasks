#include "Scheduler.h"

BEGIN_TASKS_NAMESPACE

void Scheduler::_executeTask(struct _ScheduledTask& task, struct tm* now){
  if(difftime(mktime(now), mktime(&task.lastExecution) >= 0)){
    task.task.run();
    task.lastExecution = *now;
  }
}

Scheduler::Scheduler(){}

END_TASKS_NAMESPACE