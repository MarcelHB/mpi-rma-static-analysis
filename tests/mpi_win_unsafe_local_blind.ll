; ModuleID = 'mpi_win_unsafe_local_blind.cpp'
source_filename = "mpi_win_unsafe_local_blind.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define zeroext i8 @_Z8getIndexv() #0 !dbg !93 {
  ret i8 9, !dbg !96
}

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main(i32, i8**) #1 !dbg !97 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca [128 x i8], align 16
  %7 = alloca i8, align 1
  %8 = alloca [16 x i8], align 16
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i64, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !103, metadata !DIExpression()), !dbg !104
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !105, metadata !DIExpression()), !dbg !106
  %12 = call i32 @MPI_Init(i32* %4, i8*** %5), !dbg !107
  call void @llvm.dbg.declare(metadata [128 x i8]* %6, metadata !108, metadata !DIExpression()), !dbg !112
  %13 = bitcast [128 x i8]* %6 to i8*, !dbg !112
  call void @llvm.memset.p0i8.i64(i8* %13, i8 0, i64 128, i32 16, i1 false), !dbg !112
  call void @llvm.dbg.declare(metadata i8* %7, metadata !113, metadata !DIExpression()), !dbg !114
  store i8 0, i8* %7, align 1, !dbg !114
  call void @llvm.dbg.declare(metadata [16 x i8]* %8, metadata !115, metadata !DIExpression()), !dbg !119
  %14 = bitcast [16 x i8]* %8 to i8*, !dbg !119
  call void @llvm.memset.p0i8.i64(i8* %14, i8 0, i64 16, i32 16, i1 false), !dbg !119
  call void @llvm.dbg.declare(metadata i32* %9, metadata !120, metadata !DIExpression()), !dbg !121
  store i32 -1, i32* %9, align 4, !dbg !121
  %15 = call i32 @MPI_Comm_rank(i32 1140850688, i32* %9), !dbg !122
  call void @llvm.dbg.declare(metadata i32* %10, metadata !123, metadata !DIExpression()), !dbg !125
  %16 = bitcast [128 x i8]* %6 to i8*, !dbg !126
  %17 = call i32 @MPI_Win_create(i8* %16, i64 128, i32 1, i32 469762048, i32 1140850688, i32* %10), !dbg !127
  %18 = load i32, i32* %10, align 4, !dbg !128
  %19 = call i32 @MPI_Win_fence(i32 0, i32 %18), !dbg !129
  %20 = load i32, i32* %9, align 4, !dbg !130
  %21 = icmp sgt i32 %20, 0, !dbg !132
  br i1 %21, label %22, label %33, !dbg !133

; <label>:22:                                     ; preds = %2
  %23 = bitcast [16 x i8]* %8 to i8*, !dbg !134
  %24 = load i32, i32* %9, align 4, !dbg !136
  %25 = sext i32 %24 to i64, !dbg !136
  %26 = load i32, i32* %10, align 4, !dbg !137
  %27 = call i32 @MPI_Get(i8* %23, i32 1, i32 1275069445, i32 0, i64 %25, i32 1, i32 1275069445, i32 %26), !dbg !138
  call void @llvm.dbg.declare(metadata i64* %11, metadata !139, metadata !DIExpression()), !dbg !142
  %28 = call zeroext i8 @_Z8getIndexv(), !dbg !143
  %29 = zext i8 %28 to i64, !dbg !143
  store i64 %29, i64* %11, align 8, !dbg !142
  %30 = load i64, i64* %11, align 8, !dbg !144
  %31 = getelementptr inbounds [16 x i8], [16 x i8]* %8, i64 0, i64 %30, !dbg !145
  %32 = load i8, i8* %31, align 1, !dbg !145
  store i8 %32, i8* %7, align 1, !dbg !146
  br label %33, !dbg !147

; <label>:33:                                     ; preds = %22, %2
  %34 = load i32, i32* %10, align 4, !dbg !148
  %35 = call i32 @MPI_Win_fence(i32 0, i32 %34), !dbg !149
  %36 = call i32 @MPI_Win_free(i32* %10), !dbg !150
  %37 = call i32 @MPI_Finalize(), !dbg !151
  ret i32 0, !dbg !152
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #2

declare i32 @MPI_Init(i32*, i8***) #3

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i32, i1) #4

declare i32 @MPI_Comm_rank(i32, i32*) #3

declare i32 @MPI_Win_create(i8*, i64, i32, i32, i32, i32*) #3

declare i32 @MPI_Win_fence(i32, i32) #3

declare i32 @MPI_Get(i8*, i32, i32, i32, i64, i32, i32, i32) #3

