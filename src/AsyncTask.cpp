#include "AsyncTask.h"

BEGIN_TASKS_NAMESPACE

// void AsyncTask specializations with no parameters

AsyncTask<void()>::AsyncTask(std::function<void()> task, std::function<void()> callback):
    AsyncTask(TaskParams(), task, callback) {}

AsyncTask<void()>::AsyncTask(const TaskParams& params):
    AsyncTask(params, nullptr, nullptr) {}

AsyncTask<void()>::AsyncTask(const TaskParams& params, std::function<void()> task, std::function<void()> callback):
    _params(params), _task(task), _callback(callback) {}

AsyncTask<void()>& AsyncTask<void()>::setParams(const TaskParams& params){
    _params = params;
    return *this;
}

AsyncTask<void()>& AsyncTask<void()>::setTask(std::function<void()> task){
    _task = task;
    return *this;
}

AsyncTask<void()>& AsyncTask<void()>::then(std::function<void()> callback){
    _callback = callback;
    return *this;
}

void AsyncTask<void()>::run(){
    if (_task){
        _task();
        if (_callback){
            _callback();
        }
    }   
}

void AsyncTask<void()>::operator()(){
    run();
}

void AsyncTask<void()>::_runTasks(){
    if (_task){
        if (_params.usePinnedCore){
            xTaskCreatePinnedToCore(
                _taskWrapper<void>, _params.name.c_str(), _params.stackSize,
                this, _params.priority, &_handle, _params.core
            );
            return;
        }
        xTaskCreate(
            _taskWrapper<void>, _params.name.c_str(), _params.stackSize,
            this, _params.priority, &_handle
        );
    }
}


END_TASKS_NAMESPACE

