; ModuleID = 'mpi_win_interproc_comm.cpp'
source_filename = "mpi_win_interproc_comm.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

$_Z13create_windowImEiiPT_b = comdat any

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main(i32, i8**) #0 !dbg !93 {
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
  call void @llvm.dbg.declare(metadata i32* %4, metadata !99, metadata !DIExpression()), !dbg !100
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !101, metadata !DIExpression()), !dbg !102
  %11 = call i32 @MPI_Init(i32* %4, i8*** %5), !dbg !103
  call void @llvm.dbg.declare(metadata i64* %6, metadata !104, metadata !DIExpression()), !dbg !107
  store i64 0, i64* %6, align 8, !dbg !107
  call void @llvm.dbg.declare(metadata i32* %7, metadata !108, metadata !DIExpression()), !dbg !109
  call void @llvm.dbg.declare(metadata i32* %8, metadata !110, metadata !DIExpression()), !dbg !111
  store i32 -1, i32* %8, align 4, !dbg !111
  %12 = call i32 @MPI_Comm_rank(i32 1140850688, i32* %8), !dbg !112
  %13 = load i32, i32* %8, align 4, !dbg !113
  %14 = load i32, i32* %8, align 4, !dbg !114
  %15 = call i32 @MPI_Comm_split(i32 1140850688, i32 %13, i32 %14, i32* %7), !dbg !115
  call void @llvm.dbg.declare(metadata i32* %9, metadata !116, metadata !DIExpression()), !dbg !118
  %16 = load i32, i32* %7, align 4, !dbg !119
  %17 = call i32 @_Z13create_windowImEiiPT_b(i32 %16, i64* %6, i1 zeroext false), !dbg !120
  store i32 %17, i32* %9, align 4, !dbg !118
  %18 = load i64, i64* %6, align 8, !dbg !121
  %19 = icmp ugt i64 %18, 0, !dbg !123
  br i1 %19, label %20, label %26, !dbg !124

; <label>:20:                                     ; preds = %2
  %21 = bitcast i32* %8 to i8*, !dbg !125
  %22 = load i32, i32* %8, align 4, !dbg !127
  %23 = sext i32 %22 to i64, !dbg !127
  %24 = load i32, i32* %9, align 4, !dbg !128
  %25 = call i32 @MPI_Get(i8* %21, i32 1, i32 1275069445, i32 0, i64 %23, i32 1, i32 1275069445, i32 %24), !dbg !129
  br label %26, !dbg !130

; <label>:26:                                     ; preds = %20, %2
  %27 = load i32, i32* %9, align 4, !dbg !131
  %28 = call i32 @MPI_Win_fence(i32 0, i32 %27), !dbg !132
  call void @llvm.dbg.declare(metadata i64* %10, metadata !133, metadata !DIExpression()), !dbg !134
  store i64 0, i64* %10, align 8, !dbg !134
  br label %29, !dbg !135, !llvm.loop !136

; <label>:29:                                     ; preds = %37, %26
  %30 = bitcast i32* %8 to i8*, !dbg !138
  %31 = load i32, i32* %8, align 4, !dbg !140
  %32 = sext i32 %31 to i64, !dbg !140
  %33 = load i32, i32* %9, align 4, !dbg !141
  %34 = call i32 @MPI_Get(i8* %30, i32 1, i32 1275069445, i32 0, i64 %32, i32 1, i32 1275069445, i32 %33), !dbg !142
  %35 = load i64, i64* %10, align 8, !dbg !143
  %36 = add i64 %35, 1, !dbg !143
  store i64 %36, i64* %10, align 8, !dbg !143
  br label %37, !dbg !144

; <label>:37:                                     ; preds = %29
  %38 = load i64, i64* %10, align 8, !dbg !145
  %39 = icmp ult i64 %38, 3, !dbg !146
  br i1 %39, label %29, label %40, !dbg !144, !llvm.loop !136

