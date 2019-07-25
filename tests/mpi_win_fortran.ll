; ModuleID = '/tmp/mpi_win_fortran-d8c09d.ll'
source_filename = "/tmp/mpi_win_fortran-d8c09d.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.STATICS1 = type <{ [20 x i8] }>
%structmpifcmb5_ = type <{ [4 x i8] }>
%structmpifcmb9_ = type <{ [4 x i8] }>
%structmpipriv1_ = type <{ [28 x i8] }>
%structmpipriv2_ = type <{ [24 x i8] }>
%structmpiprivc_ = type <{ [2 x i8] }>

@.STATICS1 = internal global %struct.STATICS1 <{ [20 x i8] c"\00\00\00\00\00\00\00\00\FF\FF\FF\FF\00\00\00\00\00\00\00\00" }>, align 16
@.C304_MAIN_ = internal constant i32 10
@.C307_MAIN_ = internal constant i32 1275069467
@.C285_MAIN_ = internal constant i32 1
@.C306_MAIN_ = internal constant i32 469762048
@.C300_MAIN_ = internal constant i32 4
@.C305_MAIN_ = internal constant i32 1140850688
@.C283_MAIN_ = internal constant i32 0
@mpifcmb5_ = common global %structmpifcmb5_ zeroinitializer, align 64
@mpifcmb9_ = common global %structmpifcmb9_ zeroinitializer, align 64
@mpipriv1_ = common global %structmpipriv1_ zeroinitializer, align 64
@mpipriv2_ = common global %structmpipriv2_ zeroinitializer, align 64
@mpiprivc_ = common global %structmpiprivc_ zeroinitializer, align 64

