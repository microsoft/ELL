; ModuleID = 'ELL'
source_filename = "ELL"

define void @predict(double* %input0, double* %output0) {
entry:
  %0 = getelementptr double, double* %input0, i32 0
  %1 = load double, double* %0
  %2 = getelementptr double, double* %output0, i32 0
  store double %1, double* %2
  %3 = getelementptr double, double* %input0, i32 1
  %4 = load double, double* %3
  %5 = getelementptr double, double* %output0, i32 1
  store double %4, double* %5
  %6 = getelementptr double, double* %input0, i32 2
  %7 = load double, double* %6
  %8 = getelementptr double, double* %output0, i32 2
  store double %7, double* %8
  ret void
}
