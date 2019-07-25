; ModuleID = 'switch.cpp'
source_filename = "switch.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline norecurse nounwind optnone uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i64, align 8
  store i32 0, i32* %1, align 4
  store i64 0, i64* %2, align 8
  %3 = load i64, i64* %2, align 8
  switch i64 %3, label %8 [
    i64 0, label %4
    i64 1, label %5
  ]

; <label>:4:                                      ; preds = %0
  store i64 1, i64* %2, align 8
  br label %11

; <label>:5:                                      ; preds = %0
  %6 = load i64, i64* %2, align 8
  %7 = add i64 %6, 1
  store i64 %7, i64* %2, align 8
  br label %11

; <label>:8:                                      ; preds = %0
  %9 = load i64, i64* %2, align 8
  %10 = mul i64 %9, 2
  store i64 %10, i64* %2, align 8
  br label %11

; <label>:11:                                     ; preds = %8, %5, %4
  ret i32 0
}

attributes #0 = { noinline norecurse nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
