#include "threadpool.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace cpputil {
struct Task {
  void (*func)(void*);
  void* arg;
};

static void* RunThreadWrapper(void *arg);

ThreadPool::~ThreadPool() {
  JoinThreads();
  delete notEmptyCond_;
  delete notFullCond_;
  delete mu_;
}

bool ThreadPool::AddTask(void (*func)(void*), void* arg, size_t timeout) {
  fprintf(stderr, "task num=%lu\n", tasks_.size());
  mu_->Lock();
  // lazy create thread
  if (tasks_.size() >= threads_.size() && threads_.size() < poolThreadNum_) {
    pthread_t tid;
    pthread_create(&tid, NULL, &RunThreadWrapper, this);
    threads_.push_back(tid);
    fprintf(stderr, "create new thread, %lu active thread num\n", threads_.size());
  }
  while (tasks_.size() == maxQueueSize_) {
    notFullCond_->Wait();
  }
  if (tasks_.empty()) {
    notEmptyCond_->Signal();
  }
  Task* t = new Task;
  t->func = func;
  t->arg = arg;
  tasks_.push_back(t);
  mu_->Unlock();
  return true;
}

static void* RunThreadWrapper(void *arg) {
  reinterpret_cast<ThreadPool*>(arg)->RunThread();
  return NULL;
}

void ThreadPool::RunThread() {
  // backgroud thread, keep running
  while (true) {
    mu_->Lock();
    while (tasks_.empty()) {
      if (joining) {
        mu_->Unlock();
        pthread_exit(NULL);
      }
      notEmptyCond_->Wait();
    }
    if (tasks_.size() == maxQueueSize_) {
      notFullCond_->Signal();
    }
    Task* task = tasks_.front();
    void (*function)(void*) = task->func;
    void* arg = task->arg;
    tasks_.pop_front();
    mu_->Unlock();
    (*function)(arg);
    delete  task;
  }
}

bool ThreadPool::JoinThreads(bool waitTasks) {
  if (joining) {
    return false;
  }

  mu_->Lock();
  joining = true;
  mu_->Unlock();
  // wake up all waiting threads 
  notEmptyCond_->SignalAll();
  int status;
  for (int i = 0; i < threads_.size(); i++) {
     status = pthread_join(threads_[i], NULL);
     if (status != 0) {
       fprintf(stderr, "fail to join thread %s\n", strerror(status));
     }
  }
}
} // namespace cpputil
