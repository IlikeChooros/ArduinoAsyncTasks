#include "AsyncTask.h"

BEGIN_TASKS_NAMESPACE

// void AsyncTask specializations with no parameters

using _TaskType = std::function<void()>;

AsyncTask<>::AsyncTask():
    AsyncTask(TaskParams(), nullptr) {}

AsyncTask<>::AsyncTask(_TaskType task):
    AsyncTask(TaskParams(), task) {}

AsyncTask<>::AsyncTask(const TaskParams& params):
    AsyncTask(params, nullptr) {}

AsyncTask<>::AsyncTask(const TaskParams& params, _TaskType task):
    BaseAsyncTask(params), _task(task) {}

AsyncTask<>::AsyncTask(const AsyncTask& other){
    *this = other;
}

AsyncTask<>& AsyncTask<>::setParams(const TaskParams& params){
    _params = params;
    return *this;
}

AsyncTask<>& AsyncTask<>::setTask(_TaskType task){
    _task = task;
    return *this;
}


void AsyncTask<>::run(){
    if (_task){
        _data = new _TaskData();
        if(_params.usePinnedCore){
            xTaskCreatePinnedToCore(
                _taskWrapper<void>, _params.name.c_str(), _params.stackSize,
                copy(), _params.priority, &_data->_handle, _params.core
            );
            return;
        }
        xTaskCreate(
            _taskWrapper<void>, _params.name.c_str(), _params.stackSize,
            copy(), _params.priority, &_data->_handle
        );
    }
}

void AsyncTask<>::operator()(){
    run();
}

void AsyncTask<>::_runTask(){
    if(_task){
        _task();
    }
}

AsyncTask<>& AsyncTask<>::operator=(const AsyncTask<>& other){
    _params = other._params;
    _task = other._task;
    _data = other._data;
    return *this;
}

AsyncTask<>* AsyncTask<>::copy() const{
    return new AsyncTask<>(*this);
}


END_TASKS_NAMESPACE

