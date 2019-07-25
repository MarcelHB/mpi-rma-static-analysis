cmake_minimum_required(VERSION 3.10)

include(CheckIncludeFile)

SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_COMPILER clang++)

project(mpi_rma_phasar_plugin)
set(CMAKE_CXX_STANDARD 14)

find_package(LLVM 5.0 REQUIRED CONFIG)
find_package(MPI)

check_include_file("signal.h" WITH_SIGNAL)
check_include_file("sys/resource.h" WITH_SYS_RESOURCE)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} in ${LLVM_DIR}")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# Curl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# SQL
find_path(SQLITE3_INCLUDE_DIR NAMES sqlite3.h)
find_library(SQLITE3_LIBRARY NAMES sqlite3)
include_directories(${SQLITE3_INCLUDE_DIR})

llvm_map_components_to_libnames(llvm_libs
  coverage
  coroutines
  libdriver
  lto
  support
  analysis
  bitwriter
  core
  ipo
  irreader
  instcombine
  instrumentation
  linker
  objcarcopts
  scalaropts
  transformutils
  codegen
  vectorize
)
add_definitions(${LLVM_DEFINITIONS})

find_library(CLANG_LIBRARY 5.0 NAMES clang REQUIRED)
link_directories(${CLANG_LIB_PATH})
set(CLANG_LIBRARIES
 clangTooling
 clangFrontendTool
 clangFrontend
 clangDriver
 clangSerialization
 clangCodeGen
 clangParse
 clangSema
 clangStaticAnalyzerFrontend
 clangStaticAnalyzerCheckers
 clangStaticAnalyzerCore
 clangAnalysis
 clangARCMigrate
 clangRewrite
 clangRewriteFrontend
 clangEdit
 clangAST
 clangASTMatchers
 clangLex
 clangBasic
 )

find_package(Threads)

find_package(Boost COMPONENTS filesystem graph system program_options log thread REQUIRED)
include_directories(${BOOST_INCLUDE_DIR})
add_definitions(-DBOOST_LOG_DYN_LINK)

add_executable(mpi_analysis
  src/AnalysisController.cpp
  src/BlockClassification.cpp
  src/CommSynchronizationCheck.cpp
  src/ConcurrentCommCheck.cpp
  src/DynamicMemoryRequestCheck.cpp
  src/EpochMatcher.cpp
  src/Fact.cpp
  src/FactKiller.cpp
  src/GraphSlicer.cpp
  src/InstructionDump.cpp
  src/LazyFact.cpp
  src/LazyFactKiller.cpp
  src/LazyMPIRMAUseProblem.cpp
  src/LazySymbolicExecutor.cpp
  src/LazyWindowCreator.cpp
  src/LazyWindowSheet.cpp
  src/LLVMBasedMICFG.cpp
  src/MPIRMAUseProblem.cpp
  src/MOTFResolver.cpp
  src/PathGrouper.cpp
  src/WormholeMapGenerator.cpp
  src/StandaloneMain.cpp
  src/StatisticsFact.cpp
  src/StatisticsProblem.cpp
  src/StatisticsSheet.cpp
  src/SwitchConverter.cpp
  src/SymbolicExecutor.cpp
  src/RankAttributionAndContradictionCheck.cpp
  src/RankContradictionResolver.cpp
  src/UnlockMapGenerator.cpp
  src/UnsafeAccessCheck.cpp
  src/ViolationsSheet.cpp
  src/WindowCreator.cpp
  src/WindowModeCheck.cpp
  src/WindowPath.cpp
  src/WindowSheet.cpp
  src/WindowState.cpp
)

set_target_properties(mpi_analysis
  PROPERTIES COMPILE_FLAGS "-g -O0"
)

if(WITH_SIGNAL)
  target_compile_definitions(mpi_analysis PUBLIC WITH_SIGNAL=1)
endif()
if(WITH_SYS_RESOURCE)
  target_compile_definitions(mpi_analysis PUBLIC WITH_SYS_RESOURCE=1)
endif()

if(MPI_C_FOUND)
  message(STATUS "Compiling with MPI")

  include_directories(${MPI_C_INCLUDE_PATH})
  target_sources(mpi_analysis PRIVATE
    src/FenceFlagCheck.cpp
  )

  target_link_libraries(mpi_analysis ${MPI_C_LIBRARIES})
  target_compile_definitions(mpi_analysis PUBLIC WITH_MPI=1)
endif()

target_link_libraries(mpi_analysis
  phasar_controller
  phasar_db
  phasar_experimental
  phasar_clang
  phasar_controlflow
  phasar_ifdside
  phasar_mono
  phasar_passes
  phasar_plugins
  phasar_pointer
  phasar_config
  phasar_phasarllvm_utils
  phasar_utils
  boost_program_options
  boost_filesystem
  boost_graph
  boost_system
  boost_log
  ${BOOST_THREAD}
  ${SQLITE3_LIBRARY}
  ${Boost_LIBRARIES}
  ${CMAKE_DL_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
  ${CLANG_LIBRARIES}
  ${llvm_libs}
  mysqlcppconn
  curl
)

enable_testing()
add_test(NAME complete_suite
  COMMAND rspec ${CMAKE_SOURCE_DIR}/tests/test_suite.rb
)
set_tests_properties(complete_suite PROPERTIES
  ENVIRONMENT MPI_ANALYSIS_BIN=$<TARGET_FILE:mpi_analysis>
  ENVIRONMENT MPI_ANALYSIS_TEST_LOCATION=${CMAKE_SOURCE_DIR}/tests/
)

