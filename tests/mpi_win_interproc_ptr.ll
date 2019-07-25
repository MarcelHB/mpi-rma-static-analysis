; ModuleID = 'mpi_win_interproc_ptr.cpp'
source_filename = "mpi_win_interproc_ptr.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline optnone uwtable
define void @_Z13create_windowPi(i32*) #0 {
  %2 = alloca i32*, align 8
  store i32* %0, i32** %2, align 8
  %3 = load i32*, i32** %2, align 8
  %4 = call i32 @MPI_Win_create(i8* null, i64 0, i32 0, i32 469762048, i32 1140850688, i32* %3)
  ret void
}

declare i32 @MPI_Win_create(i8*, i64, i32, i32, i32, i32*) #1

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main(i32, i8**) #2 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  %8 = call i32 @MPI_Init(i32* %4, i8*** %5)
  store i32 1, i32* %7, align 4
  call void @_Z13create_windowPi(i32* %6)
  %9 = load i32, i32* %6, align 4
  %10 = call i32 @MPI_Win_fence(i32 0, i32 %9)
  %11 = bitcast i32* %7 to i8*
  %12 = load i32, i32* %6, align 4
  %13 = call i32 @MPI_Get(i8* %11, i32 1, i32 1275069445, i32 0, i64 0, i32 1, i32 1275069445, i32 %12)
  %14 = call i32 @MPI_Win_free(i32* %6)
  %15 = call i32 @MPI_Finalize()
  ret i32 0
}

declare i32 @MPI_Init(i32*, i8***) #1

declare i32 @MPI_Win_fence(i32, i32) #1

declare i32 @MPI_Get(i8*, i32, i32, i32, i64, i32, i32, i32) #1

declare i32 @MPI_Win_free(i32*) #1

declare i32 @MPI_Finalize() #1

attributes #0 = { noinline optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