; <label>:40:                                     ; preds = %37
  %41 = load i64, i64* %10, align 8, !dbg !147
  %42 = icmp ne i64 0, %41, !dbg !149
  br i1 %42, label %43, label %46, !dbg !150

; <label>:43:                                     ; preds = %40
  %44 = load i32, i32* %9, align 4, !dbg !151
  %45 = call i32 @MPI_Win_fence(i32 0, i32 %44), !dbg !153
  br label %46, !dbg !154

; <label>:46:                                     ; preds = %43, %40
  %47 = call i32 @MPI_Win_free(i32* %9), !dbg !155
  %48 = call i32 @MPI_Comm_free(i32* %7), !dbg !156
  %49 = call i32 @MPI_Finalize(), !dbg !157
  ret i32 0, !dbg !158
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @MPI_Init(i32*, i8***) #2

declare i32 @MPI_Comm_rank(i32, i32*) #2

declare i32 @MPI_Comm_split(i32, i32, i32, i32*) #2

; Function Attrs: noinline optnone uwtable
define linkonce_odr i32 @_Z13create_windowImEiiPT_b(i32, i64*, i1 zeroext) #3 comdat !dbg !159 {
  %4 = alloca i32, align 4
  %5 = alloca i64*, align 8
  %6 = alloca i8, align 1
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !166, metadata !DIExpression()), !dbg !167
  store i64* %1, i64** %5, align 8
  call void @llvm.dbg.declare(metadata i64** %5, metadata !168, metadata !DIExpression()), !dbg !169
  %9 = zext i1 %2 to i8
  store i8 %9, i8* %6, align 1
  call void @llvm.dbg.declare(metadata i8* %6, metadata !170, metadata !DIExpression()), !dbg !171
  call void @llvm.dbg.declare(metadata i32* %7, metadata !172, metadata !DIExpression()), !dbg !173
  call void @llvm.dbg.declare(metadata i32* %8, metadata !174, metadata !DIExpression()), !dbg !175
  %10 = load i32, i32* %4, align 4, !dbg !176
  store i32 %10, i32* %8, align 4, !dbg !175
  %11 = load i64*, i64** %5, align 8, !dbg !177
  %12 = bitcast i64* %11 to i8*, !dbg !177
  %13 = load i32, i32* %8, align 4, !dbg !178
  %14 = call i32 @MPI_Win_create(i8* %12, i64 8, i32 1, i32 469762048, i32 %13, i32* %7), !dbg !179
  %15 = load i32, i32* %7, align 4, !dbg !180
  ret i32 %15, !dbg !181
}

declare i32 @MPI_Get(i8*, i32, i32, i32, i64, i32, i32, i32) #2

declare i32 @MPI_Win_fence(i32, i32) #2

declare i32 @MPI_Win_free(i32*) #2

declare i32 @MPI_Comm_free(i32*) #2

declare i32 @MPI_Finalize() #2

declare i32 @MPI_Win_create(i8*, i64, i32, i32, i32, i32*) #2

