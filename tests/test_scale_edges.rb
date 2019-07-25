require "benchmark"
require "erb"
require "fileutils"

# This test generator works for the scalability analysis of a constant
# number of facts over a growing number of instructions (edges).
#
# It generates a set of ll-files containing an increasing number of
# garbage, running them multiple times in benchmark mode, printing
# a CSV compatible output.

MPI_ANALYSIS_EXECUTABLE="../build/mpi_analysis"
# number of executions to average runtime
RUNS = 5
# (min_size..max_size).step(increase_rate)
CLOCK = (20..1_000).step(20)

CXX_TEMPLATE = <<~CODE
  #include <mpi.h>

  struct SomeClass {
    bool a;
    volatile unsigned long b;

    SomeClass () : a(true), b(1) {}
  };

  int main (int argc, char **argv) {
    MPI_Init (&argc, &argv);

    SomeClass A, B;
    SomeClass *toA = &A, *toB = &B;

    MPI_Win window;
    MPI_Win_create(
        nullptr
      , 0
      , 0
      , MPI_INFO_NULL
      , MPI_COMM_WORLD
      , &window
    );

    {
    <%= instructions.join("\n") %>
    }

    MPI_Win_free(&window);

    MPI_Finalize();

    return 0;
  }
CODE

# the garbage code to repeat
STRETCH_CODE = <<~CODE
  toA->b = toB->b;
  toB->b = toA->b;
CODE

# --- RUNNER ---

path = "test_scale_edges"
FileUtils.mkdir_p(path)

template = ERB.new(CXX_TEMPLATE)

CLOCK.each do |c|
  file_path = File.join(path, "edges_#{c}.cpp")

  unless File.exists?(file_path)
    instructions = Array.new(c, STRETCH_CODE)

    File.open(file_path, "w") do |f|
      f.write(template.result(binding))
    end

    result_file = file_path.gsub(/\.cpp$/, ".ll")

    %x(mpicxx -cxx=clang -S -emit-llvm -O0 #{file_path} -o #{result_file})
  end
end

puts %w(num time).join(",")

CLOCK.each do |c|
  file_path = File.join(path, "edges_#{c}.ll")

  result = Benchmark.measure do |bm|
    RUNS.times do
      %x(#{MPI_ANALYSIS_EXECUTABLE} -OR -B2 #{file_path})
    end
  end

  time = "%.4f" % (result.total / RUNS)

  puts [c, time].join(",")
end