define void @MAIN_() !dbg !5 {
L.entry:
  %.dY0001_383 = alloca i32, align 4
  %i_373 = alloca i32, align 4
  %.dY0002_386 = alloca i32, align 4
  %j_374 = alloca i32, align 4
  %0 = bitcast i32* @.C283_MAIN_ to i8*, !dbg !8
  %1 = bitcast void (...)* @fort_init to void (i8*, ...)*, !dbg !8
  call void (i8*, ...) %1(i8* %0), !dbg !8
  br label %L.LB1_388

L.LB1_388:                                        ; preds = %L.entry
  %2 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !10
  %3 = getelementptr i8, i8* %2, i64 4, !dbg !10
  %4 = bitcast i8* %3 to i64*, !dbg !10
  call void @mpi_init_(i64* %4), !dbg !10
  %5 = bitcast i32* @.C305_MAIN_ to i64*, !dbg !11
  %6 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !11
  %7 = getelementptr i8, i8* %6, i64 8, !dbg !11
  %8 = bitcast i8* %7 to i64*, !dbg !11
  %9 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !11
  %10 = getelementptr i8, i8* %9, i64 4, !dbg !11
  %11 = bitcast i8* %10 to i64*, !dbg !11
  call void @mpi_comm_rank_(i64* %5, i64* %8, i64* %11), !dbg !11
  %12 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !12
  %13 = getelementptr i8, i8* %12, i64 12, !dbg !12
  %14 = bitcast i32* @.C300_MAIN_ to i8*, !dbg !12
  %15 = bitcast i32* @.C300_MAIN_ to i8*, !dbg !12
  %16 = bitcast i32* @.C306_MAIN_ to i8*, !dbg !12
  %17 = bitcast i32* @.C305_MAIN_ to i8*, !dbg !12
  %18 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !12
  %19 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !12
  %20 = getelementptr i8, i8* %19, i64 4, !dbg !12
  %21 = bitcast void (...)* @mpi_win_create_ to void (i8*, i8*, i8*, i8*, i8*, i8*, i8*, ...)*, !dbg !12
  call void (i8*, i8*, i8*, i8*, i8*, i8*, i8*, ...) %21(i8* %13, i8* %14, i8* %15, i8* %16, i8* %17, i8* %18, i8* %20), !dbg !12
  %22 = bitcast i32* @.C305_MAIN_ to i64*, !dbg !13
  %23 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !13
  %24 = getelementptr i8, i8* %23, i64 4, !dbg !13
  %25 = bitcast i8* %24 to i64*, !dbg !13
  call void @mpi_barrier_(i64* %22, i64* %25), !dbg !13
  %26 = bitcast i32* @.C283_MAIN_ to i64*, !dbg !14
  %27 = bitcast %struct.STATICS1* @.STATICS1 to i64*, !dbg !14
  %28 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !14
  %29 = getelementptr i8, i8* %28, i64 4, !dbg !14
  %30 = bitcast i8* %29 to i64*, !dbg !14
  call void @mpi_win_fence_(i64* %26, i64* %27, i64* %30), !dbg !14
  %31 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !15
  %32 = getelementptr i8, i8* %31, i64 8, !dbg !15
  %33 = bitcast i8* %32 to i32*, !dbg !15
  %34 = load i32, i32* %33, align 4, !dbg !15
  %35 = icmp ne i32 %34, 0, !dbg !15
  br i1 %35, label %L.LB1_379, label %L.LB1_411, !dbg !15

L.LB1_411:                                        ; preds = %L.LB1_388
  %36 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !16
  %37 = getelementptr i8, i8* %36, i64 16, !dbg !16
  %38 = bitcast i32* @.C285_MAIN_ to i8*, !dbg !16
  %39 = bitcast i32* @.C307_MAIN_ to i8*, !dbg !16
  %40 = bitcast i32* @.C285_MAIN_ to i8*, !dbg !16
  %41 = bitcast i32* @.C283_MAIN_ to i8*, !dbg !16
  %42 = bitcast i32* @.C285_MAIN_ to i8*, !dbg !16
  %43 = bitcast i32* @.C307_MAIN_ to i8*, !dbg !16
  %44 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !16
  %45 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !16
  %46 = getelementptr i8, i8* %45, i64 4, !dbg !16
  %47 = bitcast void (...)* @mpi_get_ to void (i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, ...)*, !dbg !16
  call void (i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, ...) %47(i8* %37, i8* %38, i8* %39, i8* %40, i8* %41, i8* %42, i8* %43, i8* %44, i8* %46), !dbg !16
  %48 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !17
  %49 = getelementptr i8, i8* %48, i64 16, !dbg !17
  %50 = bitcast i8* %49 to i32*, !dbg !17
  %51 = load i32, i32* %50, align 4, !dbg !17
  %52 = add nsw i32 %51, 1, !dbg !17
  %53 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !17
  %54 = getelementptr i8, i8* %53, i64 16, !dbg !17
  %55 = bitcast i8* %54 to i32*, !dbg !17
  store i32 %52, i32* %55, align 4, !dbg !17
  br label %L.LB1_380, !dbg !18

L.LB1_379:                                        ; preds = %L.LB1_388
  store i32 10, i32* %.dY0001_383, align 4, !dbg !19
  store i32 1, i32* %i_373, align 4, !dbg !19
  br label %L.LB1_381

L.LB1_381:                                        ; preds = %L.LB1_385, %L.LB1_379
  %56 = load i32, i32* %i_373, align 4, !dbg !20
  %57 = add nsw i32 %56, 11, !dbg !20
  %58 = load i32, i32* %i_373, align 4, !dbg !20
  %59 = sub nsw i32 %57, %58, !dbg !20
  store i32 %59, i32* %.dY0002_386, align 4, !dbg !20
  %60 = load i32, i32* %i_373, align 4, !dbg !20
  store i32 %60, i32* %j_374, align 4, !dbg !20
  %61 = load i32, i32* %.dY0002_386, align 4, !dbg !20
  %62 = icmp sle i32 %61, 0, !dbg !20
  br i1 %62, label %L.LB1_385, label %L.LB1_384, !dbg !20

L.LB1_384:                                        ; preds = %L.LB1_384, %L.LB1_381
  %63 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !21
  %64 = getelementptr i8, i8* %63, i64 16, !dbg !21
  %65 = bitcast i32* @.C285_MAIN_ to i8*, !dbg !21
  %66 = bitcast i32* @.C307_MAIN_ to i8*, !dbg !21
  %67 = bitcast i32* @.C283_MAIN_ to i8*, !dbg !21
  %68 = bitcast i32* @.C283_MAIN_ to i8*, !dbg !21
  %69 = bitcast i32* @.C285_MAIN_ to i8*, !dbg !21
  %70 = bitcast i32* @.C307_MAIN_ to i8*, !dbg !21
  %71 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !21
  %72 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !21
  %73 = getelementptr i8, i8* %72, i64 4, !dbg !21
  %74 = bitcast void (...)* @mpi_put_ to void (i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, ...)*, !dbg !21
  call void (i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, ...) %74(i8* %64, i8* %65, i8* %66, i8* %67, i8* %68, i8* %69, i8* %70, i8* %71, i8* %73), !dbg !21
  %75 = load i32, i32* %j_374, align 4, !dbg !22
  %76 = add nsw i32 %75, 1, !dbg !22
  store i32 %76, i32* %j_374, align 4, !dbg !22
  %77 = load i32, i32* %.dY0002_386, align 4, !dbg !22
  %78 = sub nsw i32 %77, 1, !dbg !22
  store i32 %78, i32* %.dY0002_386, align 4, !dbg !22
  %79 = load i32, i32* %.dY0002_386, align 4, !dbg !22
  %80 = icmp sgt i32 %79, 0, !dbg !22
  br i1 %80, label %L.LB1_384, label %L.LB1_385, !dbg !22

L.LB1_385:                                        ; preds = %L.LB1_384, %L.LB1_381
  %81 = load i32, i32* %i_373, align 4, !dbg !23
  %82 = add nsw i32 %81, 1, !dbg !23
  store i32 %82, i32* %i_373, align 4, !dbg !23
  %83 = load i32, i32* %.dY0001_383, align 4, !dbg !23
  %84 = sub nsw i32 %83, 1, !dbg !23
  store i32 %84, i32* %.dY0001_383, align 4, !dbg !23
  %85 = load i32, i32* %.dY0001_383, align 4, !dbg !23
  %86 = icmp sgt i32 %85, 0, !dbg !23
  br i1 %86, label %L.LB1_381, label %L.LB1_380, !dbg !23

L.LB1_380:                                        ; preds = %L.LB1_385, %L.LB1_411
  %87 = bitcast i32* @.C283_MAIN_ to i64*, !dbg !24
  %88 = bitcast %struct.STATICS1* @.STATICS1 to i64*, !dbg !24
  %89 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !24
  %90 = getelementptr i8, i8* %89, i64 4, !dbg !24
  %91 = bitcast i8* %90 to i64*, !dbg !24
  call void @mpi_win_fence_(i64* %87, i64* %88, i64* %91), !dbg !24
  %92 = bitcast %struct.STATICS1* @.STATICS1 to i64*, !dbg !25
  %93 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !25
  %94 = getelementptr i8, i8* %93, i64 4, !dbg !25
  %95 = bitcast i8* %94 to i64*, !dbg !25
  call void @mpi_win_free_(i64* %92, i64* %95), !dbg !25
  %96 = bitcast %struct.STATICS1* @.STATICS1 to i8*, !dbg !26
  %97 = getelementptr i8, i8* %96, i64 4, !dbg !26
  %98 = bitcast i8* %97 to i64*, !dbg !26
  call void @mpi_finalize_(i64* %98), !dbg !26
  ret void, !dbg !27
}

