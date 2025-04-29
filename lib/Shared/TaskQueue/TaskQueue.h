#ifndef TASK_QUEUE
#define TASK_QUEUE

#include "../Utils/Utils.h"

struct Task {
  time delay;
  void (*run)();
};

class TaskQueue {
  public:
    TaskQueue(u8 len, Task tasks[]);

    void process();
    
  private:
    time* lastRuns;
    Task* tasks;
    u8 len;
};


#endif