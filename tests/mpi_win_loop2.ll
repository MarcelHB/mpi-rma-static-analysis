; ModuleID = 'mpi_win_loop2.cpp'
source_filename = "mpi_win_loop2.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main(i32, i8**) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i64, align 8
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i64, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  %11 = call i32 @MPI_Init(i32* %4, i8*** %5)
  store i64 0, i64* %6, align 8
  store i32 -1, i32* %8, align 4
  %12 = call i32 @MPI_Comm_rank(i32 1140850688, i32* %8)
  %13 = load i32, i32* %8, align 4
  %14 = load i32, i32* %8, align 4
  %15 = call i32 @MPI_Comm_split(i32 1140850688, i32 %13, i32 %14, i32* %7)
  %16 = bitcast i64* %6 to i8*
  %17 = load i32, i32* %7, align 4
  %18 = call i32 @MPI_Win_create(i8* %16, i64 8, i32 1, i32 469762048, i32 %17, i32* %9)
  %19 = load i32, i32* %8, align 4
  %20 = icmp ne i32 %19, 0
  br i1 %20, label %21, label %27

; <label>:21:                                     ; preds = %2
  %22 = bitcast i32* %8 to i8*
  %23 = load i32, i32* %8, align 4
  %24 = sext i32 %23 to i64
  %25 = load i32, i32* %9, align 4
  %26 = call i32 @MPI_Get(i8* %22, i32 1, i32 1275069445, i32 0, i64 %24, i32 1, i32 1275069445, i32 %25)
  br label %27

; <label>:27:                                     ; preds = %21, %2
  %28 = load i32, i32* %9, align 4
  %29 = call i32 @MPI_Win_fence(i32 0, i32 %28)
  store i64 0, i64* %10, align 8
  br label %30

; <label>:30:                                     ; preds = %48, %27
  %31 = load i64, i64* %10, align 8
  %32 = icmp ult i64 %31, 3
  br i1 %32, label %33, label %51

; <label>:33:                                     ; preds = %30
  %34 = load i32, i32* %8, align 4
  %35 = icmp sgt i32 %34, 0
  br i1 %35, label %36, label %42

; <label>:36:                                     ; preds = %33
  %37 = bitcast i32* %8 to i8*
  %38 = load i32, i32* %8, align 4
  %39 = sext i32 %38 to i64
  %40 = load i32, i32* %9, align 4
  %41 = call i32 @MPI_Get(i8* %37, i32 1, i32 1275069445, i32 0, i64 %39, i32 1, i32 1275069445, i32 %40)
  br label %48

; <label>:42:                                     ; preds = %33
  %43 = bitcast i32* %8 to i8*
  %44 = load i32, i32* %8, align 4
  %45 = sext i32 %44 to i64
  %46 = load i32, i32* %9, align 4
  %47 = call i32 @MPI_Put(i8* %43, i32 1, i32 1275069445, i32 0, i64 %45, i32 1, i32 1275069445, i32 %46)
  br label %48

; <label>:48:                                     ; preds = %42, %36
  %49 = load i64, i64* %10, align 8
  %50 = add i64 %49, 1
  store i64 %50, i64* %10, align 8
  br label %30

; <label>:51:                                     ; preds = %30
  %52 = load i64, i64* %10, align 8
  %53 = icmp ne i64 0, %52
  br i1 %53, label %54, label %57

; <label>:54:                                     ; preds = %51
  %55 = load i32, i32* %9, align 4
  %56 = call i32 @MPI_Win_fence(i32 0, i32 %55)
  br label %57

; <label>:57:                                     ; preds = %54, %51
  %58 = call i32 @MPI_Win_free(i32* %9)
  %59 = call i32 @MPI_Comm_free(i32* %7)
  %60 = call i32 @MPI_Finalize()
  ret i32 0
}

declare i32 @MPI_Init(i32*, i8***) #1

declare i32 @MPI_Comm_rank(i32, i32*) #1

declare i32 @MPI_Comm_split(i32, i32, i32, i32*) #1

declare i32 @MPI_Win_create(i8*, i64, i32, i32, i32, i32*) #1

declare i32 @MPI_Get(i8*, i32, i32, i32, i64, i32, i32, i32) #1

declare i32 @MPI_Win_fence(i32, i32) #1

declare i32 @MPI_Put(i8*, i32, i32, i32, i64, i32, i32, i32) #1

declare i32 @MPI_Win_free(i32*) #1

declare i32 @MPI_Comm_free(i32*) #1

declare i32 @MPI_Finalize() #1

attributes #0 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
