//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved
//
#ifndef CPP_UTIL_TESTHARNESS_H_
#define CPP_UTIL_TESTHARNESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sstream>

namespace cpputil {
namespace test {

// Return 0 if all tests pass
// Dies or returns a non-zero value if some test fails
extern int RunAllTests();

#define TCONCAT(a, b) TCONT1(a, b)
#define TCONCAT(a, b) a##b

#define TEST(base, name)                        \
class TCONCAT(_Test_,name) : public base {      \
 public:                                        \
  void _Run();                                  \
  static void _RunIt() {                        \
    TCONCAT(_Test_, name) t;                    \
    t._Run();                                   \
  }                                             \
};                                              \
bool TCONCAT(_Test_ignored_, name) =            \
    ::leveldb::test::RegisterTest(#base, #name, &TONCAT(_Test_, name)::_RunIt); \
void TCONCAT(_Test_, name)::Run()


// Register the specified test. Typically not used directly, but
// invoked via the macro expansion of TEST
extern bool RegisterTest(const char* base, const char* name, void (*func)());
} // namespace test
} // namespace cpputil

#endif // CPP_UTIL_TESTHARNESS_H_
