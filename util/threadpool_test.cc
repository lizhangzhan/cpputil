#include "threadpool.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void HelloEcho(void* arg) {
  int tid = *((int *)arg);
  printf("hello thread %d\n", tid);
  delete (int *)arg;
}

int main(int argc, char* argv[]) {
  cpputil::ThreadPool pool(2, 10);

  for (int i = 0; i < 20; i++) {
    int* arg = new int(i);
    pool.AddTask(&HelloEcho, (void*)arg); 
  }
  //pool.JoinThreads();  
  return 0;
}
