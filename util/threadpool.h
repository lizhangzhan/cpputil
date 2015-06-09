#ifndef SIMHASH_THREADPOOL_H_
#define SIMHASH_THREADPOOL_H_
#include <deque>
#include <vector>

#include "util.h"
namespace simhash {

typedef struct task_t {
  void (*func)(void*);
  void* arg;
} Task;

enum ErrorType {
  kFullQueueType = 1;
};

class ThreadPool {
 public:
  ThreadPool(size_t thread_num, size_t queue_size) : thread_num_(thread_num), queue_size_(queue_size) {
    mu_ = util::Mutex();
    cond_ = util::CondVar(&mu_);
    waitTaskNum_ = 0;
    joining = false;
  }
  ~ThreadPool();
  AddTask(void (*func)(void*), void* arg, size_t timeout=5);
  JoinThreads(bool waitTasks=true, bool waitThreads=true);
  RunThread();

 private:
  std::deque<Task> tasks_;
  std::vector<pthread_t> threads_;
  util::Mutex* mu_;
  util::CondVar* cond_;
  size_t thread_num_;
  size_t queue_size_;
  size_t avail_thread_num_;
  bool joining;

  // no copying
  ThreadPool(const ThreadPool&);
  void operator=(const ThreadPool&);
}
}// namespace simhash 

#endif // SIMHASH_THREADPOOL_H
