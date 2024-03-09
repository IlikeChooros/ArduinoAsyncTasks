#pragma once

#include <Arduino.h>

#include "namespaces.h"
#include <functional>
#include <tuple>

// apply function to tuple
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


template <typename _Func>
class AsyncTask;


template <typename _Res, typename... _ArgTypes>
static inline void _taskWrapper(void *param){
    // Get the task from the parameter and cast it to the correct type.
    // This is probably the only way to do this, since we can't use lambdas
    // as task functions, and we can't use static_cast with lambdas.
    auto task = static_cast<AsyncTask<_Res(_ArgTypes...)>*>(param);
    
    if (!task){
        return;
    }
    // Run the task and the callback
    task->_runTasks();
    
    // Delete the task f
    if (task->_handle != nullptr){
        vTaskDelete(task->_handle);
    }
}

/**
 * Base class for AsyncTask, used to store the task handle
*/
class BaseAsyncTask{
  public:
    BaseAsyncTask(): _handle(NULL) {}
    virtual ~BaseAsyncTask() = default;

    TaskHandle_t _handle;    
};

/**
  * AsyncTask class, used to run tasks asynchronously
  * @tparam _Res Return type of the task function
  * @tparam _ArgTypes Argument types of the task function
*/
template <typename _Res, typename... _ArgTypes>
class AsyncTask<_Res(_ArgTypes...)> : public BaseAsyncTask{

    using _TaskType = std::function<_Res(_ArgTypes...)>;
    using _CallbackType = std::function<void(_Res)>;

    TaskParams _params;
    _TaskType _task;
    _CallbackType _callback;
    std::tuple<_ArgTypes...> _args;

    public:
    AsyncTask() = default;
    AsyncTask(_TaskType task, _CallbackType callback = nullptr);
    AsyncTask(const TaskParams& params);
    AsyncTask(
      const TaskParams& params, 
      _TaskType task, 
      _CallbackType callback = nullptr
    );

    /**
     * @brief Set the task parameters
    */
    inline AsyncTask& setParams(const TaskParams& params){
        _params = params;
        return *this;
    }

    /**
     * @brief Set the task function
    */
    inline AsyncTask& setTask(_TaskType task){
        _task = task;
        return *this;
    }

    /**
     * @brief Set the callback function, called after the task function
     * @param callback Callback function, takes the return value of the task function
    */
    inline AsyncTask& then(_CallbackType callback){
        _callback = callback;
        return *this;
    }

    /**
     * @brief Run the task asynchronously
     * @param args Arguments for the task function
    */
    inline void run(_ArgTypes... args){
      _args = std::make_tuple(args...);
      if (_task){
          if(_params.usePinnedCore){
            xTaskCreatePinnedToCore(
                _taskWrapper<_Res, _ArgTypes...>, _params.name.c_str(), _params.stackSize,
                this, _params.priority, &_handle, _params.core
            );
            return;
          }
          xTaskCreate(
              _taskWrapper<_Res, _ArgTypes...>, _params.name.c_str(), _params.stackSize,
              this, _params.priority, &_handle
          );
      }
    }

    /**
     * @brief Run the task asynchronously, same as `run(...)`
    */
    inline void operator()(_ArgTypes... args){
        run(args...);
    }

    /**
     * @brief Run the task and the callback, used internally
    */
    inline void _runTasks(){
      if (_task){
          _Res res = apply(_task, _args);
          if (_callback){
            _callback(res);
          }
      }
    }
};


template <typename _Res, typename... _ArgTypes>
AsyncTask<_Res(_ArgTypes...)>::AsyncTask(_TaskType task, _CallbackType callback): 
    AsyncTask(TaskParams(), task, callback) {}

template <typename _Res, typename... _ArgTypes>
AsyncTask<_Res(_ArgTypes...)>::AsyncTask(const TaskParams& params) : 
    AsyncTask(params, nullptr, nullptr) {}

template <typename _Res, typename... _ArgTypes>
AsyncTask<_Res(_ArgTypes...)>::AsyncTask(
  const TaskParams& params, 
  _TaskType task, 
  _CallbackType callback
) : _params(params), _task(task), _callback(callback) {}


// specialization for void return type, since we can't call the callback with `void`
template <typename... _ArgTypes>
class AsyncTask<void(_ArgTypes...)> : public BaseAsyncTask{

    using _TaskType = std::function<void(_ArgTypes...)>;

    TaskParams _params;
    _TaskType _task;
    std::function<void()> _callback;
    std::tuple<_ArgTypes...> _args;


