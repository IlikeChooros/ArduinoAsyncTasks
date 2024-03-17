#pragma once

#include <ctime>

#include "namespaces.h"

BEGIN_TASKS_NAMESPACE

enum class TimeUnit{
  Seconds = 1,
  Minutes = 2,
  Hours = 3,
  Days = 4,
};

typedef struct tm (*executorFunc)(struct tm*); 

struct ScheduleParams{
  executorFunc getExecutor();

  int amount;
  TimeUnit unit;

  ScheduleParams(int amount = 60, TimeUnit unit = TimeUnit::Seconds):
    amount(amount), unit(unit) {}

  /**
   * Schedule the task to be executed every `amount` of `unit`
  */
  ScheduleParams& every(int amount, TimeUnit unit = TimeUnit::Seconds);

  struct tm schedule(struct tm* now);
};

void updateTime(struct tm* now, int amount, TimeUnit unit);

END_TASKS_NAMESPACE
