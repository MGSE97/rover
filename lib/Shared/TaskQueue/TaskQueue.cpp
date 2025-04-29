#include "TaskQueue.h"

TaskQueue::TaskQueue(u8 defs_len, Task defs[]) {
  len = defs_len;
  tasks = defs;
  lastRuns = (time*)malloc(defs_len*sizeof(time));
  for(u8 i = 0; i < defs_len; i++) lastRuns[i] = 0;
}

void TaskQueue::process() {
  time now = micros();

  // Run active tasks
  for(u8 i = 0; i < len; i++) {
    Task* task = &tasks[i];
    if(now - lastRuns[i] >= task->delay) {
      task->run();
      lastRuns[i] = now;
    }
  }
}