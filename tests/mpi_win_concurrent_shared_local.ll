; ModuleID = 'mpi_win_concurrent_shared_local.cpp'
source_filename = "mpi_win_concurrent_shared_local.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main(i32, i8**) #0 !dbg !92 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i8*, align 8
  %10 = alloca i8*, align 8
  %11 = alloca i64, align 8
  %12 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !98, metadata !DIExpression()), !dbg !99
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !100, metadata !DIExpression()), !dbg !101
  %13 = call i32 @MPI_Init(i32* %4, i8*** %5), !dbg !102
  call void @llvm.dbg.declare(metadata i32* %6, metadata !103, metadata !DIExpression()), !dbg !104
  store i32 -1, i32* %6, align 4, !dbg !104
  call void @llvm.dbg.declare(metadata i32* %7, metadata !105, metadata !DIExpression()), !dbg !106
  store i32 0, i32* %7, align 4, !dbg !106
  call void @llvm.dbg.declare(metadata i32* %8, metadata !107, metadata !DIExpression()), !dbg !109
  call void @llvm.dbg.declare(metadata i8** %9, metadata !110, metadata !DIExpression()), !dbg !112
  store i8* null, i8** %9, align 8, !dbg !112
  call void @llvm.dbg.declare(metadata i8** %10, metadata !113, metadata !DIExpression()), !dbg !114
  store i8* null, i8** %10, align 8, !dbg !114
  %14 = call i32 @MPI_Comm_rank(i32 1140850688, i32* %6), !dbg !115
  %15 = bitcast i8** %9 to i8*, !dbg !116
  %16 = call i32 @MPI_Win_allocate_shared(i64 128, i32 1, i32 469762048, i32 1140850688, i8* %15, i32* %8), !dbg !117
  %17 = load i32, i32* %6, align 4, !dbg !118
  %18 = icmp eq i32 0, %17, !dbg !120
  br i1 %18, label %19, label %26, !dbg !121

; <label>:19:                                     ; preds = %2
  %20 = load i32, i32* %8, align 4, !dbg !122
  %21 = call i32 @MPI_Win_fence(i32 0, i32 %20), !dbg !124
  %22 = load i8*, i8** %9, align 8, !dbg !125
  %23 = getelementptr inbounds i8, i8* %22, i64 0, !dbg !125
  store i8 123, i8* %23, align 1, !dbg !126
  %24 = load i32, i32* %8, align 4, !dbg !127
  %25 = call i32 @MPI_Win_fence(i32 0, i32 %24), !dbg !128
  br label %37, !dbg !129

; <label>:26:                                     ; preds = %2
  %27 = load i32, i32* %8, align 4, !dbg !130
  %28 = call i32 @MPI_Win_fence(i32 0, i32 %27), !dbg !132
  call void @llvm.dbg.declare(metadata i64* %11, metadata !133, metadata !DIExpression()), !dbg !135
  store i64 0, i64* %11, align 8, !dbg !135
  call void @llvm.dbg.declare(metadata i32* %12, metadata !136, metadata !DIExpression()), !dbg !137
  store i32 0, i32* %12, align 4, !dbg !137
  %29 = load i32, i32* %8, align 4, !dbg !138
  %30 = load i32, i32* %7, align 4, !dbg !139
  %31 = bitcast i8** %10 to i8*, !dbg !140
  %32 = call i32 @MPI_Win_shared_query(i32 %29, i32 %30, i64* %11, i32* %12, i8* %31), !dbg !141
  %33 = load i8*, i8** %10, align 8, !dbg !142
  %34 = getelementptr inbounds i8, i8* %33, i64 0, !dbg !142
  store i8 17, i8* %34, align 1, !dbg !143
  %35 = load i32, i32* %8, align 4, !dbg !144
  %36 = call i32 @MPI_Win_fence(i32 0, i32 %35), !dbg !145
  br label %37

