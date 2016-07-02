; ModuleID = 'C:\src\EMLL\prototype\Translate\Translate.cpp'
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc18.0.0"

%struct._iobuf = type { i8* }
%struct.__crt_locale_pointers = type { %struct.__crt_locale_data*, %struct.__crt_multibyte_data* }
%struct.__crt_locale_data = type opaque
%struct.__crt_multibyte_data = type opaque

$printf = comdat any

$_vfprintf_l = comdat any

$__local_stdio_printf_options = comdat any

$"\01??_C@_07MEEEMOAP@?$CFd?0?5?$CFf?6?$AA@" = comdat any

$"\01??_C@_03PPOCCAPH@?$CFf?6?$AA@" = comdat any

$"\01??_C@_03MBCKNMHH@Foo?$AA@" = comdat any

$"\01??_C@_03HJJGLLBC@Goo?$AA@" = comdat any

$"\01??_C@_03LGBJPEEI@moo?$AA@" = comdat any

$"\01?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA" = comdat any

@g_value = internal global double 0.000000e+00, align 8
@g_constData = internal constant [3 x double] [double 3.300000e+00, double 4.400000e+00, double 5.500000e+00], align 16
@g_data = internal global [3 x double] zeroinitializer, align 16
@"\01??_C@_07MEEEMOAP@?$CFd?0?5?$CFf?6?$AA@" = linkonce_odr unnamed_addr constant [8 x i8] c"%d, %f\0A\00", comdat, align 1
@"\01??_C@_03PPOCCAPH@?$CFf?6?$AA@" = linkonce_odr unnamed_addr constant [4 x i8] c"%f\0A\00", comdat, align 1
@"\01??_C@_03MBCKNMHH@Foo?$AA@" = linkonce_odr unnamed_addr constant [4 x i8] c"Foo\00", comdat, align 1
@"\01??_C@_03HJJGLLBC@Goo?$AA@" = linkonce_odr unnamed_addr constant [4 x i8] c"Goo\00", comdat, align 1
@"\01??_C@_03LGBJPEEI@moo?$AA@" = linkonce_odr unnamed_addr constant [4 x i8] c"moo\00", comdat, align 1
@"\01?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA" = linkonce_odr global i64 0, comdat, align 8

; Function Attrs: nounwind uwtable
define void @"\01?Loop@@YAXHHH@Z"(i32 %startAt, i32 %maxValue, i32 %step) #0 {
entry:
  %step.addr = alloca i32, align 4
  %maxValue.addr = alloca i32, align 4
  %startAt.addr = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %step, i32* %step.addr, align 4
  store i32 %maxValue, i32* %maxValue.addr, align 4
  store i32 %startAt, i32* %startAt.addr, align 4
  store double 0.000000e+00, double* @g_value, align 8
  %0 = load i32, i32* %startAt.addr, align 4
  store i32 %0, i32* %i, align 4
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %1 = load i32, i32* %i, align 4
  %2 = load i32, i32* %maxValue.addr, align 4
  %cmp = icmp slt i32 %1, %2
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %3 = load i32, i32* %i, align 4
  %idxprom = sext i32 %3 to i64
  %arrayidx = getelementptr inbounds [3 x double], [3 x double]* @g_constData, i64 0, i64 %idxprom
  %4 = load double, double* %arrayidx, align 8
  %5 = load i32, i32* %i, align 4
  %idxprom1 = sext i32 %5 to i64
  %arrayidx2 = getelementptr inbounds [3 x double], [3 x double]* @g_data, i64 0, i64 %idxprom1
  store double %4, double* %arrayidx2, align 8
  %6 = load i32, i32* %i, align 4
  %idxprom3 = sext i32 %6 to i64
  %arrayidx4 = getelementptr inbounds [3 x double], [3 x double]* @g_data, i64 0, i64 %idxprom3
  %7 = load double, double* %arrayidx4, align 8
  %8 = load i32, i32* %i, align 4
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @"\01??_C@_07MEEEMOAP@?$CFd?0?5?$CFf?6?$AA@", i32 0, i32 0), i32 %8, double %7)
  %9 = load i32, i32* %i, align 4
  %idxprom5 = sext i32 %9 to i64
  %arrayidx6 = getelementptr inbounds [3 x double], [3 x double]* @g_data, i64 0, i64 %idxprom5
  %10 = load double, double* %arrayidx6, align 8
  %11 = load double, double* @g_value, align 8
  %add = fadd double %11, %10
  store double %add, double* @g_value, align 8
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %12 = load i32, i32* %step.addr, align 4
  %inc = add nsw i32 %12, 1
  store i32 %inc, i32* %step.addr, align 4
  br label %for.cond

for.end:                                          ; preds = %for.cond
  %13 = load double, double* @g_value, align 8
  %call7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @"\01??_C@_03PPOCCAPH@?$CFf?6?$AA@", i32 0, i32 0), double %13)
  ret void
}

