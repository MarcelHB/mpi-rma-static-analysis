require "benchmark"
require "erb"
require "fileutils"

# This test generator works for the scalability analysis of a roughly
# constant number of instructions over a growing number of facts.
#
# It generates a set of ll-files containing an increasing number of
# branches, running them multiple times in benchmark mode, printing
# a CSV compatible output. Less branches are compensated over a higher
# number of instructions - as conditional branches cost instructions
# for their own.

MPI_ANALYSIS_EXECUTABLE="../build/mpi_analysis"
# number of executions to average runtime
RUNS = 5
# (min_size..max_size).step(increase_rate)
CLOCK = (1..15).step(1)

CXX_TEMPLATE = <<~CODE
  #include <mpi.h>

  int main (int argc, char **argv) {
    MPI_Init (&argc, &argv);

    size_t a = 0, b = 1;

    <%= variables.join("\n") %>

    MPI_Win window;
    MPI_Win_create(
        nullptr
      , 0
      , 0
      , MPI_INFO_NULL
      , MPI_COMM_WORLD
      , &window
    );

    <%= branches.join("\n") %>

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
  a += 1;
  b = a + 1;
CODE

BRANCH_CODE = <<~CODE
  if (1 == a_%{id}) {
    b += 1;
    // "busy": MPI_Win_fence(0, window);
  }
CODE

# --- RUNNER ---

path = "test_scale_facts"
FileUtils.mkdir_p(path)

template = ERB.new(CXX_TEMPLATE)

CLOCK.each do |c|
  file_path = File.join(path, "facts_#{c}.cpp")

  variables = c.times.map { |i| "size_t a_#{i} = 0;" }
  branches = c.times.map { |i| BRANCH_CODE % {id: i } }

  unless File.exists?(file_path)
    instructions = Array.new(CLOCK.max - c, STRETCH_CODE)

    File.open(file_path, "w") do |f|
      f.write(template.result(binding))
    end

    result_file = file_path.gsub(/\.cpp$/, ".ll")

    %x(mpicxx -cxx=clang -S -emit-llvm -O0 #{file_path} -o #{result_file})
  end
end

puts %w(num time).join(",")

lazy_ps = "-L" if ARGV.include?("-L")

CLOCK.each do |c|
  file_path = File.join(path, "facts_#{c}.ll")

  result = Benchmark.measure do |bm|
    RUNS.times do
      %x(#{MPI_ANALYSIS_EXECUTABLE} #{lazy_ps} -OR -B2 #{file_path})
    end

    if $?.exitstatus != 0
      STDERR.puts "! Crashed for file #{file_path}."
      exit(1)
    end
  end

  time = "%.4f" % (result.total / RUNS)

  puts [c, time].join(",")
end