; <label>:37:                                     ; preds = %26, %19
  %38 = call i32 @MPI_Win_free(i32* %8), !dbg !146
  %39 = call i32 @MPI_Finalize(), !dbg !147
  ret i32 0, !dbg !148
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @MPI_Init(i32*, i8***) #2

declare i32 @MPI_Comm_rank(i32, i32*) #2

declare i32 @MPI_Win_allocate_shared(i64, i32, i32, i32, i8*, i32*) #2

declare i32 @MPI_Win_fence(i32, i32) #2

declare i32 @MPI_Win_shared_query(i32, i32, i64*, i32*, i8*) #2

declare i32 @MPI_Win_free(i32*) #2

declare i32 @MPI_Finalize() #2

attributes #0 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!88, !89, !90}
!llvm.ident = !{!91}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3, imports: !8)
!1 = !DIFile(filename: "mpi_win_concurrent_shared_local.cpp", directory: "/home/marcelhb/Source/master_docs/passes/phasar/tests")
!2 = !{}
!3 = !{!4, !7}
!4 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Comm", file: !5, line: 287, baseType: !6)
!5 = !DIFile(filename: "/usr/include/mpich/mpi.h", directory: "/home/marcelhb/Source/master_docs/passes/phasar/tests")
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Info", file: !5, line: 528, baseType: !6)
!8 = !{!9, !17, !21, !24, !28, !31, !33, !35, !37, !39, !41, !43, !45, !48, !50, !55, !59, !63, !67, !69, !71, !73, !75, !77, !79, !81, !83, !86}
!9 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !11, file: !16, line: 48)
!10 = !DINamespace(name: "std", scope: null)
!11 = !DIDerivedType(tag: DW_TAG_typedef, name: "int8_t", file: !12, line: 24, baseType: !13)
!12 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/stdint-intn.h", directory: "/home/marcelhb/Source/master_docs/passes/phasar/tests")
!13 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int8_t", file: !14, line: 36, baseType: !15)
!14 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/types.h", directory: "/home/marcelhb/Source/master_docs/passes/phasar/tests")
!15 = !DIBasicType(name: "signed char", size: 8, encoding: DW_ATE_signed_char)
!16 = !DIFile(filename: "/usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/cstdint", directory: "/home/marcelhb/Source/master_docs/passes/phasar/tests")
!17 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !18, file: !16, line: 49)
!18 = !DIDerivedType(tag: DW_TAG_typedef, name: "int16_t", file: !12, line: 25, baseType: !19)
!19 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int16_t", file: !14, line: 38, baseType: !20)
!20 = !DIBasicType(name: "short", size: 16, encoding: DW_ATE_signed)
!21 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !22, file: !16, line: 50)
!22 = !DIDerivedType(tag: DW_TAG_typedef, name: "int32_t", file: !12, line: 26, baseType: !23)
!23 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int32_t", file: !14, line: 40, baseType: !6)
!24 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !25, file: !16, line: 51)
!25 = !DIDerivedType(tag: DW_TAG_typedef, name: "int64_t", file: !12, line: 27, baseType: !26)
!26 = !DIDerivedType(tag: DW_TAG_typedef, name: "__int64_t", file: !14, line: 43, baseType: !27)
!27 = !DIBasicType(name: "long int", size: 64, encoding: DW_ATE_signed)
!28 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !29, file: !16, line: 53)
!29 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_fast8_t", file: !30, line: 68, baseType: !15)
!30 = !DIFile(filename: "/usr/include/stdint.h", directory: "/home/marcelhb/Source/master_docs/passes/phasar/tests")
!31 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !32, file: !16, line: 54)
!32 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_fast16_t", file: !30, line: 70, baseType: !27)
!33 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !34, file: !16, line: 55)
!34 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_fast32_t", file: !30, line: 71, baseType: !27)
!35 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !36, file: !16, line: 56)
!36 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_fast64_t", file: !30, line: 72, baseType: !27)
!37 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !38, file: !16, line: 58)
!38 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_least8_t", file: !30, line: 43, baseType: !15)
!39 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !40, file: !16, line: 59)
!40 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_least16_t", file: !30, line: 44, baseType: !20)
!41 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !42, file: !16, line: 60)
!42 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_least32_t", file: !30, line: 45, baseType: !6)
!43 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !44, file: !16, line: 61)
!44 = !DIDerivedType(tag: DW_TAG_typedef, name: "int_least64_t", file: !30, line: 47, baseType: !27)
!45 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !46, file: !16, line: 63)
!46 = !DIDerivedType(tag: DW_TAG_typedef, name: "intmax_t", file: !30, line: 111, baseType: !47)
!47 = !DIDerivedType(tag: DW_TAG_typedef, name: "__intmax_t", file: !14, line: 61, baseType: !27)
!48 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !49, file: !16, line: 64)
!49 = !DIDerivedType(tag: DW_TAG_typedef, name: "intptr_t", file: !30, line: 97, baseType: !27)
!50 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !51, file: !16, line: 66)
!51 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint8_t", file: !52, line: 24, baseType: !53)
!52 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/stdint-uintn.h", directory: "/home/marcelhb/Source/master_docs/passes/phasar/tests")
!53 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !14, line: 37, baseType: !54)
!54 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!55 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !56, file: !16, line: 67)
!56 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint16_t", file: !52, line: 25, baseType: !57)
!57 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint16_t", file: !14, line: 39, baseType: !58)
!58 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!59 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !60, file: !16, line: 68)
!60 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint32_t", file: !52, line: 26, baseType: !61)
!61 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint32_t", file: !14, line: 41, baseType: !62)
!62 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!63 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !64, file: !16, line: 69)
!64 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint64_t", file: !52, line: 27, baseType: !65)
!65 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint64_t", file: !14, line: 44, baseType: !66)
!66 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!67 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !68, file: !16, line: 71)
!68 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_fast8_t", file: !30, line: 81, baseType: !54)
!69 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !70, file: !16, line: 72)
!70 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_fast16_t", file: !30, line: 83, baseType: !66)
!71 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !72, file: !16, line: 73)
!72 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_fast32_t", file: !30, line: 84, baseType: !66)
!73 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !74, file: !16, line: 74)
!74 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_fast64_t", file: !30, line: 85, baseType: !66)
!75 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !76, file: !16, line: 76)
!76 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_least8_t", file: !30, line: 54, baseType: !54)
!77 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !78, file: !16, line: 77)
!78 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_least16_t", file: !30, line: 55, baseType: !58)
!79 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !80, file: !16, line: 78)
!80 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_least32_t", file: !30, line: 56, baseType: !62)
!81 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !82, file: !16, line: 79)
!82 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint_least64_t", file: !30, line: 58, baseType: !66)
!83 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !84, file: !16, line: 81)
!84 = !DIDerivedType(tag: DW_TAG_typedef, name: "uintmax_t", file: !30, line: 112, baseType: !85)
!85 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uintmax_t", file: !14, line: 62, baseType: !66)
!86 = !DIImportedEntity(tag: DW_TAG_imported_declaration, scope: !10, entity: !87, file: !16, line: 82)
!87 = !DIDerivedType(tag: DW_TAG_typedef, name: "uintptr_t", file: !30, line: 100, baseType: !66)
!88 = !{i32 2, !"Dwarf Version", i32 4}
!89 = !{i32 2, !"Debug Info Version", i32 3}
!90 = !{i32 1, !"wchar_size", i32 4}
!91 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
!92 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 5, type: !93, isLocal: false, isDefinition: true, scopeLine: 5, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!93 = !DISubroutineType(types: !94)
!94 = !{!6, !6, !95}
!95 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !96, size: 64)
!96 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !97, size: 64)
!97 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!98 = !DILocalVariable(name: "argc", arg: 1, scope: !92, file: !1, line: 5, type: !6)
!99 = !DILocation(line: 5, column: 15, scope: !92)
!100 = !DILocalVariable(name: "argv", arg: 2, scope: !92, file: !1, line: 5, type: !95)
!101 = !DILocation(line: 5, column: 28, scope: !92)
!102 = !DILocation(line: 6, column: 3, scope: !92)
!103 = !DILocalVariable(name: "rank", scope: !92, file: !1, line: 8, type: !6)
!104 = !DILocation(line: 8, column: 7, scope: !92)
!105 = !DILocalVariable(name: "masterRank", scope: !92, file: !1, line: 9, type: !6)
!106 = !DILocation(line: 9, column: 7, scope: !92)
!107 = !DILocalVariable(name: "window", scope: !92, file: !1, line: 10, type: !108)
!108 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Win", file: !5, line: 296, baseType: !6)
!109 = !DILocation(line: 10, column: 11, scope: !92)
!110 = !DILocalVariable(name: "local_shared_memory", scope: !92, file: !1, line: 11, type: !111)
!111 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !51, size: 64)
!112 = !DILocation(line: 11, column: 12, scope: !92)
!113 = !DILocalVariable(name: "other_shared_memory", scope: !92, file: !1, line: 12, type: !111)
!114 = !DILocation(line: 12, column: 12, scope: !92)
!115 = !DILocation(line: 14, column: 3, scope: !92)
!116 = !DILocation(line: 21, column: 7, scope: !92)
!117 = !DILocation(line: 16, column: 3, scope: !92)
!118 = !DILocation(line: 25, column: 12, scope: !119)
!119 = distinct !DILexicalBlock(scope: !92, file: !1, line: 25, column: 7)
!120 = !DILocation(line: 25, column: 9, scope: !119)
!121 = !DILocation(line: 25, column: 7, scope: !92)
!122 = !DILocation(line: 26, column: 22, scope: !123)
!123 = distinct !DILexicalBlock(scope: !119, file: !1, line: 25, column: 18)
!124 = !DILocation(line: 26, column: 5, scope: !123)
!125 = !DILocation(line: 28, column: 5, scope: !123)
!126 = !DILocation(line: 28, column: 28, scope: !123)
!127 = !DILocation(line: 30, column: 22, scope: !123)
!128 = !DILocation(line: 30, column: 5, scope: !123)
!129 = !DILocation(line: 31, column: 3, scope: !123)
!130 = !DILocation(line: 32, column: 22, scope: !131)
!131 = distinct !DILexicalBlock(scope: !119, file: !1, line: 31, column: 10)
!132 = !DILocation(line: 32, column: 5, scope: !131)
!133 = !DILocalVariable(name: "size", scope: !131, file: !1, line: 33, type: !134)
!134 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Aint", file: !5, line: 558, baseType: !27)
!135 = !DILocation(line: 33, column: 14, scope: !131)
!136 = !DILocalVariable(name: "disp", scope: !131, file: !1, line: 34, type: !6)
!137 = !DILocation(line: 34, column: 9, scope: !131)
!138 = !DILocation(line: 37, column: 9, scope: !131)
!139 = !DILocation(line: 38, column: 9, scope: !131)
!140 = !DILocation(line: 41, column: 9, scope: !131)
!141 = !DILocation(line: 36, column: 5, scope: !131)
!142 = !DILocation(line: 44, column: 5, scope: !131)
!143 = !DILocation(line: 44, column: 28, scope: !131)
!144 = !DILocation(line: 46, column: 22, scope: !131)
!145 = !DILocation(line: 46, column: 5, scope: !131)
!146 = !DILocation(line: 49, column: 3, scope: !92)
!147 = !DILocation(line: 51, column: 3, scope: !92)
!148 = !DILocation(line: 53, column: 3, scope: !92)
