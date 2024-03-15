#pragma once


#include "namespaces.h"

BEGIN_TASKS_NAMESPACE

struct ScheduleLike{
  virtual inline struct tm execute(struct tm* now){
    return *now;
  };
};

// ## Simple Schedule
// The simple schedule is a schedule that runs every `n` seconds.
struct SimpleSchedule: public ScheduleLike{
  int seconds;

  SimpleSchedule(int seconds = 60): seconds(seconds) {}

  inline struct tm execute(struct tm* now){
    struct tm next = *now;
    next.tm_sec += seconds;
    mktime(&next);
    return next;
  }
};

END_TASKS_NAMESPACE
