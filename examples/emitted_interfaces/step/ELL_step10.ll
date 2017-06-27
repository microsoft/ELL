; ModuleID = 'ELL'
source_filename = "ELL"

@g_0 = internal global double 0.000000e+00
@g_1 = internal global [10 x double] zeroinitializer
@0 = private unnamed_addr constant [33 x i8] c"SteppableMap_10_50_DataCallback\0A\00"
@g_2 = internal global [10 x double] zeroinitializer
@c_0 = internal constant [10 x double] [double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00]
@g_3 = internal global [10 x double] zeroinitializer
@1 = private unnamed_addr constant [36 x i8] c"SteppableMap_10_50_ResultsCallback\0A\00"
@g_4 = internal global [10 x double] zeroinitializer
@g_5 = internal global double 0.000000e+00
@2 = private unnamed_addr constant [44 x i8] c"sampleTicks = %f, increment = %f, end = %f\0A\00"
@g_5.1 = internal global double 0.000000e+00

define void @Step10_Predict(double* %input0, double* %output0) !ell.header.declare !0 !ell.fn.predict !0 {
entry:
  %0 = alloca i32
  br label %Node_1037

Node_1037:                                        ; preds = %entry
  %1 = getelementptr double, double* %input0, i32 0
  call void @_Node__SourceNode_double__in_2_out_10(double* %1, double* getelementptr inbounds ([10 x double], [10 x double]* @g_2, i32 0, i32 0))
  br label %Node_1039

Node_1039:                                        ; preds = %Node_1037
  call void @_Node__BinaryOperationNode_double__in_10_10_out_10(double* getelementptr inbounds ([10 x double], [10 x double]* @g_2, i32 0, i32 0), double* getelementptr inbounds ([10 x double], [10 x double]* @c_0, i32 0, i32 0), double* getelementptr inbounds ([10 x double], [10 x double]* @g_3, i32 0, i32 0))
  call void @_Node__SinkNode_double__in_10_out_10(double* getelementptr inbounds ([10 x double], [10 x double]* @g_3, i32 0, i32 0), double* getelementptr inbounds ([10 x double], [10 x double]* @g_4, i32 0, i32 0))
  br label %Node_1042

Node_1042:                                        ; preds = %Node_1039
  br label %for.init

for.init:                                         ; preds = %Node_1042
  store i32 0, i32* %0
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %2 = load i32, i32* %0
  %3 = icmp slt i32 %2, 10
  br i1 %3, label %for.body, label %for.after

for.body:                                         ; preds = %for.cond
  %4 = load i32, i32* %0
  %5 = add i32 %4, 0
  %6 = add i32 %4, 0
  %7 = getelementptr [10 x double], [10 x double]* @g_4, i32 0, i32 %5
  %8 = load double, double* %7
  %9 = getelementptr double, double* %output0, i32 %6
  store double %8, double* %9
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %10 = load i32, i32* %0
  %11 = add i32 %10, 1
  store i32 %11, i32* %0
  br label %for.cond

for.after:                                        ; preds = %for.cond
  ret void
}

define void @_Node__SourceNode_double__in_2_out_10(double* %input1, double* %output1) {
entry:
  %0 = alloca i32
  br label %Node_1038

Node_1038:                                        ; preds = %entry
  store double 0.000000e+00, double* @g_0
  %1 = load double, double* @g_0
  %2 = getelementptr double, double* %input1, i32 0
  %3 = load double, double* %2
  %4 = getelementptr double, double* %input1, i32 1
  %5 = load double, double* %4
  %6 = fcmp one double %3, %1
  br i1 %6, label %if.then, label %if.end

if.then:                                          ; preds = %Node_1038
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([33 x i8], [33 x i8]* @0, i32 0, i32 0))
  %8 = call i8 @SteppableMap_10_50_DataCallback(double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 0))
  %9 = icmp eq i8 %8, 1
  br i1 %9, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.then
  %10 = fcmp ogt double %5, %3
  br i1 %10, label %if.then5, label %if.end6

