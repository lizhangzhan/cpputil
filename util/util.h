#ifndef SIMHASH_UTIL_H_
#define SIMHASH_UTIL_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>

namespace simhash {
namespace util {

class CondVar;

class Mutex {
 public:
  Mutex() {
  pthread_mutex_init(&mu_, NULL);
  }

  ~Mutex() {
  pthread_mutex_destroy(&mu_);
  }

  void Lock() {
    pthread_mutex_lock(&mu_);
  }
  void Unlock() {
    pthread_mutex_unlock(&mu_);
  }
 
private:
  friend class CondVar;
  pthread_mutex_t mu_;
  
  // No copying
  Mutex(const  Mutex&);
  void operator=(const Mutex&);
}; // class Mutex

class CondVar {
 public:
  explicit CondVar(Mutex* mu) : mu_(mu) {
    pthread_cond_init(&cv_, NULL);
  }
  ~CondVar() {
    pthread_cond_destroy(&cv_);
  }
  void Wait() {
    pthread_cond_wait(&cv_, &(mu_->mu_));
  }
  void Signal() {
    pthread_cond_signal(&cv_);
  }
  void SignalAll() {
    pthread_cond_broadcast(&cv_);
  }
 private:
  pthread_cond_t cv_;
  Mutex* mu_;

  // No copying
  CondVar(const CondVar&);
  void operator=(const CondVar&);
}; // class CondVar

inline std::string& ltrim(std::string & s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

inline std::string& rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

inline std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

inline bool split(const std::string& s, std::vector<std::string>& ret, char delim) {
  std::stringstream ss(s);
  std::string term;
  while(getline(ss, term, delim)) {
    if (term.size() > 0) {
      ret.push_back(term);
    }
  }
  return true;
}

inline std::string GenString(uint64_t val) {
  std::ostringstream tmp;
  tmp << std::hex << val << std::dec; // convert hex str
  return tmp.str();
}

inline int IsDir(const char *path) {
  struct stat st;
  stat(path, &st);
  return S_ISDIR(st.st_mode) ? 0 : 1;
}

inline uint64_t ConvertToUint64(const char* cstr) {
  char* end;
  return strtoull(cstr, &end, 16);
}
} // namespace util
}// namespace simhash
#endif // SIMHASH_UTIL_H_
