// RUN: %clang_cc1 -fno-threadsafe-statics -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -Wunclear-static-synchronization -emit-llvm -verify %s -o /dev/null

struct teststruct {
teststruct() __attribute__ ((unsynchronized_constructor));
teststruct(int);
};

void func()
{
	static teststruct ts;
// CHECK: load atomic i8, i8* bitcast (i64* @_ZGVZ4funcvE2ts to i8*) acquire
// CHECK: call i32 @__cxa_guard_acquire(i64* @_ZGVZ4funcvE2ts)
// CHECK: call void @_ZN10teststructC1Ev(%struct.teststruct* @_ZZ4funcvE2ts)
// CHECK-NEXT: call void @__cxa_guard_release(i64* @_ZGVZ4funcvE2ts)
}

void func2()
{
  static teststruct ts2(1); // expected-warning {{function-local static ts2 generates implicit synchronization}}
}
