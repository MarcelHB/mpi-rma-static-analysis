; ModuleID = 'mpi_wormhole.cpp'
source_filename = "mpi_wormhole.cpp"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline norecurse optnone uwtable
define i32 @main(i32, i8**) #0 !dbg !93 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i64, align 8
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  %15 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !99, metadata !DIExpression()), !dbg !100
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !101, metadata !DIExpression()), !dbg !102
  %16 = call i32 @MPI_Init(i32* %4, i8*** %5), !dbg !103
  call void @llvm.dbg.declare(metadata i64* %6, metadata !104, metadata !DIExpression()), !dbg !107
  store i64 0, i64* %6, align 8, !dbg !107
  call void @llvm.dbg.declare(metadata i32* %7, metadata !108, metadata !DIExpression()), !dbg !109
  call void @llvm.dbg.declare(metadata i32* %8, metadata !110, metadata !DIExpression()), !dbg !111
  store i32 -1, i32* %8, align 4, !dbg !111
  call void @llvm.dbg.declare(metadata i32* %9, metadata !112, metadata !DIExpression()), !dbg !113
  store i32 0, i32* %9, align 4, !dbg !113
  call void @llvm.dbg.declare(metadata i32* %10, metadata !114, metadata !DIExpression()), !dbg !115
  store i32 1, i32* %10, align 4, !dbg !115
  call void @llvm.dbg.declare(metadata i32* %11, metadata !116, metadata !DIExpression()), !dbg !117
  store i32 2, i32* %11, align 4, !dbg !117
  call void @llvm.dbg.declare(metadata i32* %12, metadata !118, metadata !DIExpression()), !dbg !119
  store i32 0, i32* %12, align 4, !dbg !119
  call void @llvm.dbg.declare(metadata i32* %13, metadata !120, metadata !DIExpression()), !dbg !121
  store i32 0, i32* %13, align 4, !dbg !121
  call void @llvm.dbg.declare(metadata i32* %14, metadata !122, metadata !DIExpression()), !dbg !123
  store i32 39, i32* %14, align 4, !dbg !123
  %17 = call i32 @MPI_Comm_rank(i32 1140850688, i32* %8), !dbg !124
  %18 = load i32, i32* %8, align 4, !dbg !125
  %19 = load i32, i32* %8, align 4, !dbg !126
  %20 = call i32 @MPI_Comm_split(i32 1140850688, i32 %18, i32 %19, i32* %7), !dbg !127
  call void @llvm.dbg.declare(metadata i32* %15, metadata !128, metadata !DIExpression()), !dbg !130
  %21 = bitcast i64* %6 to i8*, !dbg !131
  %22 = load i32, i32* %7, align 4, !dbg !132
  %23 = call i32 @MPI_Win_create(i8* %21, i64 8, i32 1, i32 469762048, i32 %22, i32* %15), !dbg !133
  %24 = load i32, i32* %15, align 4, !dbg !134
  %25 = call i32 @MPI_Win_fence(i32 0, i32 %24), !dbg !135
  %26 = load i32, i32* %8, align 4, !dbg !136
  %27 = icmp sgt i32 %26, 0, !dbg !138
  br i1 %27, label %28, label %34, !dbg !139

; <label>:28:                                     ; preds = %2
  %29 = bitcast i32* %8 to i8*, !dbg !140
  %30 = load i32, i32* %8, align 4, !dbg !142
  %31 = sext i32 %30 to i64, !dbg !142
  %32 = load i32, i32* %15, align 4, !dbg !143
  %33 = call i32 @MPI_Get(i8* %29, i32 1, i32 1275069445, i32 0, i64 %31, i32 1, i32 1275069445, i32 %32), !dbg !144
  br label %40, !dbg !145

; <label>:34:                                     ; preds = %2
  %35 = bitcast i32* %8 to i8*, !dbg !146
  %36 = load i32, i32* %8, align 4, !dbg !148
  %37 = sext i32 %36 to i64, !dbg !148
  %38 = load i32, i32* %15, align 4, !dbg !149
  %39 = call i32 @MPI_Put(i8* %35, i32 1, i32 1275069445, i32 0, i64 %37, i32 1, i32 1275069445, i32 %38), !dbg !150
  br label %40

; <label>:40:                                     ; preds = %34, %28
  %41 = load i32, i32* %9, align 4, !dbg !151
  %42 = icmp ne i32 %41, 0, !dbg !151
  br i1 %42, label %43, label %65, !dbg !153

