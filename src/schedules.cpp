#include "schedules.h"

BEGIN_TASKS_NAMESPACE

struct tm simpleExecutor(struct tm* now){
  struct tm next = *now;
  next.tm_sec += 60;
  mktime(&next);
  return next;
}

executorFunc ScheduleParams::getExecutor(){
  return simpleExecutor;
}

ScheduleParams& ScheduleParams::every(int amount, TimeUnit unit){
  this->amount = amount;
  this->unit = unit;
  return *this;
}

tm ScheduleParams::schedule(struct tm* now){
  struct tm next = *now;
  updateTime(&next, amount, unit);
  return next;
}

void updateTime(struct tm* now, int amount, TimeUnit unit){
  int *ptr = nullptr;
  switch(unit){
    case TimeUnit::Seconds:
      ptr = &now->tm_sec;
      break;
    case TimeUnit::Minutes:
      ptr = &now->tm_min;
      break;
    case TimeUnit::Hours:
      ptr = &now->tm_hour;
      break;
    case TimeUnit::Days:
      ptr = &now->tm_mday;
      break;
    default:
      break;
  }
  if (ptr){
    *ptr += amount;
    mktime(now);
  }
}

END_TASKS_NAMESPACE
