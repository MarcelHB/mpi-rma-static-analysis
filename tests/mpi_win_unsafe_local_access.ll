; ModuleID = 'mpi_win_unsafe_local_access.cpp'
source_filename = "mpi_win_unsafe_local_access.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main(i32, i8**) #0 !dbg !93 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca [128 x i8], align 16
  %7 = alloca i8, align 1
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !99, metadata !DIExpression()), !dbg !100
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !101, metadata !DIExpression()), !dbg !102
  %10 = call i32 @MPI_Init(i32* %4, i8*** %5), !dbg !103
  call void @llvm.dbg.declare(metadata [128 x i8]* %6, metadata !104, metadata !DIExpression()), !dbg !108
  %11 = bitcast [128 x i8]* %6 to i8*, !dbg !108
  call void @llvm.memset.p0i8.i64(i8* %11, i8 0, i64 128, i32 16, i1 false), !dbg !108
  call void @llvm.dbg.declare(metadata i8* %7, metadata !109, metadata !DIExpression()), !dbg !110
  store i8 0, i8* %7, align 1, !dbg !110
  call void @llvm.dbg.declare(metadata i32* %8, metadata !111, metadata !DIExpression()), !dbg !112
  store i32 -1, i32* %8, align 4, !dbg !112
  %12 = call i32 @MPI_Comm_rank(i32 1140850688, i32* %8), !dbg !113
  call void @llvm.dbg.declare(metadata i32* %9, metadata !114, metadata !DIExpression()), !dbg !116
  %13 = bitcast [128 x i8]* %6 to i8*, !dbg !117
  %14 = call i32 @MPI_Win_create(i8* %13, i64 128, i32 1, i32 469762048, i32 1140850688, i32* %9), !dbg !118
  %15 = load i32, i32* %9, align 4, !dbg !119
  %16 = call i32 @MPI_Win_fence(i32 0, i32 %15), !dbg !120
  %17 = load i32, i32* %8, align 4, !dbg !121
  %18 = icmp sge i32 %17, 1, !dbg !123
  br i1 %18, label %19, label %29, !dbg !124

; <label>:19:                                     ; preds = %2
  %20 = load i32, i32* %8, align 4, !dbg !125
  %21 = sext i32 %20 to i64, !dbg !125
  %22 = load i32, i32* %9, align 4, !dbg !127
  %23 = call i32 @MPI_Get(i8* %7, i32 1, i32 1275069445, i32 0, i64 %21, i32 1, i32 1275069445, i32 %22), !dbg !128
  %24 = load i8, i8* %7, align 1, !dbg !129
  %25 = zext i8 %24 to i32, !dbg !129
  %26 = add nsw i32 %25, 1, !dbg !130
  %27 = trunc i32 %26 to i8, !dbg !129
  %28 = getelementptr inbounds [128 x i8], [128 x i8]* %6, i64 0, i64 0, !dbg !131
  store i8 %27, i8* %28, align 16, !dbg !132
  br label %34, !dbg !133

; <label>:29:                                     ; preds = %2
  %30 = load i32, i32* %8, align 4, !dbg !134
  %31 = sext i32 %30 to i64, !dbg !134
  %32 = load i32, i32* %9, align 4, !dbg !136
  %33 = call i32 @MPI_Put(i8* %7, i32 1, i32 1275069445, i32 0, i64 %31, i32 1, i32 1275069445, i32 %32), !dbg !137
  store i8 2, i8* %7, align 1, !dbg !138
  br label %34

; <label>:34:                                     ; preds = %29, %19
  %35 = load i32, i32* %9, align 4, !dbg !139
  %36 = call i32 @MPI_Win_fence(i32 0, i32 %35), !dbg !140
  %37 = call i32 @MPI_Win_free(i32* %9), !dbg !141
  %38 = call i32 @MPI_Finalize(), !dbg !142
  ret i32 0, !dbg !143
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @MPI_Init(i32*, i8***) #2

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i32, i1) #3

declare i32 @MPI_Comm_rank(i32, i32*) #2

declare i32 @MPI_Win_create(i8*, i64, i32, i32, i32, i32*) #2

declare i32 @MPI_Win_fence(i32, i32) #2

declare i32 @MPI_Get(i8*, i32, i32, i32, i64, i32, i32, i32) #2