attributes #0 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noinline optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!89, !90, !91}
!llvm.ident = !{!92}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3, imports: !9)
!1 = !DIFile(filename: "mpi_win_interproc_comm.cpp", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!2 = !{}
!3 = !{!4, !7, !8}
!4 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Comm", file: !5, line: 287, baseType: !6)
!5 = !DIFile(filename: "/usr/include/mpich/mpi.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Datatype", file: !5, line: 104, baseType: !6)
!8 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Info", file: !5, line: 528, baseType: !6)
!9 = !{!10, !18, !22, !25, !29, !32, !34, !36, !38, !40, !42, !44, !46, !49, !51, !56, !60, !64, !68, !70, !72, !74, !76, !78, !80, !82, !84, !87}
!10 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !12, file: !17, line: 48)
!11 = !DINamespace(name: "std", scope: null)
!12 = !DIDerivedType(tag: DW_TAG_typedef, name: "int8_t", file: !13, line: 24, baseType: !14)
!13 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!14 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int8_t", file: !15, line: 36, baseType: !16)
!15 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/types.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!16 = !DIBasicType(name: "signed char", size: 8, encoding: DW_ATE_signed_char)
!17 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/cstdint", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!18 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !19, file: !17, line: 49)
!19 = !DIDerivedType(tag: DW_TAG_typedef, name: "int16_t", file: !13, line: 25, baseType: !20)
!20 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int16_t", file: !15, line: 38, baseType: !21)
!21 = !DIBasicType(name: "short", size: 16, encoding: DW_ATE_signed)
!22 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !23, file: !17, line: 50)
!23 = !DIDerivedType(tag: DW_TAG_typedef, name: "int32_t", file: !13, line: 26, baseType: !24)
!24 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int32_t", file: !15, line: 40, baseType: !6)
!25 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !26, file: !17, line: 51)
!26 = !DIDerivedType(tag: DW_TAG_typedef, name: "int64_t", file: !13, line: 27, baseType: !27)
!27 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int64_t", file: !15, line: 43, baseType: !28)
!28 = !DIBasicType(name: "long int", size: 64, encoding: DW_ATE_signed)
!29 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !30, file: !17, line: 53)
!30 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_fast8_t", file: !31, line: 68, baseType: !16)
!31 = !DIFile(filename: "/usr/include/stdint.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!32 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !33, file: !17, line: 54)
!33 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_fast16_t", file: !31, line: 70, baseType: !28)
!34 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !35, file: !17, line: 55)
!35 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_fast32_t", file: !31, line: 71, baseType: !28)
!36 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !37, file: !17, line: 56)
!37 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_fast64_t", file: !31, line: 72, baseType: !28)
!38 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !39, file: !17, line: 58)
!39 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_least8_t", file: !31, line: 43, baseType: !16)
!40 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !41, file: !17, line: 59)
!41 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_least16_t", file: !31, line: 44, baseType: !21)
!42 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !43, file: !17, line: 60)
!43 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_least32_t", file: !31, line: 45, baseType: !6)
!44 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !45, file: !17, line: 61)
!45 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_least64_t", file: !31, line: 47, baseType: !28)
!46 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !47, file: !17, line: 63)
!47 = !DIDerivedType(tag: DW_TAG_typedef, name: "intmax_t", file: !31, line: 111, baseType: !48)
!48 = !DIDerivedType(tag: DW_TAG_typedef, name: "__intmax_t", file: !15, line: 61, baseType: !28)
!49 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !50, file: !17, line: 64)
!50 = !DIDerivedType(tag: DW_TAG_typedef, name: "intptr_t", file: !31, line: 97, baseType: !28)
!51 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !52, file: !17, line: 66)
!52 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint8_t", file: !53, line: 24, baseType: !54)
!53 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/stdint-uintn.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!54 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !15, line: 37, baseType: !55)
!55 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!56 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !57, file: !17, line: 67)
!57 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint16_t", file: !53, line: 25, baseType: !58)
!58 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint16_t", file: !15, line: 39, baseType: !59)
!59 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!60 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !61, file: !17, line: 68)
!61 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint32_t", file: !53, line: 26, baseType: !62)
!62 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint32_t", file: !15, line: 41, baseType: !63)
!63 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!64 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !65, file: !17, line: 69)
!65 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint64_t", file: !53, line: 27, baseType: !66)
!66 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint64_t", file: !15, line: 44, baseType: !67)
!67 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!68 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !69, file: !17, line: 71)
!69 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_fast8_t", file: !31, line: 81, baseType: !55)
!70 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !71, file: !17, line: 72)
!71 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_fast16_t", file: !31, line: 83, baseType: !67)
!72 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !73, file: !17, line: 73)
!73 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_fast32_t", file: !31, line: 84, baseType: !67)
!74 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !75, file: !17, line: 74)
!75 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_fast64_t", file: !31, line: 85, baseType: !67)
!76 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !77, file: !17, line: 76)
!77 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_least8_t", file: !31, line: 54, baseType: !55)
!78 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !79, file: !17, line: 77)
!79 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_least16_t", file: !31, line: 55, baseType: !59)
!80 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !81, file: !17, line: 78)
!81 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_least32_t", file: !31, line: 56, baseType: !63)
!82 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !83, file: !17, line: 79)
!83 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_least64_t", file: !31, line: 58, baseType: !67)
!84 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !85, file: !17, line: 81)
!85 = !DIDerivedType(tag: DW_TAG_typedef, name: "uintmax_t", file: !31, line: 112, baseType: !86)
!86 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uintmax_t", file: !15, line: 62, baseType: !67)
!87 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !11, entity: !88, file: !17, line: 82)
!88 = !DIDerivedType(tag: DW_TAG_typedef, name: "uintptr_t", file: !31, line: 100, baseType: !67)
!89 = !{i32 2, !"Dwarf Version", i32 4}
!90 = !{i32 2, !"Debug Info Version", i32 3}
!91 = !{i32 1, !"wchar_size", i32 4}
!92 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
!93 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 20, type: !94, isLocal: false, isDefinition: true, scopeLine: 20, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!94 = !DISubroutineType(types: !95)
!95 = !{!6, !6, !96}
!96 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !97, size: 64)
!97 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !98, size: 64)
!98 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!99 = !DILocalVariable(name: "argc", arg: 1, scope: !93, file: !1, line: 20, type: !6)
!100 = !DILocation(line: 20, column: 15, scope: !93)
!101 = !DILocalVariable(name: "argv", arg: 2, scope: !93, file: !1, line: 20, type: !96)
!102 = !DILocation(line: 20, column: 28, scope: !93)
!103 = !DILocation(line: 21, column: 3, scope: !93)
!104 = !DILocalVariable(name: "memory", scope: !93, file: !1, line: 23, type: !105)
!105 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !106, line: 62, baseType: !67)
!106 = !DIFile(filename: "/usr/include/clang/6.0.0/include/stddef.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!107 = !DILocation(line: 23, column: 10, scope: !93)
!108 = !DILocalVariable(name: "other_comm", scope: !93, file: !1, line: 24, type: !4)
!109 = !DILocation(line: 24, column: 12, scope: !93)
!110 = !DILocalVariable(name: "rank", scope: !93, file: !1, line: 25, type: !6)
!111 = !DILocation(line: 25, column: 7, scope: !93)
!112 = !DILocation(line: 27, column: 3, scope: !93)
!113 = !DILocation(line: 31, column: 7, scope: !93)
!114 = !DILocation(line: 32, column: 7, scope: !93)
!115 = !DILocation(line: 29, column: 3, scope: !93)
!116 = !DILocalVariable(name: "window", scope: !93, file: !1, line: 36, type: !117)
!117 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Win", file: !5, line: 296, baseType: !6)
!118 = !DILocation(line: 36, column: 11, scope: !93)
!119 = !DILocation(line: 36, column: 42, scope: !93)
!120 = !DILocation(line: 36, column: 20, scope: !93)
!121 = !DILocation(line: 38, column: 7, scope: !122)
!122 = distinct !DILexicalBlock(scope: !93, file: !1, line: 38, column: 7)
!123 = !DILocation(line: 38, column: 14, scope: !122)
!124 = !DILocation(line: 38, column: 7, scope: !93)
!125 = !DILocation(line: 40, column: 9, scope: !126)
!126 = distinct !DILexicalBlock(scope: !122, file: !1, line: 38, column: 19)
!127 = !DILocation(line: 44, column: 9, scope: !126)
!128 = !DILocation(line: 47, column: 9, scope: !126)
!129 = !DILocation(line: 39, column: 5, scope: !126)
!130 = !DILocation(line: 49, column: 3, scope: !126)
!131 = !DILocation(line: 51, column: 20, scope: !93)
!132 = !DILocation(line: 51, column: 3, scope: !93)
!133 = !DILocalVariable(name: "i", scope: !93, file: !1, line: 53, type: !105)
!134 = !DILocation(line: 53, column: 10, scope: !93)
!135 = !DILocation(line: 54, column: 3, scope: !93)
!136 = distinct !{!136, !135, !137}
!137 = !DILocation(line: 67, column: 17, scope: !93)
!138 = !DILocation(line: 56, column: 9, scope: !139)
!139 = distinct !DILexicalBlock(scope: !93, file: !1, line: 54, column: 6)
!140 = !DILocation(line: 60, column: 9, scope: !139)
!141 = !DILocation(line: 63, column: 9, scope: !139)
!142 = !DILocation(line: 55, column: 5, scope: !139)
!143 = !DILocation(line: 66, column: 7, scope: !139)
!144 = !DILocation(line: 67, column: 3, scope: !139)
!145 = !DILocation(line: 67, column: 12, scope: !93)
!146 = !DILocation(line: 67, column: 14, scope: !93)
!147 = !DILocation(line: 69, column: 12, scope: !148)
!148 = distinct !DILexicalBlock(scope: !93, file: !1, line: 69, column: 7)
!149 = !DILocation(line: 69, column: 9, scope: !148)
!150 = !DILocation(line: 69, column: 7, scope: !93)
!151 = !DILocation(line: 70, column: 22, scope: !152)
!152 = distinct !DILexicalBlock(scope: !148, file: !1, line: 69, column: 15)
!153 = !DILocation(line: 70, column: 5, scope: !152)
!154 = !DILocation(line: 71, column: 3, scope: !152)
!155 = !DILocation(line: 73, column: 3, scope: !93)
!156 = !DILocation(line: 75, column: 3, scope: !93)
!157 = !DILocation(line: 77, column: 3, scope: !93)
!158 = !DILocation(line: 79, column: 3, scope: !93)
!159 = distinct !DISubprogram(name: "create_window<unsigned long>", linkageName: "_Z13create_windowImEiiPT_b", scope: !1, file: !1, line: 4, type: !160, isLocal: false, isDefinition: true, scopeLine: 4, flags: DIFlagPrototyped, isOptimized: false, unit: !0, templateParams: !164, variables: !2)
!160 = !DISubroutineType(types: !161)
!161 = !{!117, !4, !162, !163}
!162 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !67, size: 64)
!163 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!164 = !{!165}
!165 = !DITemplateTypeParameter(name: "T", type: !67)
!166 = !DILocalVariable(name: "comm", arg: 1, scope: !159, file: !1, line: 4, type: !4)
!167 = !DILocation(line: 4, column: 33, scope: !159)
!168 = !DILocalVariable(name: "mem", arg: 2, scope: !159, file: !1, line: 4, type: !162)
!169 = !DILocation(line: 4, column: 42, scope: !159)
!170 = !DILocalVariable(name: "flag", arg: 3, scope: !159, file: !1, line: 4, type: !163)
!171 = !DILocation(line: 4, column: 52, scope: !159)
!172 = !DILocalVariable(name: "window", scope: !159, file: !1, line: 5, type: !117)
!173 = !DILocation(line: 5, column: 11, scope: !159)
!174 = !DILocalVariable(name: "comm_to_use", scope: !159, file: !1, line: 6, type: !4)
!175 = !DILocation(line: 6, column: 12, scope: !159)
!176 = !DILocation(line: 6, column: 26, scope: !159)
!177 = !DILocation(line: 9, column: 7, scope: !159)
!178 = !DILocation(line: 13, column: 7, scope: !159)
!179 = !DILocation(line: 8, column: 3, scope: !159)
!180 = !DILocation(line: 17, column: 10, scope: !159)
!181 = !DILocation(line: 17, column: 3, scope: !159)
