; ModuleID = 'mpi_win_do_loop.cpp'
source_filename = "mpi_win_do_loop.cpp"
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

; <label>:29:                                     ; preds = %47, %26
  %30 = load i32, i32* %8, align 4, !dbg !138
  %31 = icmp sgt i32 %30, 0, !dbg !141
  br i1 %31, label %32, label %38, !dbg !142

; <label>:32:                                     ; preds = %29
  %33 = bitcast i32* %8 to i8*, !dbg !143
  %34 = load i32, i32* %8, align 4, !dbg !145
  %35 = sext i32 %34 to i64, !dbg !145
  %36 = load i32, i32* %9, align 4, !dbg !146
  %37 = call i32 @MPI_Get(i8* %33, i32 1, i32 1275069445, i32 0, i64 %35, i32 1, i32 1275069445, i32 %36), !dbg !147
  br label %44, !dbg !148

; <label>:38:                                     ; preds = %29
  %39 = bitcast i32* %8 to i8*, !dbg !149
  %40 = load i32, i32* %8, align 4, !dbg !151
  %41 = sext i32 %40 to i64, !dbg !151
  %42 = load i32, i32* %9, align 4, !dbg !152
  %43 = call i32 @MPI_Put(i8* %39, i32 1, i32 1275069445, i32 0, i64 %41, i32 1, i32 1275069445, i32 %42), !dbg !153
  br label %44

; <label>:44:                                     ; preds = %38, %32
  %45 = load i64, i64* %10, align 8, !dbg !154
  %46 = add i64 %45, 1, !dbg !154
  store i64 %46, i64* %10, align 8, !dbg !154
  br label %47, !dbg !155

; <label>:47:                                     ; preds = %44
  %48 = load i64, i64* %10, align 8, !dbg !156
  %49 = icmp ult i64 %48, 3, !dbg !157
  br i1 %49, label %29, label %50, !dbg !155, !llvm.loop !136

; <label>:50:                                     ; preds = %47
  %51 = load i64, i64* %10, align 8, !dbg !158
  %52 = icmp ne i64 0, %51, !dbg !160
  br i1 %52, label %53, label %56, !dbg !161

; <label>:53:                                     ; preds = %50
  %54 = load i32, i32* %9, align 4, !dbg !162
  %55 = call i32 @MPI_Win_fence(i32 0, i32 %54), !dbg !164
  br label %56, !dbg !165

; <label>:56:                                     ; preds = %53, %50
  %57 = call i32 @MPI_Win_free(i32* %9), !dbg !166
  %58 = call i32 @MPI_Comm_free(i32* %7), !dbg !167
  %59 = call i32 @MPI_Finalize(), !dbg !168
  ret i32 0, !dbg !169
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @MPI_Init(i32*, i8***) #2

declare i32 @MPI_Comm_rank(i32, i32*) #2

declare i32 @MPI_Comm_split(i32, i32, i32, i32*) #2

; Function Attrs: noinline optnone uwtable
define linkonce_odr i32 @_Z13create_windowImEiiPT_b(i32, i64*, i1 zeroext) #3 comdat !dbg !170 {
  %4 = alloca i32, align 4
  %5 = alloca i64*, align 8
  %6 = alloca i8, align 1
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !177, metadata !DIExpression()), !dbg !178
  store i64* %1, i64** %5, align 8
  call void @llvm.dbg.declare(metadata i64** %5, metadata !179, metadata !DIExpression()), !dbg !180
  %9 = zext i1 %2 to i8
  store i8 %9, i8* %6, align 1
  call void @llvm.dbg.declare(metadata i8* %6, metadata !181, metadata !DIExpression()), !dbg !182
  call void @llvm.dbg.declare(metadata i32* %7, metadata !183, metadata !DIExpression()), !dbg !184
  call void @llvm.dbg.declare(metadata i32* %8, metadata !185, metadata !DIExpression()), !dbg !186
  %10 = load i32, i32* %4, align 4, !dbg !187
  store i32 %10, i32* %8, align 4, !dbg !186
  %11 = load i64*, i64** %5, align 8, !dbg !188
  %12 = bitcast i64* %11 to i8*, !dbg !188
  %13 = load i32, i32* %8, align 4, !dbg !189
  %14 = call i32 @MPI_Win_create(i8* %12, i64 8, i32 1, i32 469762048, i32 %13, i32* %7), !dbg !190
  %15 = load i32, i32* %7, align 4, !dbg !191
  ret i32 %15, !dbg !192
}

declare i32 @MPI_Get(i8*, i32, i32, i32, i64, i32, i32, i32) #2

declare i32 @MPI_Win_fence(i32, i32) #2

