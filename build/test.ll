target triple = "aarch64-unknown-linux-android24"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128-Fn32"
@NAN = external constant double
@NEG_INF = external constant double
@INF = external constant double
@F64_EPS = external constant double
@F64_MIN = external constant double
@F64_MAX = external constant double
@I32_MIN = external constant i32
@I32_MAX = external constant i32
@SEED = external global i32
@LN10 = external constant double
@LN2 = external constant double
@SQRT2 = external constant double
@PHI = external constant double
@E = external constant double
@TAU = external constant double
@PI = external constant double
define void @_screen_int_test_0(i32 %x) {
entry:
  call i32 (ptr, ...) @printf(ptr getelementptr ([4 x i8], [4 x i8]* @.fmt_int_test_0, i32 0, i32 0),
    i32 %x)
  call i32 @fflush(ptr null)
  ret void
}
@.fmt_int_test_0 = private constant [4 x i8] c"%d\0A\00"
declare void @_zen_string_free(ptr)
declare void @_os_exit(i32)
define void @_screen_string_test_0(ptr %x) {
entry:
  call i32 (ptr, ...) @printf(ptr getelementptr ([4 x i8], [4 x i8]* @.fmt_string_test_0, i32 0, i32 0),
    ptr %x)
  call i32 @fflush(ptr null)
  ret void
}
@.fmt_string_test_0 = private constant [4 x i8] c"%s\0A\00"
declare i32 @fflush(ptr)
declare i32 @printf(ptr, ...)
declare void @llvm.memcpy.p0.p0.i64(ptr, ptr, i64, i1)
declare ptr @_str_dup(ptr)
declare i64 @_time_millis()
%HttpServer = type opaque
%HttpRequest = type opaque
%HttpResponse = type opaque
%Json = type opaque
%JsonArray = type opaque
%JsonObject = type opaque
%Ptr = type { ptr }
%Map = type opaque
declare void @_zen_init_Database(ptr)
%Database = type { i64 }
declare i1 @Database_ok(ptr)
declare i32 @Database_query(ptr, ptr)
declare ptr @Database_error(ptr)
declare void @Database_close(ptr)
declare void @zen_lib_connect(ptr, ptr, ptr, ptr, ptr, i32)
@argc = global ptr null
@argv = global i32 0
@.str_test_0 = private unnamed_addr constant [10 x i8] c"127.0.0.1\00"
@.str_test_1 = private unnamed_addr constant [5 x i8] c"root\00"
@.str_test_2 = private unnamed_addr constant [1 x i8] c"\00"
@.str_test_3 = private unnamed_addr constant [5 x i8] c"test\00"
@t_test_0 = global %Database zeroinitializer
@.str_test_4 = private unnamed_addr constant [10 x i8] c"connected\00"
@.str_test_5 = private unnamed_addr constant [53 x i8] c"CREATE TABLE IF NOT EXISTS users (id INT, name TEXT)\00"
@t_test_1 = global i32  0
@.str_test_6 = private unnamed_addr constant [40 x i8] c"INSERT INTO users VALUES (1, 'Jishith')\00"
@.str_test_7 = private unnamed_addr constant [7 x i8] c"closed\00"

define void @_assignSeed () {
  entry:

  %t0 = call i64 @_time_millis()
  store i64 %t0, ptr @SEED
  ret void
}
    
define i32 @main(i32 %argc, ptr %argv) { 
entry:
call void @_assignSeed()
%t14 = alloca %Database
store i32 %argc, ptr @argc
store ptr %argv, ptr @argv
%t6 = getelementptr inbounds [10 x i8], ptr @.str_test_0, i64 0, i64 0
%t7 = call ptr @_str_dup(ptr %t6)
%t8 = getelementptr inbounds [5 x i8], ptr @.str_test_1, i64 0, i64 0
%t9 = call ptr @_str_dup(ptr %t8)
%t10 = getelementptr inbounds [1 x i8], ptr @.str_test_2, i64 0, i64 0
%t11 = call ptr @_str_dup(ptr %t10)
%t12 = getelementptr inbounds [5 x i8], ptr @.str_test_3, i64 0, i64 0
%t13 = call ptr @_str_dup(ptr %t12)
call void @zen_lib_connect(ptr sret(%Database) %t14, ptr %t7, ptr %t9, ptr %t11, ptr %t13, i32 3306)
call void @llvm.memcpy.p0.p0.i64(ptr @t_test_0, ptr %t14, i64 8, i1 false)
%t15 = call i1 @Database_ok(ptr @t_test_0)
%t16 = xor i1 %t15, true
br i1 %t16, label %if1, label %end0
if1:
%t17 = call ptr @Database_error(ptr @t_test_0)
call void @_screen_string_test_0(ptr %t17)
call void @_os_exit(i32 1)
br label %end0
end0:
%t18 = getelementptr inbounds [10 x i8], ptr @.str_test_4, i64 0, i64 0
%t19 = call ptr @_str_dup(ptr %t18)
call void @_screen_string_test_0(ptr %t19)
call void @_zen_string_free(ptr %t19)
%t20 = getelementptr inbounds [53 x i8], ptr @.str_test_5, i64 0, i64 0
%t21 = call ptr @_str_dup(ptr %t20)
%t22 = call i32 @Database_query(ptr @t_test_0, ptr %t21)
store i32 %t22, ptr @t_test_1
%t23 = load i32, ptr @t_test_1
call void @_screen_int_test_0(i32 %t23)
%t24 = load i32, ptr @t_test_1
%t25 = icmp ne i32 %t24, 0
br i1 %t25, label %if3, label %end2
if3:
%t26 = call ptr @Database_error(ptr @t_test_0)
call void @_screen_string_test_0(ptr %t26)
br label %end2
end2:
%t27 = getelementptr inbounds [40 x i8], ptr @.str_test_6, i64 0, i64 0
%t28 = call ptr @_str_dup(ptr %t27)
%t29 = call i32 @Database_query(ptr @t_test_0, ptr %t28)
store i32 %t29, ptr @t_test_1
%t30 = load i32, ptr @t_test_1
call void @_screen_int_test_0(i32 %t30)
%t31 = load i32, ptr @t_test_1
%t32 = icmp ne i32 %t31, 0
br i1 %t32, label %if5, label %end4
if5:
%t33 = call ptr @Database_error(ptr @t_test_0)
call void @_screen_string_test_0(ptr %t33)
br label %end4
end4:
call void @Database_close(ptr @t_test_0)
%t34 = getelementptr inbounds [7 x i8], ptr @.str_test_7, i64 0, i64 0
%t35 = call ptr @_str_dup(ptr %t34)
call void @_screen_string_test_0(ptr %t35)
call void @_zen_string_free(ptr %t35)
ret i32 0 
}