; <label>:43:                                     ; preds = %40
  %44 = load i32, i32* %10, align 4, !dbg !154
  %45 = icmp ne i32 %44, 0, !dbg !154
  br i1 %45, label %46, label %65, !dbg !155

; <label>:46:                                     ; preds = %43
  %47 = load i32, i32* %11, align 4, !dbg !156
  %48 = icmp ne i32 %47, 0, !dbg !156
  br i1 %48, label %49, label %65, !dbg !157

; <label>:49:                                     ; preds = %46
  %50 = load i32, i32* %12, align 4, !dbg !158
  %51 = icmp ne i32 %50, 0, !dbg !158
  br i1 %51, label %52, label %56, !dbg !161

; <label>:52:                                     ; preds = %49
  %53 = load i32, i32* %10, align 4, !dbg !162
  %54 = load i32, i32* %11, align 4, !dbg !164
  %55 = add nsw i32 %53, %54, !dbg !165
  store i32 %55, i32* %14, align 4, !dbg !166
  br label %64, !dbg !167

; <label>:56:                                     ; preds = %49
  br label %57, !dbg !168

; <label>:57:                                     ; preds = %60, %56
  %58 = load i32, i32* %13, align 4, !dbg !170
  %59 = icmp ne i32 %58, 10, !dbg !171
  br i1 %59, label %60, label %63, !dbg !168

; <label>:60:                                     ; preds = %57
  %61 = load i32, i32* %13, align 4, !dbg !172
  %62 = add nsw i32 %61, 1, !dbg !172
  store i32 %62, i32* %13, align 4, !dbg !172
  br label %57, !dbg !168, !llvm.loop !174

; <label>:63:                                     ; preds = %57
  br label %64

; <label>:64:                                     ; preds = %63, %52
  br label %65, !dbg !176

; <label>:65:                                     ; preds = %64, %46, %43, %40
  %66 = load i32, i32* %15, align 4, !dbg !177
  %67 = call i32 @MPI_Win_fence(i32 0, i32 %66), !dbg !178
  %68 = call i32 @MPI_Win_free(i32* %15), !dbg !179
  %69 = call i32 @MPI_Comm_free(i32* %7), !dbg !180
  %70 = call i32 @MPI_Finalize(), !dbg !181
  ret i32 0, !dbg !182
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @MPI_Init(i32*, i8***) #2

declare i32 @MPI_Comm_rank(i32, i32*) #2

declare i32 @MPI_Comm_split(i32, i32, i32, i32*) #2

declare i32 @MPI_Win_create(i8*, i64, i32, i32, i32, i32*) #2

declare i32 @MPI_Win_fence(i32, i32) #2

declare i32 @MPI_Get(i8*, i32, i32, i32, i64, i32, i32, i32) #2

declare i32 @MPI_Put(i8*, i32, i32, i32, i64, i32, i32, i32) #2

declare i32 @MPI_Win_free(i32*) #2

declare i32 @MPI_Comm_free(i32*) #2

declare i32 @MPI_Finalize() #2

attributes #0 = { noinline norecurse optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!89, !90, !91}
!llvm.ident = !{!92}

