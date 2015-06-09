#include "threadpool.h"
#include <uistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace simhash {
namespaces {

ThreadPool::~ThreadPool() {
  joining = true;
  JoinThreads();
  delete cond_;
  delete mu_;
}

ThreadPool::AddTask(void (*func)(void*), void* arg, size_t timeout = 5) {
  if (wait_task_num_ == queue_size) {
    size_t remaining = timeout;
    while (remaining > 0) {
      usleep(1);
      if (wait_task_num_ < queue_size)
        break;
      remaining -= 1;
    }
    if (remaining == 0 && wait_task_num_ == queue_size) {
      return kFullQueueType;
    }
  }
  Task t;
  t.func = func;
  t.arg = arg;
  mu_->Lock();
  tasks_.push_back(t);
  wait_task_num_ += 1;
  mu_->Unlock();
}

ThreadPool::RunThread() {
  while (true) {

  }
}
} // anonymous namespace
} // namespace simhas