declare i32 @MPI_Put(i8*, i32, i32, i32, i64, i32, i32, i32) #2

declare i32 @MPI_Win_free(i32*) #2

declare i32 @MPI_Finalize() #2

attributes #0 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!89, !90, !91}
!llvm.ident = !{!92}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3, imports: !9)
!1 = !DIFile(filename: "mpi_win_unsafe_local_access.cpp", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!2 = !{}
!3 = !{!4, !7, !8}
!4 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Comm", file: !5, line: 287, baseType: !6)
!5 = !DIFile(filename: "/usr/include/mpich/mpi.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Info", file: !5, line: 528, baseType: !6)
!8 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Datatype", file: !5, line: 104, baseType: !6)
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
!93 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 3, type: !94, isLocal: false, isDefinition: true, scopeLine: 3, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!94 = !DISubroutineType(types: !95)
!95 = !{!6, !6, !96}
!96 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !97, size: 64)
!97 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !98, size: 64)
!98 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!99 = !DILocalVariable(name: "argc", arg: 1, scope: !93, file: !1, line: 3, type: !6)
!100 = !DILocation(line: 3, column: 15, scope: !93)
!101 = !DILocalVariable(name: "argv", arg: 2, scope: !93, file: !1, line: 3, type: !96)
!102 = !DILocation(line: 3, column: 28, scope: !93)
!103 = !DILocation(line: 4, column: 3, scope: !93)
!104 = !DILocalVariable(name: "memory", scope: !93, file: !1, line: 6, type: !105)
!105 = !DICompositeType(tag: DW_TAG_array_type, baseType: !52, size: 1024, elements: !106)
!106 = !{!107}
!107 = !DISubrange(count: 128)
!108 = !DILocation(line: 6, column: 11, scope: !93)
!109 = !DILocalVariable(name: "value", scope: !93, file: !1, line: 6, type: !52)
!110 = !DILocation(line: 6, column: 30, scope: !93)
!111 = !DILocalVariable(name: "rank", scope: !93, file: !1, line: 7, type: !6)
!112 = !DILocation(line: 7, column: 7, scope: !93)
!113 = !DILocation(line: 9, column: 3, scope: !93)
!114 = !DILocalVariable(name: "window", scope: !93, file: !1, line: 11, type: !115)
!115 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Win", file: !5, line: 296, baseType: !6)
!116 = !DILocation(line: 11, column: 11, scope: !93)
!117 = !DILocation(line: 14, column: 7, scope: !93)
!118 = !DILocation(line: 13, column: 3, scope: !93)
!119 = !DILocation(line: 22, column: 20, scope: !93)
!120 = !DILocation(line: 22, column: 3, scope: !93)
!121 = !DILocation(line: 24, column: 7, scope: !122)
!122 = distinct !DILexicalBlock(scope: !93, file: !1, line: 24, column: 7)
!123 = !DILocation(line: 24, column: 12, scope: !122)
!124 = !DILocation(line: 24, column: 7, scope: !93)
!125 = !DILocation(line: 30, column: 9, scope: !126)
!126 = distinct !DILexicalBlock(scope: !122, file: !1, line: 24, column: 18)
!127 = !DILocation(line: 33, column: 9, scope: !126)
!128 = !DILocation(line: 25, column: 5, scope: !126)
!129 = !DILocation(line: 36, column: 17, scope: !126)
!130 = !DILocation(line: 36, column: 23, scope: !126)
!131 = !DILocation(line: 36, column: 5, scope: !126)
!132 = !DILocation(line: 36, column: 15, scope: !126)
!133 = !DILocation(line: 37, column: 3, scope: !126)
!134 = !DILocation(line: 43, column: 9, scope: !135)
!135 = distinct !DILexicalBlock(scope: !122, file: !1, line: 37, column: 10)
!136 = !DILocation(line: 46, column: 9, scope: !135)
!137 = !DILocation(line: 38, column: 5, scope: !135)
!138 = !DILocation(line: 49, column: 11, scope: !135)
!139 = !DILocation(line: 51, column: 20, scope: !93)
!140 = !DILocation(line: 51, column: 3, scope: !93)
!141 = !DILocation(line: 53, column: 3, scope: !93)
!142 = !DILocation(line: 55, column: 3, scope: !93)
!143 = !DILocation(line: 57, column: 3, scope: !93)
