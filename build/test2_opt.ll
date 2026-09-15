; ModuleID = '/data/data/com.termux/files/home/mysql/build/test2.ll'
source_filename = "/data/data/com.termux/files/home/mysql/build/test2.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-android24"

@SEED = external local_unnamed_addr global i32
@argc = local_unnamed_addr global ptr null
@argv = local_unnamed_addr global i32 0

declare void @foo() local_unnamed_addr

declare i64 @_time_millis() local_unnamed_addr

define void @_assignSeed() local_unnamed_addr {
entry:
  %t0 = tail call i64 @_time_millis()
  store i64 %t0, ptr @SEED, align 8
  ret void
}

define noundef i32 @main(i32 %argc, ptr %argv) local_unnamed_addr {
entry:
  %t0.i = tail call i64 @_time_millis()
  store i64 %t0.i, ptr @SEED, align 8
  store i32 %argc, ptr @argc, align 8
  store ptr %argv, ptr @argv, align 8
  tail call void @foo()
  ret i32 0
}