if.then5:                                         ; preds = %if.then2
  br label %if.after4

if.end6:                                          ; preds = %if.then2
  br label %if.after4

if.after4:                                        ; preds = %if.end6, %if.then5
  br label %if.after1

if.end3:                                          ; preds = %if.then
  br label %if.after1

if.after1:                                        ; preds = %if.end3, %if.after4
  br label %if.after

if.end:                                           ; preds = %Node_1038
  br label %if.after

if.after:                                         ; preds = %if.end, %if.after1
  br label %for.init

for.init:                                         ; preds = %if.after
  store i32 0, i32* %0
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %11 = load i32, i32* %0
  %12 = icmp slt i32 %11, 10
  br i1 %12, label %for.body, label %for.after

for.body:                                         ; preds = %for.cond
  %13 = load i32, i32* %0
  %14 = getelementptr [10 x double], [10 x double]* @g_1, i32 0, i32 %13
  %15 = load double, double* %14
  %16 = getelementptr double, double* %output1, i32 %13
  store double %15, double* %16
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %17 = load i32, i32* %0
  %18 = add i32 %17, 1
  store i32 %18, i32* %0
  br label %for.cond

for.after:                                        ; preds = %for.cond
  store double %3, double* @g_0
  ret void
}

declare !ell.header.declare !0 !ell.fn.callback !1 i8 @SteppableMap_10_50_DataCallback(double*)

declare i32 @printf(i8*, ...)

define void @_Node__BinaryOperationNode_double__in_10_10_out_10(double* %input2, double* %input3, double* %output2) {
entry:
  %0 = alloca i32
  br label %Node_1040

Node_1040:                                        ; preds = %entry
  br label %for.init

for.init:                                         ; preds = %Node_1040
  store i32 0, i32* %0
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %1 = load i32, i32* %0
  %2 = icmp slt i32 %1, 10
  br i1 %2, label %for.body, label %for.after

for.body:                                         ; preds = %for.cond
  %3 = load i32, i32* %0
  %4 = getelementptr double, double* %input2, i32 %3
  %5 = load double, double* %4
  %6 = getelementptr double, double* %input3, i32 %3
  %7 = load double, double* %6
  %8 = fmul double %5, %7
  %9 = getelementptr double, double* %output2, i32 %3
  store double %8, double* %9
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %10 = load i32, i32* %0
  %11 = add i32 %10, 1
  store i32 %11, i32* %0
  br label %for.cond

for.after:                                        ; preds = %for.cond
  ret void
}

define void @_Node__SinkNode_double__in_10_out_10(double* %input4, double* %output3) {
entry:
  %0 = alloca i32
  br label %Node_1041

Node_1041:                                        ; preds = %entry
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([36 x i8], [36 x i8]* @1, i32 0, i32 0))
  %2 = getelementptr double, double* %input4, i32 0
  call void @SteppableMap_10_50_ResultsCallback(double* %2)
  br label %for.init

for.init:                                         ; preds = %Node_1041
  store i32 0, i32* %0
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %3 = load i32, i32* %0
  %4 = icmp slt i32 %3, 10
  br i1 %4, label %for.body, label %for.after

for.body:                                         ; preds = %for.cond
  %5 = load i32, i32* %0
  %6 = getelementptr double, double* %input4, i32 %5
  %7 = load double, double* %6
  %8 = getelementptr double, double* %output3, i32 %5
  store double %7, double* %8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %9 = load i32, i32* %0
  %10 = add i32 %9, 1
  store i32 %10, i32* %0
  br label %for.cond

for.after:                                        ; preds = %for.cond
  ret void
}

declare !ell.header.declare !0 !ell.fn.callback !2 void @SteppableMap_10_50_ResultsCallback(double*)

define i32 @ELL_GetInputSize() !ell.header.declare !0 {
entry:
  ret i32 2
}

