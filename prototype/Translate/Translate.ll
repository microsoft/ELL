; ModuleID = 'C:\umesh\Projects\Native\Translate\Translate.cpp'
target datalayout = "e-m:w-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc18.0.0"

%struct._iobuf = type { i8* }
%struct.__crt_locale_pointers = type { %struct.__crt_locale_data*, %struct.__crt_multibyte_data* }
%struct.__crt_locale_data = type opaque
%struct.__crt_multibyte_data = type opaque

$printf = comdat any

$_vfprintf_l = comdat any

$__local_stdio_printf_options = comdat any

$"\01??_C@_07IDDJFNOO@Data?5?$CFf?$AA@" = comdat any

$"\01?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA" = comdat any

@"\01??_C@_07IDDJFNOO@Data?5?$CFf?$AA@" = linkonce_odr unnamed_addr constant [8 x i8] c"Data %f\00", comdat, align 1
@"\01?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA" = linkonce_odr global i64 0, comdat, align 8

; Function Attrs: nounwind uwtable
define double @"\01?Predict@@YANPEAN@Z"(double* %features) #0 {
entry:
  %features.addr = alloca double*, align 8
  %retVal = alloca double, align 8
  store double* %features, double** %features.addr, align 8
  store double 0.000000e+00, double* %retVal, align 8
  %0 = load double*, double** %features.addr, align 8
  %arrayidx = getelementptr inbounds double, double* %0, i64 0
  %1 = load double, double* %arrayidx, align 8
  %mul = fmul double -3.250000e-01, %1
  %2 = load double, double* %retVal, align 8
  %add = fadd double %2, %mul
  store double %add, double* %retVal, align 8
  %3 = load double*, double** %features.addr, align 8
  %arrayidx1 = getelementptr inbounds double, double* %3, i64 1
  %4 = load double, double* %arrayidx1, align 8
  %mul2 = fmul double 5.775000e-01, %4
  %5 = load double, double* %retVal, align 8
  %add3 = fadd double %5, %mul2
  store double %add3, double* %retVal, align 8
  %6 = load double*, double** %features.addr, align 8
  %arrayidx4 = getelementptr inbounds double, double* %6, i64 2
  %7 = load double, double* %arrayidx4, align 8
  %mul5 = fmul double -9.350000e-01, %7
  %8 = load double, double* %retVal, align 8
  %add6 = fadd double %8, %mul5
  store double %add6, double* %retVal, align 8
  %9 = load double*, double** %features.addr, align 8
  %arrayidx7 = getelementptr inbounds double, double* %9, i64 3
  %10 = load double, double* %arrayidx7, align 8
  %mul8 = fmul double 3.325000e+00, %10
  %11 = load double, double* %retVal, align 8
  %add9 = fadd double %11, %mul8
  store double %add9, double* %retVal, align 8
  %12 = load double, double* %retVal, align 8
  ret double %12
}

; Function Attrs: norecurse nounwind uwtable
define i32 @main() #1 {
entry:
  %features = alloca double*, align 8
  %call = call noalias i8* @malloc(i64 32)
  %0 = bitcast i8* %call to double*
  store double* %0, double** %features, align 8
  %1 = load double*, double** %features, align 8
  %arrayidx = getelementptr inbounds double, double* %1, i64 1
  store double 9.900000e+00, double* %arrayidx, align 8
  %2 = load double*, double** %features, align 8
  %arrayidx1 = getelementptr inbounds double, double* %2, i64 1
  %3 = load double, double* %arrayidx1, align 8
  %call2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @"\01??_C@_07IDDJFNOO@Data?5?$CFf?$AA@", i32 0, i32 0), double %3)
  %4 = load double*, double** %features, align 8
  %5 = bitcast double* %4 to i8*
  call void @free(i8* %5)
  ret i32 0
}

declare noalias i8* @malloc(i64) #2

; Function Attrs: inlinehint nounwind uwtable
define linkonce_odr i32 @printf(i8* %_Format, ...) #3 comdat {
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

declare void @free(i8*) #2

; Function Attrs: nounwind
declare void @llvm.va_start(i8*) #4

; Function Attrs: inlinehint nounwind uwtable
define linkonce_odr i32 @_vfprintf_l(%struct._iobuf* %_Stream, i8* %_Format, %struct.__crt_locale_pointers* %_Locale, i8* %_ArgList) #3 comdat {
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

declare %struct._iobuf* @__acrt_iob_func(i32) #2

; Function Attrs: nounwind
declare void @llvm.va_end(i8*) #4

declare i32 @__stdio_common_vfprintf(i64, %struct._iobuf*, i8*, %struct.__crt_locale_pointers*, i8*) #2

; Function Attrs: inlinehint noinline nounwind uwtable
define linkonce_odr i64* @__local_stdio_printf_options() #5 comdat {
entry:
  ret i64* @"\01?_OptionsStorage@?1??__local_stdio_printf_options@@9@4_KA"
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { norecurse nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { inlinehint nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }
attributes #5 = { inlinehint noinline nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !3}
!llvm.ident = !{!4}

!0 = !{i32 6, !"Linker Options", !1}
!1 = !{!2}
!2 = !{!"/FAILIFMISMATCH:\22_CRT_STDIO_ISO_WIDE_SPECIFIERS=0\22"}
!3 = !{i32 1, !"PIC Level", i32 2}
!4 = !{!"clang version 3.8.0 (branches/release_38)"}