!0 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !1, producer: "clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3, imports: !9)
!1 = !DIFile(filename: "mpi_wormhole.cpp", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
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
!105 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !106, line: 62, baseType: !67)
!106 = !DIFile(filename: "/usr/include/clang/6.0.0/include/stddef.h", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
!107 = !DILocation(line: 6, column: 10, scope: !93)
!108 = !DILocalVariable(name: "other_comm", scope: !93, file: !1, line: 7, type: !4)
!109 = !DILocation(line: 7, column: 12, scope: !93)
!110 = !DILocalVariable(name: "rank", scope: !93, file: !1, line: 8, type: !6)
!111 = !DILocation(line: 8, column: 7, scope: !93)
!112 = !DILocalVariable(name: "a", scope: !93, file: !1, line: 10, type: !6)
!113 = !DILocation(line: 10, column: 7, scope: !93)
!114 = !DILocalVariable(name: "b", scope: !93, file: !1, line: 10, type: !6)
!115 = !DILocation(line: 10, column: 14, scope: !93)
!116 = !DILocalVariable(name: "c", scope: !93, file: !1, line: 10, type: !6)
!117 = !DILocation(line: 10, column: 21, scope: !93)
!118 = !DILocalVariable(name: "d", scope: !93, file: !1, line: 10, type: !6)
!119 = !DILocation(line: 10, column: 28, scope: !93)
!120 = !DILocalVariable(name: "e", scope: !93, file: !1, line: 10, type: !6)
!121 = !DILocation(line: 10, column: 35, scope: !93)
!122 = !DILocalVariable(name: "x", scope: !93, file: !1, line: 10, type: !6)
!123 = !DILocation(line: 10, column: 42, scope: !93)
!124 = !DILocation(line: 12, column: 3, scope: !93)
!125 = !DILocation(line: 16, column: 7, scope: !93)
!126 = !DILocation(line: 17, column: 7, scope: !93)
!127 = !DILocation(line: 14, column: 3, scope: !93)
!128 = !DILocalVariable(name: "window", scope: !93, file: !1, line: 21, type: !129)
!129 = !DIDerivedType(tag: DW_TAG_typedef, name: "MPI_Win", file: !5, line: 296, baseType: !6)
!130 = !DILocation(line: 21, column: 11, scope: !93)
!131 = !DILocation(line: 24, column: 7, scope: !93)
!132 = !DILocation(line: 28, column: 7, scope: !93)
!133 = !DILocation(line: 23, column: 3, scope: !93)
!134 = !DILocation(line: 32, column: 20, scope: !93)
!135 = !DILocation(line: 32, column: 3, scope: !93)
!136 = !DILocation(line: 34, column: 7, scope: !137)
!137 = distinct !DILexicalBlock(scope: !93, file: !1, line: 34, column: 7)
!138 = !DILocation(line: 34, column: 12, scope: !137)
!139 = !DILocation(line: 34, column: 7, scope: !93)
!140 = !DILocation(line: 36, column: 9, scope: !141)
!141 = distinct !DILexicalBlock(scope: !137, file: !1, line: 34, column: 17)
!142 = !DILocation(line: 40, column: 9, scope: !141)
!143 = !DILocation(line: 43, column: 9, scope: !141)
!144 = !DILocation(line: 35, column: 5, scope: !141)
!145 = !DILocation(line: 45, column: 3, scope: !141)
!146 = !DILocation(line: 47, column: 9, scope: !147)
!147 = distinct !DILexicalBlock(scope: !137, file: !1, line: 45, column: 10)
!148 = !DILocation(line: 51, column: 9, scope: !147)
!149 = !DILocation(line: 54, column: 9, scope: !147)
!150 = !DILocation(line: 46, column: 5, scope: !147)
!151 = !DILocation(line: 58, column: 7, scope: !152)
!152 = distinct !DILexicalBlock(scope: !93, file: !1, line: 58, column: 7)
!153 = !DILocation(line: 58, column: 9, scope: !152)
!154 = !DILocation(line: 58, column: 12, scope: !152)
!155 = !DILocation(line: 58, column: 14, scope: !152)
!156 = !DILocation(line: 58, column: 17, scope: !152)
!157 = !DILocation(line: 58, column: 7, scope: !93)
!158 = !DILocation(line: 59, column: 9, scope: !159)
!159 = distinct !DILexicalBlock(scope: !160, file: !1, line: 59, column: 9)
!160 = distinct !DILexicalBlock(scope: !152, file: !1, line: 58, column: 20)
!161 = !DILocation(line: 59, column: 9, scope: !160)
!162 = !DILocation(line: 60, column: 11, scope: !163)
!163 = distinct !DILexicalBlock(scope: !159, file: !1, line: 59, column: 12)
!164 = !DILocation(line: 60, column: 15, scope: !163)
!165 = !DILocation(line: 60, column: 13, scope: !163)
!166 = !DILocation(line: 60, column: 9, scope: !163)
!167 = !DILocation(line: 61, column: 5, scope: !163)
!168 = !DILocation(line: 62, column: 7, scope: !169)
!169 = distinct !DILexicalBlock(scope: !159, file: !1, line: 61, column: 12)
!170 = !DILocation(line: 62, column: 14, scope: !169)
!171 = !DILocation(line: 62, column: 16, scope: !169)
!172 = !DILocation(line: 63, column: 11, scope: !173)
!173 = distinct !DILexicalBlock(scope: !169, file: !1, line: 62, column: 23)
!174 = distinct !{!174, !168, !175}
!175 = !DILocation(line: 64, column: 7, scope: !169)
!176 = !DILocation(line: 66, column: 3, scope: !160)
!177 = !DILocation(line: 68, column: 20, scope: !93)
!178 = !DILocation(line: 68, column: 3, scope: !93)
!179 = !DILocation(line: 70, column: 3, scope: !93)
!180 = !DILocation(line: 72, column: 3, scope: !93)
!181 = !DILocation(line: 74, column: 3, scope: !93)
!182 = !DILocation(line: 76, column: 3, scope: !93)
