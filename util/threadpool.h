#ifndef CPPUTIL_THREADPOOL_H_
#define CPPUTIL_THREADPOOL_H_
#include <deque>
#include <vector>

#include "util.h"

namespace cpputil {

typedef struct Task Task;

class ThreadPool {
 public:
  ThreadPool(size_t threadNum, size_t queueSize) : poolThreadNum_(threadNum), maxQueueSize_(queueSize) {
    mu_ = new util::Mutex();
    notEmptyCond_ = new util::CondVar(mu_);
    notFullCond_ = new util::CondVar(mu_);
    joining = false;
  }
  ~ThreadPool();
  bool AddTask(void (*func)(void*), void* arg, size_t timeout=0);
  void RunThread();
  bool JoinThreads(bool waitTasks=false);

 private:
  std::deque<Task*> tasks_;
  std::vector<pthread_t> threads_;
  util::Mutex* mu_;
  util::CondVar* notEmptyCond_;
  util::CondVar* notFullCond_;
  size_t poolThreadNum_;
  size_t maxQueueSize_;
  bool joining;

  // no copying
  ThreadPool(const ThreadPool&);
  void operator=(const ThreadPool&);
}; // class ThreadPool

}// namespace cpputil
#endif // CPPUTIL_THREADPOOL_H_
