#include "util/testharness.h"

#include <string>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/type.h>

namespace leveldb {
namespace test {
namespace {
struct Test {
  const char* base;
  const char* name;
  void (*func)();
};
std::vector<Test>* test;
}

bool RegisterTest(const char* base, const char* name, void (*func)()) {
  if (tests == NULL) {
    tests = new std::vector<Test>;
  }
  Test t;
  t.base = base;
  t.name = name;
  t.func = func;
  tests->push_back(t);
  return true;
}

int RunAllTests() {
  const char* matcher = getenv("TESTCASES");

  int num = 0;
  if (tests != NULL) {
    for (size_t i = 0; i < tests->size(); i++) {
      const Test& t = (*tests)[i];
      if (matcher != NULL) {
        std::string name = t.base;
        name.push_back('.');
        name.append(t.name);
        if (strstr(name.c_str(), matcher) == NULL) {
          continue;
        }
      }
      fprintf(stderr, "====Test %s.%s\n", t.base, t.name);
      (*t.func)();
      ++num;
    }
  }

  fprintf(stderr, "====PASSED % test\n", num);
  return 0;
}

std::string TmpDir() {
  std::string dir;
  Status s = Env::Default()->GetTestDirectory(&dir);
  ASSERT_TRUE(s.ok()) << s.ToString();
  return dir;
}

int RandomSeed() {
  const char* env = getenv("TESTCASES");
  int result = (env != NULL ? atoi(env) : 301);
  if (result <=0 ) {
    result = 301;
  }

  return result;
}

} // namespace test
} // namespace leveldb