declare void @mpi_put_(...)

declare void @mpi_get_(...)

declare void @mpi_win_create_(...)

declare void @fort_init(...)

declare void @mpi_init_(i64*)

declare void @mpi_barrier_(i64*, i64*)

declare void @mpi_comm_rank_(i64*, i64*, i64*)

declare void @mpi_win_free_(i64*, i64*)

declare void @mpi_finalize_(i64*)

declare void @mpi_win_fence_(i64*, i64*, i64*)

!llvm.module.flags = !{!0, !1}
!llvm.dbg.cu = !{!2}

!0 = !{i32 2, !"Dwarf Version", i32 4}
!1 = !{i32 2, !"Debug Info Version", i32 3}
!2 = distinct !DICompileUnit(language: DW_LANG_Fortran90, file: !3, producer: " F90 Flang - 1.5 2017-05-01", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, retainedTypes: !4, globals: !4, imports: !4)
!3 = !DIFile(filename: "mpi_win_fortran.f90", directory: "/home/marcelhb/Source/master_docs/passes/phasar/tests")
!4 = !{}
!5 = distinct !DISubprogram(name: "main", scope: !2, file: !3, line: 1, type: !6, isLocal: false, isDefinition: true, scopeLine: 1, flags: DIFlagMainSubprogram, isOptimized: false, unit: !2, variables: !4)
!6 = !DISubroutineType(cc: DW_CC_program, types: !7)
!7 = !{null}
!8 = !DILocation(line: 1, column: 1, scope: !9)
!9 = !DILexicalBlock(scope: !5, file: !3, line: 1, column: 1)
!10 = !DILocation(line: 10, column: 1, scope: !9)
!11 = !DILocation(line: 11, column: 1, scope: !9)
!12 = !DILocation(line: 13, column: 1, scope: !9)
!13 = !DILocation(line: 15, column: 1, scope: !9)
!14 = !DILocation(line: 17, column: 1, scope: !9)
!15 = !DILocation(line: 19, column: 1, scope: !9)
!16 = !DILocation(line: 20, column: 1, scope: !9)
!17 = !DILocation(line: 21, column: 1, scope: !9)
!18 = !DILocation(line: 22, column: 1, scope: !9)
!19 = !DILocation(line: 23, column: 1, scope: !9)
!20 = !DILocation(line: 24, column: 1, scope: !9)
!21 = !DILocation(line: 25, column: 1, scope: !9)
!22 = !DILocation(line: 26, column: 1, scope: !9)
!23 = !DILocation(line: 27, column: 1, scope: !9)
!24 = !DILocation(line: 30, column: 1, scope: !9)
!25 = !DILocation(line: 32, column: 1, scope: !9)
!26 = !DILocation(line: 34, column: 1, scope: !9)
!27 = !DILocation(line: 35, column: 1, scope: !9)
