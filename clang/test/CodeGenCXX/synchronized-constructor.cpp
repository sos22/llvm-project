// RUN: %clang_cc1 -emit-llvm %s -o - |  FileCheck -check-prefix STATICSAFE %s
// RUN: %clang_cc1 -emit-llvm %s -fno-threadsafe-statics -o - | FileCheck -check-prefix STATICUNSAFE %s

struct teststruct {
teststruct() __attribute ((synchronized_constructor));

teststruct(int); // argument constructor needs synch.
};

void func()
{
  static teststruct nosynch;
// STATICSAFE: load atomic i8, i8* @_ZGVZ4funcvE7nosynch acquire
// STATICSAFE-NOT: __cxa_guard_acquire
// STATICSAFE: call void @_ZN10teststructC1Ev(%struct.teststruct* @_ZZ4funcvE7nosynch)
// STATICSAFE-NEXT: store atomic i8 1, i8* @_ZGVZ4funcvE7nosynch release, align 1
// STATICUNSAFE: load i8, i8* @_ZGVZ4funcvE7nosynch
// STATICUNSAFE: call void @_ZN10teststructC1Ev
// STATICUNSAFE-NEXT: store i8 1, i8* @_ZGVZ4funcvE7nosynch

  static teststruct synch(1);
// STATICSAFE: load atomic i8, i8* bitcast (i64* @_ZGVZ4funcvE5synch to i8*) acquire
// STATICSAFE: call i32 @__cxa_guard_acquire(i64* @_ZGVZ4funcvE5synch)
// STATICSAFE: call void @_ZN10teststructC1Ei(%struct.teststruct* @_ZZ4funcvE5synch, i32 1)
// STATICSAFE-NEXT: call void @__cxa_guard_release(i64* @_ZGVZ4funcvE5synch)
// STATICUNSAFE: load i8, i8* @_ZGVZ4funcvE5synch
// STATICUNSAFE: call void @_ZN10teststructC1Ei
// STATICUNSAFE-NEXT: store i8 1, i8* @_ZGVZ4funcvE5synch
}