declare i32 @MPI_Win_free(i32*) #3

declare i32 @MPI_Finalize() #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind readnone speculatable }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!89, !90, !91}
!llvm.ident = !{!92}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3, imports: !9)
!1 = !DIFile(filename: "mpi_win_unsafe_local_blind.cpp", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
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
!93 = distinct !DISubprogram(name: "getIndex", linkageName: "_Z8getIndexv", scope: !1, file: !1, line: 3, type: !94, isLocal: false, isDefinition: true, scopeLine: 3, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!94 = !DISubroutineType(types: !95)
!95 = !{!52}
!96 = !DILocation(line: 4, column: 3, scope: !93)
!97 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 7, type: !98, isLocal: false, isDefinition: true, scopeLine: 7, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!98 = !DISubroutineType(types: !99)
!99 = !{!6, !6, !100}
!100 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !101, size: 64)
!101 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !102, size: 64)
!102 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!103 = !DILocalVariable(name: "argc", arg: 1, scope: !97, file: !1, line: 7, type: !6)
!104 = !DILocation(line: 7, column: 15, scope: !97)
!105 = !DILocalVariable(name: "argv", arg: 2, scope: !97, file: !1, line: 7, type: !100)
!106 = !DILocation(line: 7, column: 28, scope: !97)
!107 = !DILocation(line: 8, column: 3, scope: !97)
!108 = !DILocalVariable(name: "memory", scope: !97, file: !1, line: 10, type: !109)
!109 = !DICompositeType(tag: DW_TAG_array_type, baseType: !52, size: 1024, elements: !110)
!110 = !{!111}
!111 = !DISubrange(count: 128)
!112 = !DILocation(line: 10, column: 11, scope: !97)
!113 = !DILocalVariable(name: "value", scope: !97, file: !1, line: 10, type: !52)
!114 = !DILocation(line: 10, column: 30, scope: !97)
!115 = !DILocalVariable(name: "localMemory", scope: !97, file: !1, line: 10, type: !116)
!116 = !DICompositeType(tag: DW_TAG_array_type, baseType: !52, size: 128, elements: !117)
!117 = !{!118}
!118 = !DISubrange(count: 16)
!119 = !DILocation(line: 10, column: 41, scope: !97)
!120 = !DILocalVariable(name: "rank", scope: !97, file: !1, line: 11, type: !6)
!121 = !DILocation(line: 11, column: 7, scope: !97)
!122 = !DILocation(line: 13, column: 3, scope: !97)
!123 = !DILocalVariable(name: "window", scope: !97, file: !1, line: 15, type: !124)
!124 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Win", file: !5, line: 296, baseType: !6)
!125 = !DILocation(line: 15, column: 11, scope: !97)
!126 = !DILocation(line: 18, column: 7, scope: !97)
!127 = !DILocation(line: 17, column: 3, scope: !97)
!128 = !DILocation(line: 26, column: 20, scope: !97)
!129 = !DILocation(line: 26, column: 3, scope: !97)
!130 = !DILocation(line: 28, column: 7, scope: !131)
!131 = distinct !DILexicalBlock(scope: !97, file: !1, line: 28, column: 7)
!132 = !DILocation(line: 28, column: 12, scope: !131)
!133 = !DILocation(line: 28, column: 7, scope: !97)
!134 = !DILocation(line: 30, column: 9, scope: !135)
!135 = distinct !DILexicalBlock(scope: !131, file: !1, line: 28, column: 17)
!136 = !DILocation(line: 34, column: 9, scope: !135)
!137 = !DILocation(line: 37, column: 9, scope: !135)
!138 = !DILocation(line: 29, column: 5, scope: !135)
!139 = !DILocalVariable(name: "i", scope: !135, file: !1, line: 40, type: !140)
!140 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !141, line: 62, baseType: !67)
!141 = !DIFile(filename: "/usr/include/clang/6.0.0/include/stddef.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!142 = !DILocation(line: 40, column: 12, scope: !135)
!143 = !DILocation(line: 40, column: 16, scope: !135)
!144 = !DILocation(line: 41, column: 25, scope: !135)
!145 = !DILocation(line: 41, column: 13, scope: !135)
!146 = !DILocation(line: 41, column: 11, scope: !135)
!147 = !DILocation(line: 42, column: 3, scope: !135)
!148 = !DILocation(line: 44, column: 20, scope: !97)
!149 = !DILocation(line: 44, column: 3, scope: !97)
!150 = !DILocation(line: 46, column: 3, scope: !97)
!151 = !DILocation(line: 48, column: 3, scope: !97)
!152 = !DILocation(line: 50, column: 3, scope: !97)
