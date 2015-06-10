#include "threadpool.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace cpputil {

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
    //fprintf(stderr, "broadcasting all waiting threads\n");
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
  //fprintf(stderr, "runing thread\n");
  while (true) {
    mu_->Lock();
    while (tasks_.empty()) {
      if (joining) {
        mu_->Unlock();
        //fprintf(stderr, "joining thread\n");
        return;
      }
      notEmptyCond_->Wait();
      //fprintf(stderr, "wait for task\n");
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

  if (waitTasks) {
    while (!tasks_.empty()) {
      usleep(0.1);
    }
  }
  mu_->Lock();
  joining = true;
  mu_->Unlock();
  // wake up all waiting threads 
  fprintf(stderr, "broadcasting all threads to join\n");
  notEmptyCond_->SignalAll();
  int status;
  for (int i = 0; i < threads_.size(); i++) {
     status = pthread_join(threads_[i], NULL);
     if (status != 0) {
       fprintf(stderr, "fail to join thread %s\n", strerror(status));
     }
  }
  fprintf(stderr, "joined threads");
}
} // namespace cpputil
