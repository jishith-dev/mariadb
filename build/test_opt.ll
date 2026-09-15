; ModuleID = '/data/data/com.termux/files/home/mysql/build/test.ll'
source_filename = "/data/data/com.termux/files/home/mysql/build/test.ll"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "aarch64-unknown-linux-android24"

%Database = type { i64 }

@SEED = external local_unnamed_addr global i32
@.fmt_int_test_0 = private constant [4 x i8] c"%d\0A\00"
@argc = local_unnamed_addr global ptr null
@argv = local_unnamed_addr global i32 0
@.str_test_0 = private unnamed_addr constant [10 x i8] c"127.0.0.1\00"
@.str_test_1 = private unnamed_addr constant [5 x i8] c"root\00"
@.str_test_2 = private unnamed_addr constant [1 x i8] zeroinitializer
@.str_test_3 = private unnamed_addr constant [5 x i8] c"test\00"
@t_test_0 = global %Database zeroinitializer
@.str_test_4 = private unnamed_addr constant [10 x i8] c"connected\00"
@.str_test_5 = private unnamed_addr constant [53 x i8] c"CREATE TABLE IF NOT EXISTS users (id INT, name TEXT)\00"
@t_test_1 = local_unnamed_addr global i32 0
@.str_test_6 = private unnamed_addr constant [40 x i8] c"INSERT INTO users VALUES (1, 'Jishith')\00"
@.str_test_7 = private unnamed_addr constant [7 x i8] c"closed\00"

; Function Attrs: nofree nounwind
define void @_screen_int_test_0(i32 %x) local_unnamed_addr #0 {
entry:
  %0 = tail call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.fmt_int_test_0, i32 %x)
  %1 = tail call i32 @fflush(ptr null)
  ret void
}

declare void @_zen_string_free(ptr) local_unnamed_addr

declare void @_os_exit(i32) local_unnamed_addr

; Function Attrs: nofree nounwind
define void @_screen_string_test_0(ptr readonly captures(none) %x) local_unnamed_addr #0 {
entry:
  %puts = tail call i32 @puts(ptr nonnull dereferenceable(1) %x)
  %0 = tail call i32 @fflush(ptr null)
  ret void
}

; Function Attrs: nofree nounwind
declare noundef i32 @fflush(ptr noundef captures(none)) local_unnamed_addr #0

; Function Attrs: nofree nounwind
declare noundef i32 @printf(ptr noundef readonly captures(none), ...) local_unnamed_addr #0

declare ptr @_str_dup(ptr) local_unnamed_addr

declare i64 @_time_millis() local_unnamed_addr

declare i1 @Database_ok(ptr) local_unnamed_addr

declare i32 @Database_query(ptr, ptr) local_unnamed_addr

declare ptr @Database_error(ptr) local_unnamed_addr

declare void @Database_close(ptr) local_unnamed_addr

declare void @zen_lib_connect(ptr, ptr, ptr, ptr, ptr, i32) local_unnamed_addr

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
  %t14 = alloca %Database, align 8
  store i32 %argc, ptr @argc, align 8
  store ptr %argv, ptr @argv, align 8
  %t7 = tail call ptr @_str_dup(ptr nonnull @.str_test_0)
  %t9 = tail call ptr @_str_dup(ptr nonnull @.str_test_1)
  %t11 = tail call ptr @_str_dup(ptr nonnull @.str_test_2)
  %t13 = tail call ptr @_str_dup(ptr nonnull @.str_test_3)
  call void @zen_lib_connect(ptr nonnull sret(%Database) %t14, ptr %t7, ptr %t9, ptr %t11, ptr %t13, i32 3306)
  %0 = load i64, ptr %t14, align 8
  store i64 %0, ptr @t_test_0, align 8
  %t15 = call i1 @Database_ok(ptr nonnull @t_test_0)
  br i1 %t15, label %end0, label %if1

if1:                                              ; preds = %entry
  %t17 = call ptr @Database_error(ptr nonnull @t_test_0)
  %puts.i = call i32 @puts(ptr nonnull readonly dereferenceable(1) %t17)
  %1 = call i32 @fflush(ptr null)
  call void @_os_exit(i32 1)
  br label %end0

end0:                                             ; preds = %if1, %entry
  %t19 = call ptr @_str_dup(ptr nonnull @.str_test_4)
  %puts.i1 = call i32 @puts(ptr nonnull readonly dereferenceable(1) %t19)
  %2 = call i32 @fflush(ptr null)
  call void @_zen_string_free(ptr nonnull %t19)
  %t21 = call ptr @_str_dup(ptr nonnull @.str_test_5)
  %t22 = call i32 @Database_query(ptr nonnull @t_test_0, ptr %t21)
  store i32 %t22, ptr @t_test_1, align 4
  %3 = call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.fmt_int_test_0, i32 %t22)
  %4 = call i32 @fflush(ptr null)
  %t24 = load i32, ptr @t_test_1, align 4
  %t25.not = icmp eq i32 %t24, 0
  br i1 %t25.not, label %end2, label %if3

if3:                                              ; preds = %end0
  %t26 = call ptr @Database_error(ptr nonnull @t_test_0)
  %puts.i2 = call i32 @puts(ptr nonnull readonly dereferenceable(1) %t26)
  %5 = call i32 @fflush(ptr null)
  br label %end2

end2:                                             ; preds = %if3, %end0
  %t28 = call ptr @_str_dup(ptr nonnull @.str_test_6)
  %t29 = call i32 @Database_query(ptr nonnull @t_test_0, ptr %t28)
  store i32 %t29, ptr @t_test_1, align 4
  %6 = call i32 (ptr, ...) @printf(ptr nonnull dereferenceable(1) @.fmt_int_test_0, i32 %t29)
  %7 = call i32 @fflush(ptr null)
  %t31 = load i32, ptr @t_test_1, align 4
  %t32.not = icmp eq i32 %t31, 0
  br i1 %t32.not, label %end4, label %if5

if5:                                              ; preds = %end2
  %t33 = call ptr @Database_error(ptr nonnull @t_test_0)
  %puts.i3 = call i32 @puts(ptr nonnull readonly dereferenceable(1) %t33)
  %8 = call i32 @fflush(ptr null)
  br label %end4

end4:                                             ; preds = %if5, %end2
  call void @Database_close(ptr nonnull @t_test_0)
  %t35 = call ptr @_str_dup(ptr nonnull @.str_test_7)
  %puts.i4 = call i32 @puts(ptr nonnull readonly dereferenceable(1) %t35)
  %9 = call i32 @fflush(ptr null)
  call void @_zen_string_free(ptr nonnull %t35)
  ret i32 0
}

; Function Attrs: nofree nounwind
declare noundef i32 @puts(ptr noundef readonly captures(none)) local_unnamed_addr #0

attributes #0 = { nofree nounwind }