; Function Attrs: inlinehint nounwind uwtable
define linkonce_odr i32 @printf(i8* %_Format, ...) #1 comdat {
entry:
  %_Format.addr = alloca i8*, align 8
  %_Result = alloca i32, align 4
  %_ArgList = alloca i8*, align 8
  store i8* %_Format, i8** %_Format.addr, align 8
  %_ArgList1 = bitcast i8** %_ArgList to i8*
  call void @llvm.va_start(i8* %_ArgList1)
  %0 = load i8*, i8** %_ArgList, align 8
  %1 = load i8*, i8** %_Format.addr, align 8
  %call = call %struct._iobuf* @__acrt_iob_func(i32 1)
  %call2 = call i32 @_vfprintf_l(%struct._iobuf* %call, i8* %1, %struct.__crt_locale_pointers* null, i8* %0)
  store i32 %call2, i32* %_Result, align 4
  %_ArgList3 = bitcast i8** %_ArgList to i8*
  call void @llvm.va_end(i8* %_ArgList3)
  %2 = load i32, i32* %_Result, align 4
  ret i32 %2
}

; Function Attrs: nounwind uwtable
define void @"\01?IfThen@@YAXHH@Z"(i32 %x, i32 %y) #0 {
entry:
  %y.addr = alloca i32, align 4
  %x.addr = alloca i32, align 4
  store i32 %y, i32* %y.addr, align 4
  store i32 %x, i32* %x.addr, align 4
  %0 = load i32, i32* %x.addr, align 4
  %cmp = icmp sgt i32 %0, 35
  br i1 %cmp, label %if.then, label %if.else4

if.then:                                          ; preds = %entry
  %1 = load i32, i32* %y.addr, align 4
  %cmp1 = icmp slt i32 %1, 50
  br i1 %cmp1, label %if.then2, label %if.else

if.then2:                                         ; preds = %if.then
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @"\01??_C@_03MBCKNMHH@Foo?$AA@", i32 0, i32 0))
  br label %if.end

if.else:                                          ; preds = %if.then
  %call3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @"\01??_C@_03HJJGLLBC@Goo?$AA@", i32 0, i32 0))
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then2
  br label %if.end11

if.else4:                                         ; preds = %entry
  %2 = load i32, i32* %y.addr, align 4
  %cmp5 = icmp sgt i32 %2, 43
  br i1 %cmp5, label %if.then6, label %if.else8

if.then6:                                         ; preds = %if.else4
  %3 = load i32, i32* %y.addr, align 4
  %conv = sitofp i32 %3 to double
  %call7 = call double @sin(double %conv) #4
  br label %if.end10

if.else8:                                         ; preds = %if.else4
  %call9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @"\01??_C@_03LGBJPEEI@moo?$AA@", i32 0, i32 0))
  br label %if.end10

if.end10:                                         ; preds = %if.else8, %if.then6
  br label %if.end11

if.end11:                                         ; preds = %if.end10, %if.end
  ret void
}

; Function Attrs: nounwind
declare double @sin(double) #2

; Function Attrs: norecurse nounwind uwtable
define i32 @main() #3 {
entry:
  call void @"\01?Loop@@YAXHHH@Z"(i32 0, i32 10, i32 1)
  ret i32 0
}

; Function Attrs: nounwind
declare void @llvm.va_start(i8*) #4

; Function Attrs: inlinehint nounwind uwtable
define linkonce_odr i32 @_vfprintf_l(%struct._iobuf* %_Stream, i8* %_Format, %struct.__crt_locale_pointers* %_Locale, i8* %_ArgList) #1 comdat {
entry:
  %_ArgList.addr = alloca i8*, align 8
  %_Locale.addr = alloca %struct.__crt_locale_pointers*, align 8
  %_Format.addr = alloca i8*, align 8
  %_Stream.addr = alloca %struct._iobuf*, align 8
  store i8* %_ArgList, i8** %_ArgList.addr, align 8
  store %struct.__crt_locale_pointers* %_Locale, %struct.__crt_locale_pointers** %_Locale.addr, align 8
  store i8* %_Format, i8** %_Format.addr, align 8
  store %struct._iobuf* %_Stream, %struct._iobuf** %_Stream.addr, align 8
  %0 = load i8*, i8** %_ArgList.addr, align 8
  %1 = load %struct.__crt_locale_pointers*, %struct.__crt_locale_pointers** %_Locale.addr, align 8
  %2 = load i8*, i8** %_Format.addr, align 8
  %3 = load %struct._iobuf*, %struct._iobuf** %_Stream.addr, align 8
  %call = call i64* @__local_stdio_printf_options()
  %4 = load i64, i64* %call, align 8
  %call1 = call i32 @__stdio_common_vfprintf(i64 %4, %struct._iobuf* %3, i8* %2, %struct.__crt_locale_pointers* %1, i8* %0)
  ret i32 %call1
}

declare %struct._iobuf* @__acrt_iob_func(i32) #5

; Function Attrs: nounwind
declare void @llvm.va_end(i8*) #4

declare i32 @__stdio_common_vfprintf(i64, %struct._iobuf*, i8*, %struct.__crt_locale_pointers*, i8*) #5

; Function Attrs: inlinehint noinline nounwind uwtable
define linkonce_odr i64* @__local_stdio_printf_options() #6 comdat {
entry:
  ret i64* @"\01?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA"
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { inlinehint nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { norecurse nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }
attributes #5 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { inlinehint noinline nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !3}
!llvm.ident = !{!4}

!0 = !{i32 6, !"Linker Options", !1}
!1 = !{!2}
!2 = !{!"/FAILIFMISMATCH:\22_CRT_STDIO_ISO_WIDE_SPECIFIERS=0\22"}
!3 = !{i32 1, !"PIC Level", i32 2}
!4 = !{!"clang version 3.8.0 (branches/release_38)"}
