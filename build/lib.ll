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
declare void @llvm.memcpy.p0.p0.i64(ptr, ptr, i64, i1)
declare ptr @_zen_list_push(ptr, ptr)
%ZenList = type { ptr, i32, i32, i64 }
declare void @_zen_list_set_meta(ptr, i32, i32)
declare ptr @_zen_list_new(i64)
declare ptr @zen_mysql_error (i64)
declare i32 @zen_mysql_query (i64, ptr)
declare void @zen_mysql_close (i64)
declare i64 @zen_mysql_connect (ptr, ptr, ptr, ptr, i32)
%HttpServer = type opaque
%HttpRequest = type opaque
%HttpResponse = type opaque
%Json = type opaque
%JsonArray = type opaque
%JsonObject = type opaque
%Ptr = type { ptr }
%Map = type opaque
@argc = external global i32
@argv = external global ptr
%Database = type { i64 }




define void @_zen_init_Database(ptr %this) {
ret void
}
define i1 @Database_ok (ptr %this) {
entry:


%t0 = getelementptr %Database, %Database* %this, i32 0, i32 0
%t1 = load i64, ptr %t0
%t3 = sext i32 0 to i64
%t2 = icmp ne i64 %t1, %t3
ret i1 %t2
}
define i32 @Database_query (ptr %this, ptr %t0) {
entry:

%sql.addr = alloca ptr
store ptr %t0, ptr %sql.addr
%t1 = getelementptr %Database, %Database* %this, i32 0, i32 0
%t2 = load i64, ptr %t1
%t3 = load ptr, ptr %sql.addr
%t4 = call i32 @zen_mysql_query(i64 %t2, ptr %t3)
ret i32 %t4
}
define ptr @Database_error (ptr %this) {
entry:


%t0 = getelementptr %Database, %Database* %this, i32 0, i32 0
%t1 = load i64, ptr %t0
%t2 = call ptr @zen_mysql_error(i64 %t1)
ret ptr %t2
}
define void @Database_close (ptr %this) {
entry:


%t0 = getelementptr %Database, %Database* %this, i32 0, i32 0
%t1 = load i64, ptr %t0
call void @zen_mysql_close(i64 %t1)
ret void
}
define void @zen_lib_connect (ptr sret(%Database) %sret, ptr %t0, ptr %t1, ptr %t2, ptr %t3, i32 %t4) {
entry:
%t11 = alloca i64
%t13 = alloca %Database
%host.addr = alloca ptr
store ptr %t0, ptr %host.addr
%user.addr = alloca ptr
store ptr %t1, ptr %user.addr
%password.addr = alloca ptr
store ptr %t2, ptr %password.addr
%database.addr = alloca ptr
store ptr %t3, ptr %database.addr
%port.addr = alloca i32
store i32 %t4, ptr %port.addr
%t5 = load ptr, ptr %host.addr
%t6 = load ptr, ptr %user.addr
%t7 = load ptr, ptr %password.addr
%t8 = load ptr, ptr %database.addr
%t9 = load i32, ptr %port.addr
%t10 = call i64 @zen_mysql_connect(ptr %t5, ptr %t6, ptr %t7, ptr %t8, i32 %t9)
store i64 %t10, ptr %t11
%t14 = getelementptr inbounds %Database, ptr %t13, i32 0, i32 0
%t15 = load i64, ptr %t11
store i64 %t15, ptr %t14
call void @llvm.memcpy.p0.p0.i64(ptr %sret, ptr %t13, i64 8, i1 false)
ret void
}
