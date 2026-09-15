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
declare void @foo ()
declare i64 @_time_millis()
%HttpServer = type opaque
%HttpRequest = type opaque
%HttpResponse = type opaque
%Json = type opaque
%JsonArray = type opaque
%JsonObject = type opaque
%Ptr = type { ptr }
%Map = type opaque
@argc = global ptr null
@argv = global i32 0


define void @_assignSeed () {
  entry:

  %t0 = call i64 @_time_millis()
  store i64 %t0, ptr @SEED
  ret void
}
    
define i32 @main(i32 %argc, ptr %argv) { 
entry:
call void @_assignSeed()

store i32 %argc, ptr @argc
store ptr %argv, ptr @argv
call void @foo()
ret i32 0 
}
