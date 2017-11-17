# Design summary

This document contains an extremely brief summary of the design of the `IRThreadPool` and related classes, which provide a simple thread pool implementation for asynchronously executing tasks in parallel. 

There were two goals that led to the peculiarities of the current design:
- Efficiently schedule many tasks that use the same function on different arguments (to make the common case of speeding up emarassingly-parallel problems --- particularly parallel-for --- efficient)
- Allow the thread pool to be used without ever needing to allocate memory on the heap. This is only really possible if the size of the task arrays in known at compile-time. Fortunately for us, we often can assume we do know how many tasks we want at compile time.

## Classes

The thread pool class cluster has the following classes:


- `IRTask`: An abstract base class for tasks that can be scheduled to run asynchronously.  
   Methods:
   - `Wait`
   - `GetReturnValue`     
- `IRThreadPoolTask`: A concrete subclass of `IRTask` that runs on the thread pool.  
- `IRTaskArray`: An abstract base class for a set of tasks that share the same function but have different arguments, and can all be scheduled at the same time.  
  Methods:
  - `WaitAll`
  - `GetTask`
- `IRThreadPoolTaskArray`: A concrete subclass of `IRTaskArray` whose tasks run on the thread pool
- `IRThreadPoolTaskQueue`: A data structure used to keep track of the scheduled, running, and finished tasks.  
  Methods:
  - `StartTasks`
  - `PopNextTask`
  - `WaitAll`
- `IRThreadPool`: A global set of threads that are allocated and spun up in the beginning of execution, and run the tasks scheduled by the user.  
  Methods:
  - `Initialize` 
  - `StartTasks`
  - `ShutDown`

## Client API summary 

Client code never needs to directly interact with the thread pool itself. Tasks are scheduled via a `StartTasks` method in `IRFunctionEmitter`, which returns an `IRTaskArray` object. The `IRTaskArray` interface is very limited: you can wait for the tasks in the task array to finish, and an individual task (an `IRTask`) from the array. `IRTask` is similarly limited: you can wait for it to finish, and, once finished, get its return value.

### Usage
To use the new API, you need to first define a function for your tasks:

```
auto taskFunction = module.BeginFunction("taskFunction", VariableType::Int32 {VariableType::Int32, VariableType::Int32});
{
    auto start = taskFunction.GetArgument(0);
    auto end = taskFunction.GetArgument(1);
    // Do something
    taskFunction.Return(result);
}
taskFunction.End();
```

Now many instances of the task function can be run with different arguments:

```
auto task1Args = std::vector<llvm::Value*> {start1, end1};
auto task2Args = std::vector<llvm::Value*> {start2, end2};
...
auto tasks = function.StartTasks(function, {{task1Args}, {task2Args}, {task3Args}, ...});

tasks.WaitAll(); // block until all tasks are done
```

## Limitations

The most significant limitation of the current design and implementation is that the array of tasks is allocated on the stack of the function that submits the tasks to the thread pool. This implies that all the tasks must finish before the function returns. This limits the space of things that these tasks can do: for instance, there's no way to enqueue tasks in one node and then wait for them to finish in another.

The other limitation is that once an array of tasks is scheduled, you can't submit another set of tasks until the first is done. It will be relatively simple to remove this limitation in the future.

