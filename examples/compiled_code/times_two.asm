; ModuleID = 'ELL'
source_filename = "ELL"

@c_0 = internal constant [3 x double] [double 2.000000e+00, double 2.000000e+00, double 2.000000e+00]
@g_0 = internal global [3 x double] zeroinitializer

define void @predict(double* %input0, double* %output0) {
entry:
  br label %Node_1384

Node_1384:                                        ; preds = %entry
  br label %for.init

for.init:                                         ; preds = %Node_1384
  %0 = alloca i32
  store i32 0, i32* %0
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %1 = load i32, i32* %0
  %2 = icmp slt i32 %1, 3
  br i1 %2, label %for.body, label %for.after

for.body:                                         ; preds = %for.cond
  %3 = load i32, i32* %0
  %4 = getelementptr double, double* %input0, i32 %3
  %5 = load double, double* %4
  %6 = getelementptr [3 x double], [3 x double]* @c_0, i32 0, i32 %3
  %7 = load double, double* %6
  %8 = fmul double %5, %7
  %9 = getelementptr [3 x double], [3 x double]* @g_0, i32 0, i32 %3
  store double %8, double* %9
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %10 = load i32, i32* %0
  %11 = add i32 %10, 1
  store i32 %11, i32* %0
  br label %for.cond

for.after:                                        ; preds = %for.cond
  %12 = load double, double* getelementptr inbounds ([3 x double], [3 x double]* @g_0, i32 0, i32 0)
  %13 = getelementptr double, double* %output0, i32 0
  store double %12, double* %13
  %14 = load double, double* getelementptr inbounds ([3 x double], [3 x double]* @g_0, i32 0, i32 1)
  %15 = getelementptr double, double* %output0, i32 1
  store double %14, double* %15
  %16 = load double, double* getelementptr inbounds ([3 x double], [3 x double]* @g_0, i32 0, i32 2)
  %17 = getelementptr double, double* %output0, i32 2
  store double %16, double* %17
  ret void
}
