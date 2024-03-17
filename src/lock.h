#pragma once

#include <Arduino.h>

#include "namespaces.h"

BEGIN_TASKS_NAMESPACE

/**
 * Lock class, used to lock a semaphore, and unlock it when the object is destroyed
 */
class Lock{
  bool _locked;
  SemaphoreHandle_t _semaphore;
public:
  Lock(): _locked(false) {}
  
  Lock(SemaphoreHandle_t semaphore): 
    _locked(false), _semaphore(semaphore) {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    _locked = true;
  }

  ~Lock(){
    if(_locked){
      xSemaphoreGive(_semaphore);
    }
  }
};


END_TASKS_NAMESPACE