declare i32 @MPI_Put(i8*, i32, i32, i32, i64, i32, i32, i32) #2

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
!1 = !DIFile(filename: "mpi_win_do_loop.cpp", directory: "/home/marcelhb/Sources/master_docs/passes/phasar/tests")
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
!137 = !DILocation(line: 80, column: 17, scope: !93)
!138 = !DILocation(line: 55, column: 9, scope: !139)
!139 = distinct !DILexicalBlock(scope: !140, file: !1, line: 55, column: 9)
!140 = distinct !DILexicalBlock(scope: !93, file: !1, line: 54, column: 6)
!141 = !DILocation(line: 55, column: 14, scope: !139)
!142 = !DILocation(line: 55, column: 9, scope: !140)
!143 = !DILocation(line: 57, column: 11, scope: !144)
!144 = distinct !DILexicalBlock(scope: !139, file: !1, line: 55, column: 19)
!145 = !DILocation(line: 61, column: 11, scope: !144)
!146 = !DILocation(line: 64, column: 11, scope: !144)
!147 = !DILocation(line: 56, column: 7, scope: !144)
!148 = !DILocation(line: 66, column: 5, scope: !144)
!149 = !DILocation(line: 68, column: 11, scope: !150)
!150 = distinct !DILexicalBlock(scope: !139, file: !1, line: 66, column: 12)
!151 = !DILocation(line: 72, column: 11, scope: !150)
!152 = !DILocation(line: 75, column: 11, scope: !150)
!153 = !DILocation(line: 67, column: 7, scope: !150)
!154 = !DILocation(line: 79, column: 7, scope: !140)
!155 = !DILocation(line: 80, column: 3, scope: !140)
!156 = !DILocation(line: 80, column: 12, scope: !93)
!157 = !DILocation(line: 80, column: 14, scope: !93)
!158 = !DILocation(line: 82, column: 12, scope: !159)
!159 = distinct !DILexicalBlock(scope: !93, file: !1, line: 82, column: 7)
!160 = !DILocation(line: 82, column: 9, scope: !159)
!161 = !DILocation(line: 82, column: 7, scope: !93)
!162 = !DILocation(line: 83, column: 22, scope: !163)
!163 = distinct !DILexicalBlock(scope: !159, file: !1, line: 82, column: 15)
!164 = !DILocation(line: 83, column: 5, scope: !163)
!165 = !DILocation(line: 84, column: 3, scope: !163)
!166 = !DILocation(line: 86, column: 3, scope: !93)
!167 = !DILocation(line: 88, column: 3, scope: !93)
!168 = !DILocation(line: 90, column: 3, scope: !93)
!169 = !DILocation(line: 92, column: 3, scope: !93)
!170 = distinct !DISubprogram(name: "create_window<unsigned long>", linkageName: "_Z13create_windowImEiiPT_b", scope: !1, file: !1, line: 4, type: !171, isLocal: false, isDefinition: true, scopeLine: 4, flags: DIFlagPrototyped, isOptimized: false, unit: !0, templateParams: !175, variables: !2)
!171 = !DISubroutineType(types: !172)
!172 = !{!117, !4, !173, !174}
!173 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !67, size: 64)
!174 = !DIBasicType(name: "bool", size: 8, encoding: DW_ATE_boolean)
!175 = !{!176}
!176 = !DITemplateTypeParameter(name: "T", type: !67)
!177 = !DILocalVariable(name: "comm", arg: 1, scope: !170, file: !1, line: 4, type: !4)
!178 = !DILocation(line: 4, column: 33, scope: !170)
!179 = !DILocalVariable(name: "mem", arg: 2, scope: !170, file: !1, line: 4, type: !173)
!180 = !DILocation(line: 4, column: 42, scope: !170)
!181 = !DILocalVariable(name: "flag", arg: 3, scope: !170, file: !1, line: 4, type: !174)
!182 = !DILocation(line: 4, column: 52, scope: !170)
!183 = !DILocalVariable(name: "window", scope: !170, file: !1, line: 5, type: !117)
!184 = !DILocation(line: 5, column: 11, scope: !170)
!185 = !DILocalVariable(name: "comm_to_use", scope: !170, file: !1, line: 6, type: !4)
!186 = !DILocation(line: 6, column: 12, scope: !170)
!187 = !DILocation(line: 6, column: 26, scope: !170)
!188 = !DILocation(line: 9, column: 7, scope: !170)
!189 = !DILocation(line: 13, column: 7, scope: !170)
!190 = !DILocation(line: 8, column: 3, scope: !170)
!191 = !DILocation(line: 17, column: 10, scope: !170)
!192 = !DILocation(line: 17, column: 3, scope: !170)
