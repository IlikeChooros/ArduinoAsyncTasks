#pragma once

#include <Arduino.h>

#include "namespaces.h"
#include <functional>
#include <tuple>

// `apply` implementation for tuples
#include "tuple.h"

BEGIN_TASKS_NAMESPACE

/**
  * Task parameters, used to create a task, has information about the task:
  * - stack size (default is 4096)
  * - priority (default is tskIDLE_PRIORITY)
  * - name (default is "Task")
  * - use pinned core (default is false)
  * - core (default is 0)
*/
struct TaskParams{
    // stack size, default is 4096
    int stackSize = 4096;

    // priority of the task, default is tskIDLE_PRIORITY, probably shouldn't be changed
    int priority = tskIDLE_PRIORITY;

    // name of the task, default is "Task"
    std::string name = "Task";

    // use pinned core, default is false
    bool usePinnedCore = false;

    // core to pin the task to (0 or 1), default is 0 (if usePinnedCore is true)
    int core = 0;
    
    TaskParams(
        int stackSize = 4096, 
        int priority = tskIDLE_PRIORITY, 
        std::string name = "Task", 
        bool usePinnedCore = false, 
        int core = 0
    ): stackSize(stackSize), priority(priority), name(name),
    usePinnedCore(usePinnedCore), core(core) {}
    
};

/**
 * Task data, used to store the task handle.
 * 
 * To pass the `TaskHandle_t` to the task function, we need to store it in the heap.
 * This is a simple struct that stores the handle, and is used to delete the task
 * after it's done.
*/
typedef struct _TaskData{
    TaskHandle_t _handle;

    _TaskData(TaskHandle_t handle = NULL):
        _handle(handle) {}
};


template <typename... _ArgTypes>
class AsyncTask;

/**
 * @brief Delete the task and free the memory
*/
template <typename... _ArgTypes>
void _deleteTask(AsyncTask<_ArgTypes...>* p){
    TaskHandle_t handle = NULL;

    if (p->_data && p->_data->_handle != NULL){
        handle = p->_data->_handle;
    }

    delete p->_data;
    delete p;

    if (handle){
        vTaskDelete(handle);
    }
}

/**
 * Base class for AsyncTask, used to store parameters and the task data
*/
class BaseAsyncTask{
  public:
    BaseAsyncTask(): _data(nullptr), _params() {}
    BaseAsyncTask(const TaskParams& params): _data(nullptr), _params(params) {}
    virtual ~BaseAsyncTask() = default;

  protected:
    /**
     * @brief Wrapper for the task function, casts the task, runs it and deletes it
    */
    template <typename _Res, typename... _ArgTypes>
    static void _taskWrapper(void *param){
        // Get the task from the parameter and cast it to the correct type.
        // This is probably the only way to do this, since we can't use lambdas
        // as task functions, and we can't use static_cast with lambdas.
        // The task also must be allocated on the heap, since there's a possibility
        // that the task will be deleted before it's done if it was on stack memory.
        auto task = static_cast<AsyncTask<_ArgTypes...>*>(param);
        
        if (!task){
            return;
        }

        // Run the task on the current thread
        task->_runTask();
        
        // Delete the task after it's done, must be called after the task is done,
        // also terminates the FreeRTOS task
        _deleteTask<_ArgTypes...>(task);
    }

    template <typename... _ArgTypes>
    friend void _deleteTask(AsyncTask<_ArgTypes...>* p);

    _TaskData* _data;
    TaskParams _params;
};


/**
 * ## AsyncTask
 * 
 * A class that represents a task that can be run in the background.
 * 
*/
template <typename... _ArgTypes>
class AsyncTask : public BaseAsyncTask{

    using _TaskType = std::function<void(_ArgTypes...)>;

    _TaskType _task;
    std::tuple<_ArgTypes...> _args;

public:
    AsyncTask() = default;
    AsyncTask(_TaskType task);
    AsyncTask(const TaskParams& params);
    AsyncTask(
      const TaskParams& params, 
      _TaskType task
    );
    AsyncTask(const AsyncTask& other);

    inline AsyncTask& setParams(const TaskParams& params){
        _params = params;
        return *this;
    }

    inline AsyncTask& setTask(_TaskType task){
        _task = task;
        return *this;
    }

    /**
     * @brief Run the task in the background
    */
    inline void run(_ArgTypes... args){
      _args = std::make_tuple(args...);
      if (_task){
            _data = new _TaskData();
            if (_params.usePinnedCore){
                xTaskCreatePinnedToCore(
                    _taskWrapper<void, _ArgTypes...>, _params.name.c_str(), _params.stackSize,
                    copy(), _params.priority, &_data->_handle, _params.core
                );
                return;
            }
            xTaskCreate(
                _taskWrapper<void, _ArgTypes...>, _params.name.c_str(), _params.stackSize,
                copy(), _params.priority, &_data->_handle
            );
      }
    }

    /**
     * @brief Same as `run(...)`, but with operator overloading
    */
    inline void operator()(_ArgTypes... args){
        run(args...);
    }

    /**
     * @brief Run the task in the current thread, used internally
    */
    inline void _runTask(){
      if (_task){
          apply(_task, _args);
      }
    }

    AsyncTask& operator=(const AsyncTask& other){
        _params = other._params;
        _task = other._task;
        _args = other._args;
        _data = other._data;
        return *this;
    }

    /**
     * @brief Copy the task to the heap, used internally
    */
    AsyncTask* copy() const{
        return new AsyncTask(*this);
    }
};

template <typename... _ArgTypes>
AsyncTask<_ArgTypes...>::AsyncTask(_TaskType task): 
    AsyncTask(TaskParams(), task) {}

template <typename... _ArgTypes>
AsyncTask<_ArgTypes...>::AsyncTask(const TaskParams& params) : 
    AsyncTask(params, nullptr) {}

template <typename... _ArgTypes>
AsyncTask<_ArgTypes...>::AsyncTask(
  const TaskParams& params, 
  _TaskType task
) : BaseAsyncTask(params), _task(task) {}

template <typename... _ArgTypes>
AsyncTask<_ArgTypes...>::AsyncTask(const AsyncTask& other){
    *this = other;
}

/**
 * ## AsyncTask
 * 
 * A class that represents a task that can be run in the background.
 * 
 * This is a specialization of the AsyncTask class, used when the task
 * doesn't take any arguments.
*/
template <>
class AsyncTask<> : public BaseAsyncTask{

    using _TaskType = std::function<void()>;

    _TaskType _task;

public:
    AsyncTask();
    AsyncTask(_TaskType task);
    AsyncTask(const TaskParams& params);
    AsyncTask(const TaskParams& params, _TaskType task);
    AsyncTask(const AsyncTask& other);

    AsyncTask& setParams(const TaskParams& params);
    AsyncTask& setTask(_TaskType task);

    /**
     * @brief Run the task in the background
    */
    void run();

    /**
     * @brief Same as `run()`, but with operator overloading
    */
    void operator()();

    /**
     * @brief Run the task in the current thread, used internally
    */
    void _runTask();    

    AsyncTask& operator=(const AsyncTask& other);

    /**
     * @brief Copy the task to the heap, used internally
    */
    AsyncTask* copy() const;
};


END_TASKS_NAMESPACE