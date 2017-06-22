; ModuleID = 'ELL'
source_filename = "ELL"

@g_0 = internal global double 0.000000e+00
@g_1 = internal global [10 x double] zeroinitializer
@g_2 = internal global [10 x double] zeroinitializer
@c_0 = internal constant [10 x double] [double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00, double 2.000000e+00]
@g_3 = internal global [10 x double] zeroinitializer
@g_4 = internal global double 0.000000e+00
@0 = private unnamed_addr constant [44 x i8] c"sampleTicks = %f, increment = %f, end = %f\0A\00"
@g_4.1 = internal global double 0.000000e+00

define void @Step10_Predict(double* %input0, double* %output0) !ell.header.declare !0 !ell.fn.predict !0 {
entry:
  %0 = alloca i32
  br label %Node_1031

Node_1031:                                        ; preds = %entry
  %1 = getelementptr double, double* %input0, i32 0
  call void @_Node__SourceNode_double__in_2_out_10(double* %1, double* getelementptr inbounds ([10 x double], [10 x double]* @g_2, i32 0, i32 0))
  br label %Node_1033

Node_1033:                                        ; preds = %Node_1031
  call void @_Node__BinaryOperationNode_double__in_10_10_out_10(double* getelementptr inbounds ([10 x double], [10 x double]* @g_2, i32 0, i32 0), double* getelementptr inbounds ([10 x double], [10 x double]* @c_0, i32 0, i32 0), double* getelementptr inbounds ([10 x double], [10 x double]* @g_3, i32 0, i32 0))
  br label %Node_1035

Node_1035:                                        ; preds = %Node_1033
  br label %for.init

for.init:                                         ; preds = %Node_1035
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
  %7 = getelementptr [10 x double], [10 x double]* @g_3, i32 0, i32 %5
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
  br label %Node_1032

Node_1032:                                        ; preds = %entry
  store double 0x7FC4E770AD38, double* @g_0
  %0 = load double, double* @g_0
  %1 = getelementptr double, double* %input1, i32 0
  %2 = load double, double* %1
  %3 = getelementptr double, double* %input1, i32 1
  %4 = load double, double* %3
  %5 = fcmp one double %2, %0
  br i1 %5, label %if.then, label %if.end

if.then:                                          ; preds = %Node_1032
  %6 = call i8 @SteppableMap_10_50_DataCallback(double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 0))
  %7 = icmp eq i8 %6, 1
  br i1 %7, label %if.then2, label %if.end3

if.then2:                                         ; preds = %if.then
  %8 = fcmp ogt double %4, %2
  br i1 %8, label %if.then5, label %if.end6

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

if.end:                                           ; preds = %Node_1032
  br label %if.after

if.after:                                         ; preds = %if.end, %if.after1
  %9 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 0)
  %10 = getelementptr double, double* %output1, i32 0
  store double %9, double* %10
  %11 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 1)
  %12 = getelementptr double, double* %output1, i32 1
  store double %11, double* %12
  %13 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 2)
  %14 = getelementptr double, double* %output1, i32 2
  store double %13, double* %14
  %15 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 3)
  %16 = getelementptr double, double* %output1, i32 3
  store double %15, double* %16
  %17 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 4)
  %18 = getelementptr double, double* %output1, i32 4
  store double %17, double* %18
  %19 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 5)
  %20 = getelementptr double, double* %output1, i32 5
  store double %19, double* %20
  %21 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 6)
  %22 = getelementptr double, double* %output1, i32 6
  store double %21, double* %22
  %23 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 7)
  %24 = getelementptr double, double* %output1, i32 7
  store double %23, double* %24
  %25 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 8)
  %26 = getelementptr double, double* %output1, i32 8
  store double %25, double* %26
  %27 = load double, double* getelementptr inbounds ([10 x double], [10 x double]* @g_1, i32 0, i32 9)
  %28 = getelementptr double, double* %output1, i32 9
  store double %27, double* %28
  store double %2, double* @g_0
  ret void
}

declare !ell.header.declare !0 !ell.fn.callback !1 i8 @SteppableMap_10_50_DataCallback(double*)

define void @_Node__BinaryOperationNode_double__in_10_10_out_10(double* %input2, double* %input3, double* %output2) {
entry:
  %0 = alloca i32
  br label %Node_1034

Node_1034:                                        ; preds = %entry
  br label %for.init

for.init:                                         ; preds = %Node_1034
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
  ret i32 5
}

declare double @ELL_GetSteadyClockMilliseconds()

define void @Step10(double* %input4, double* %output3) !ell.header.declare !0 !ell.fn.step !2 {
entry:
  %0 = call double @ELL_GetSteadyClockMilliseconds()
  store double 0.000000e+00, double* @g_4
  %1 = load double, double* @g_4
  %2 = fcmp oeq double %1, 0.000000e+00
  %3 = alloca double, i32 2
  %4 = alloca double
  %5 = alloca double
  br i1 %2, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %6 = fsub double %0, 5.000000e+01
  store double %6, double* @g_4
  br label %if.after

if.end:                                           ; preds = %entry
  br label %if.after

if.after:                                         ; preds = %if.end, %if.then
  %7 = load double, double* @g_4
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
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([44 x i8], [44 x i8]* @0, i32 0, i32 0), double %14, double 5.000000e+01, double %0)
  %16 = fsub double %14, %7
  %17 = getelementptr double, double* %3, i32 0
  store double %16, double* %17
  %18 = fsub double %0, %7
  %19 = getelementptr double, double* %3, i32 1
  store double %18, double* %19
  %20 = getelementptr double, double* %3, i32 0
  call void @Step10_Predict(double* %20, double* %output3)
  store double %14, double* @g_4
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

declare i32 @printf(i8*, ...)

define double @Step10_WaitTimeForNextPredict() !ell.header.declare !0 !ell.fn.stepWaitTime !0 {
entry:
  %0 = alloca double
  store double 0.000000e+00, double* %0
  store double 0.000000e+00, double* @g_4.1
  %1 = load double, double* @g_4.1
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

!0 = !{!""}
!1 = !{!"SourceNode"}
!2 = !{!"10"}
