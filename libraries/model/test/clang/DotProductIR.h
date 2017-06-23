// IR code for dot product
const char* GetDotProductFunctionName()
{
    return "_Node__DotProduct";
}
const char* GetDotProductIR()
{
    return R"xx(
; ModuleID = 'DotProduct.cpp'
source_filename = "DotProduct.cpp"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

; Function Attrs: nounwind ssp uwtable
define void @_Node__DotProduct(double*, double*, i32, double*) #0 {
  %5 = alloca double*, align 8
  %6 = alloca double*, align 8
  %7 = alloca i32, align 4
  %8 = alloca double*, align 8
  %9 = alloca double, align 8
  %10 = alloca i32, align 4
  store double* %0, double** %5, align 8
  store double* %1, double** %6, align 8
  store i32 %2, i32* %7, align 4
  store double* %3, double** %8, align 8
  store double 0.000000e+00, double* %9, align 8
  store i32 0, i32* %10, align 4
  br label %11

; <label>:11                                      ; preds = %29, %4
  %12 = load i32, i32* %10, align 4
  %13 = load i32, i32* %7, align 4
  %14 = icmp slt i32 %12, %13
  br i1 %14, label %15, label %32

; <label>:15                                      ; preds = %11
  %16 = load i32, i32* %10, align 4
  %17 = sext i32 %16 to i64
  %18 = load double*, double** %5, align 8
  %19 = getelementptr inbounds double, double* %18, i64 %17
  %20 = load double, double* %19, align 8
  %21 = load i32, i32* %10, align 4
  %22 = sext i32 %21 to i64
  %23 = load double*, double** %6, align 8
  %24 = getelementptr inbounds double, double* %23, i64 %22
  %25 = load double, double* %24, align 8
  %26 = fmul double %20, %25
  %27 = load double, double* %9, align 8
  %28 = fadd double %27, %26
  store double %28, double* %9, align 8
  br label %29

; <label>:29                                      ; preds = %15
  %30 = load i32, i32* %10, align 4
  %31 = add nsw i32 %30, 1
  store i32 %31, i32* %10, align 4
  br label %11

; <label>:32                                      ; preds = %11
  %33 = load double, double* %9, align 8
  %34 = load double*, double** %8, align 8
  %35 = getelementptr inbounds double, double* %34, i64 0
  store double %33, double* %35, align 8
  ret void
}

attributes #0 = { nounwind ssp uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"Apple LLVM version 8.0.0 (clang-800.0.42.1)"}
)xx";
}