    public:
    AsyncTask() = default;
    AsyncTask(_TaskType task, std::function<void()> callback = nullptr);
    AsyncTask(const TaskParams& params);
    AsyncTask(
      const TaskParams& params, 
      _TaskType task, 
      std::function<void()> callback = nullptr
    );

    inline AsyncTask& setParams(const TaskParams& params){
        _params = params;
        return *this;
    }

    inline AsyncTask& setTask(_TaskType task){
        _task = task;
        return *this;
    }

    inline AsyncTask& then(std::function<void()> callback){
        _callback = callback;
        return *this;
    }

    inline void run(_ArgTypes... args){
      _args = std::make_tuple(args...);
      if (_task){
        if (_params.usePinnedCore){
            xTaskCreatePinnedToCore(
                _taskWrapper<void, _ArgTypes...>, _params.name.c_str(), _params.stackSize,
                this, _params.priority, &_handle, _params.core
            );
            return;
        }
        xTaskCreate(
            _taskWrapper<void, _ArgTypes...>, _params.name.c_str(), _params.stackSize,
            this, _params.priority, &_handle
        );
      }
    }

    inline void operator()(_ArgTypes... args){
        run(args...);
    }

    inline void _runTasks(){
      if (_task){
          apply(_task, _args);
          if (_callback){
            _callback();
          }
      }
    }
};

template <typename... _ArgTypes>
AsyncTask<void(_ArgTypes...)>::AsyncTask(_TaskType task, std::function<void()> callback): 
    AsyncTask(TaskParams(), task, callback) {}

template <typename... _ArgTypes>
AsyncTask<void(_ArgTypes...)>::AsyncTask(const TaskParams& params) : 
    AsyncTask(params, nullptr, nullptr) {}

template <typename... _ArgTypes>
AsyncTask<void(_ArgTypes...)>::AsyncTask(
  const TaskParams& params, 
  _TaskType task,
  std::function<void()> callback
) : _params(params), _task(task), _callback(callback) {}


// specialization for _Res return type and no arguments
template <typename _Res>
class AsyncTask<_Res()> : public BaseAsyncTask{

    using _TaskType = std::function<_Res()>;
    using _CallbackType = std::function<void(_Res)>;

    TaskParams _params;
    _TaskType _task;
    _CallbackType _callback;

    public:
    AsyncTask() = default;
    AsyncTask(_TaskType task, _CallbackType callback = nullptr);
    AsyncTask(const TaskParams& params);
    AsyncTask(const TaskParams& params, _TaskType task, _CallbackType callback = nullptr);

    AsyncTask& setParams(const TaskParams& params){
        _params = params;
        return *this;
    }

    AsyncTask& setTask(_TaskType task){
        _task = task;
        return *this;
    }

    AsyncTask& then(_CallbackType callback){
        _callback = callback;
        return *this;
    }

    void run(){
        if (_task){
            if(_params.usePinnedCore){
                xTaskCreatePinnedToCore(
                    _taskWrapper<_Res>, _params.name.c_str(), _params.stackSize,
                    this, _params.priority, &_handle, _params.core
                );
                return;
            }
            xTaskCreate(
                _taskWrapper<_Res>, _params.name.c_str(), _params.stackSize,
                this, _params.priority, &_handle
            );
        }
    }

    void _runTasks(){
        if (_task){
            _Res res = _task();
            if (_callback){
                _callback(res);
            }
        }
    }

    void operator()(){
        run();
    }
};

template <typename _Res>
AsyncTask<_Res()>::AsyncTask(_TaskType task, _CallbackType callback): 
    AsyncTask(TaskParams(), task, callback) {}

template <typename _Res>
AsyncTask<_Res()>::AsyncTask(const TaskParams& params) : 
    AsyncTask(params, nullptr, nullptr) {}

template <typename _Res>
AsyncTask<_Res()>::AsyncTask(
  const TaskParams& params, 
  _TaskType task, 
  _CallbackType callback
) : _params(params), _task(task), _callback(callback) {}


// specialization for void return type and no arguments
template <>
class AsyncTask<void()> : public BaseAsyncTask{

    using _TaskType = std::function<void()>;

    TaskParams _params;
    _TaskType _task;
    _TaskType _callback;

    public:
    AsyncTask() = default;
    AsyncTask(_TaskType task, _TaskType callback = nullptr);
    AsyncTask(const TaskParams& params);
    AsyncTask(const TaskParams& params, _TaskType task, _TaskType callback = nullptr);

    AsyncTask& setParams(const TaskParams& params);
    AsyncTask& setTask(_TaskType task);
    AsyncTask& then(_TaskType callback);

    void run();
    void _runTasks();

    void operator()();
};


END_TASKS_NAMESPACE