define i32 @ELL_GetOutputSize() !ell.header.declare !0 {
entry:
  ret i32 10
}

define i32 @ELL_GetNumNodes() !ell.header.declare !0 {
entry:
  ret i32 6
}

declare double @ELL_GetSteadyClockMilliseconds()

define void @Step10(double* %input5, double* %output4) !ell.header.declare !0 !ell.fn.step !3 {
entry:
  %0 = call double @ELL_GetSteadyClockMilliseconds()
  store double 0.000000e+00, double* @g_5
  %1 = load double, double* @g_5
  %2 = fcmp oeq double %1, 0.000000e+00
  %3 = alloca double, i32 2
  %4 = alloca double
  %5 = alloca double
  br i1 %2, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %6 = fsub double %0, 5.000000e+01
  store double %6, double* @g_5
  br label %if.after

if.end:                                           ; preds = %entry
  br label %if.after

if.after:                                         ; preds = %if.end, %if.then
  %7 = load double, double* @g_5
  %8 = fadd double %7, 5.000000e+01
  %9 = getelementptr double, double* %4, i32 0
  store double %0, double* %9
  br label %for.init

for.init:                                         ; preds = %if.after
  store double %8, double* %5
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %for.init
  %10 = getelementptr double, double* %4, i32 0
  %11 = load double, double* %10
  %12 = load double, double* %5
  %13 = fcmp ole double %12, %11
  br i1 %13, label %for.body, label %for.after

for.body:                                         ; preds = %for.cond
  %14 = load double, double* %5
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([44 x i8], [44 x i8]* @2, i32 0, i32 0), double %14, double 5.000000e+01, double %0)
  %16 = fsub double %14, %7
  %17 = getelementptr double, double* %3, i32 0
  store double %16, double* %17
  %18 = fsub double %0, %7
  %19 = getelementptr double, double* %3, i32 1
  store double %18, double* %19
  %20 = getelementptr double, double* %3, i32 0
  call void @Step10_Predict(double* %20, double* %output4)
  store double %14, double* @g_5
  %21 = call double @ELL_GetSteadyClockMilliseconds()
  %22 = getelementptr double, double* %4, i32 0
  store double %21, double* %22
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %23 = load double, double* %5
  %24 = fadd double %23, 5.000000e+01
  store double %24, double* %5
  br label %for.cond

for.after:                                        ; preds = %for.cond
  ret void
}

define double @Step10_WaitTimeForNextPredict() !ell.header.declare !0 !ell.fn.stepTime !4 {
entry:
  %0 = alloca double
  store double 0.000000e+00, double* %0
  store double 0.000000e+00, double* @g_5.1
  %1 = load double, double* @g_5.1
  %2 = fcmp oeq double %1, 0.000000e+00
  br i1 %2, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %3 = fadd double %1, 5.000000e+01
  %4 = call double @ELL_GetSteadyClockMilliseconds()
  %5 = fcmp ogt double %3, %4
  br i1 %5, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.then
  %6 = fsub double %3, %4
  store double %6, double* %0
  br label %if.after1

if.end3:                                          ; preds = %if.then
  br label %if.after1

if.after1:                                        ; preds = %if.end3, %if.then2
  br label %if.after

if.end:                                           ; preds = %entry
  br label %if.after

if.after:                                         ; preds = %if.end, %if.after1
  %7 = load double, double* %0
  ret double %7
}

define double @Step10_GetInterval() !ell.header.declare !0 !ell.fn.stepTime !5 {
entry:
  %0 = alloca double
  store double 5.000000e+01, double* %0
  %1 = load double, double* %0
  ret double %1
}

!0 = !{!""}
!1 = !{!"SourceNode"}
!2 = !{!"SinkNode"}
!3 = !{!"10"}
!4 = !{!"WaitTimeForNextStep"}
!5 = !{!"GetInterval"}
