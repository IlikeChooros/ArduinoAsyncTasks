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


/**
 * TryLock class, used to try to lock a semaphore, and unlock it when the object is destroyed
 */
class TryLock{
  bool _locked;
  SemaphoreHandle_t _semaphore;
public:
  TryLock(): _locked(false) {}
  
  /**
   * Try to lock the semaphore, if it's not possible, the object will be created
   * with the `locked` attribute set to false
   * 
   * @param semaphore the semaphore to lock
   * @param timeout the timeout to wait for the semaphore in milliseconds
   */
  TryLock(SemaphoreHandle_t semaphore, int timeout = 1000): 
    _locked(false), _semaphore(semaphore) {
    _locked = xSemaphoreTake(semaphore, timeout * portTICK_PERIOD_MS) == pdTRUE;
  }

  ~TryLock(){
    if(_locked){
      xSemaphoreGive(_semaphore);
    }
  }

  /**
   * Check if the semaphore is locked
   * 
   * @return true if the semaphore is locked, false otherwise
   */
  bool locked() const {
    return _locked;
  }
};


END_TASKS_NAMESPACE