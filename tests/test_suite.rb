require "csv"
require "fileutils"
require "rspec"
require "rspec/expectations"
require "securerandom"
require "timeout"

$executable = ENV.fetch("MPI_ANALYSIS_BIN", "../build/mpi_analysis")
$sample_directory = ENV.fetch("MPI_ANALYSIS_TEST_LOCATION", ".")

TIMEOUT_SECONDS = 10

RSpec.shared_context(:mpi_analysis_setup) do |ll_file, flags=""|
  let(:context_directory) { @context_directory }

  before(:all) do
    @test_context = SecureRandom.hex(4)
    @context_directory = "test_#{@test_context}"
    file_location = File.join($sample_directory, ll_file)

    FileUtils.mkdir_p(@context_directory)
    Timeout.timeout(TIMEOUT_SECONDS) do
      %x(#{$executable} #{flags} -o #{@context_directory}/ #{file_location})
    end
  end

  after(:all) do
    FileUtils.rm_rf(@context_directory)
  end

  it { expect($?.exitstatus).to(eq(0)) }
end

RSpec.describe("MPI RMA analysis") do
  let(:csv_options) { { col_sep: "," } }
  let(:violations_path) { File.join(context_directory, "violations.csv") }

  let(:statistics_sheet) { CSV.read(File.join(context_directory, "statistics.csv"), csv_options)[1..-1] }
  let(:violations_sheet) { CSV.read(violations_path, csv_options)[1..-1] }
  let(:window_sheet)     { CSV.read(File.join(context_directory, "windows.csv"), csv_options)[1..-1] }

  let(:free_of_violations) { !File.exists?(violations_path) }

  let(:grouped_windows) do
    window_sheet.group_by { |win| win[0] }
      .map do |_, v|
        {
          id: v.first[0],
          status: v.first[1],
          rank_value: v.first[2],
          rank_pred: v.first[3],
          code_lines: v.map { |val| val[4] },
          instructions: v.map { |val| val[5] },
        }
      end
  end

  let(:grouped_violations) do
    violations_sheet.group_by { |v| v[0] }
      .map do |_, v|
        {
          id: v.first[0],
          violations: v.map { |val| val[1] },
          code_lines: v.map { |val| val[2] },
          instructions: v.map { |val| val[3] },
        }
      end
  end

  context "analysis: concurrent local access" do
    include_context :mpi_analysis_setup, "mpi_win_concurrent_local.ll"

    it "detects a local win memory access while being targeted" do
      expect(grouped_violations.length).to eq(1)
      expect(grouped_violations.first[:violations].length).to eq(1)
      expect(grouped_violations.first[:violations].first).to eq("concurrent_local_access")
      expect(grouped_violations.first[:code_lines].first).to eq("mpi_win_concurrent_local.cpp:24: main")
    end
  end

  context "analysis: concurrent local and shared access" do
    include_context :mpi_analysis_setup, "mpi_win_concurrent_shared_local.ll"

    it "detects a local win memory access while being targeted, even in shared mode" do
      expect(grouped_violations.length).to eq(1)
      expect(grouped_violations.first[:violations].length).to eq(1)
      expect(grouped_violations.first[:violations].first).to eq("concurrent_local_access")
      expect(grouped_violations.first[:code_lines].first).to eq("mpi_win_concurrent_shared_local.cpp:28: main")
    end
  end

  context "analysis: inter-rank expose analysis" do
    include_context :mpi_analysis_setup, "mpi_win_no_expose.ll"

    it "detects the comm request not having a valid counterpart path" do
      expect(grouped_violations.length).to eq(1)
      expect(grouped_violations.first[:violations].length).to eq(1)
      expect(grouped_violations.first[:violations].first).to eq("no_corresponding_rank")
      expect(grouped_violations.first[:code_lines].first).to eq("mpi_win_no_expose.cpp:32: main")
    end
  end

  context "analysis: inter-rank fence-lock analysis" do
    include_context :mpi_analysis_setup, "mpi_win_lock_all_fence.ll"

    it "accepts hardly acceptable lock-fence interactions" do
      expect(free_of_violations).to be(true)
    end
  end

  context "analysis: inter-rank fence-lock analysis (2)" do
    include_context :mpi_analysis_setup, "mpi_win_lock_all_fence2.ll"

    it "accepts hardly acceptable lock-fence interactions" do
      expect(free_of_violations).to be(true)
    end
  end

  context "analysis: rank contradiction analysis" do
    include_context :mpi_analysis_setup, "mpi_win_rank_merging.ll"

    it "recognizes paths that intrinsically contradict each other over branch conditions" do
      expect(grouped_violations.length).to eq(2)
      expect(grouped_violations.all? { |v| v[:violations].length == 1}).to be(true)
      expect(grouped_violations.all? { |v| v[:violations].all? { |vl| vl == "elimination_by_rank" }}).to be(true)
      expect(grouped_violations.all? { |v| v[:code_lines].first == "mpi_win_rank_merging.cpp:30: main" }).to be(true)
    end
  end

  context "analysis: rank contradiction analysis with dynamic rank constraint" do
    context "in regular mode" do
      include_context :mpi_analysis_setup, "mpi_win_rank_dynamic.ll"

      it "recognizes paths that intrinsically contradict each other over branch conditions, over changing constraints" do
        expect(grouped_violations.length).to eq(1)
        expect(grouped_violations.first[:violations].length).to eq(1)
        expect(grouped_violations.first[:violations].first).to eq("elimination_by_rank")
      end
    end

    context "in regular mode" do
      include_context :mpi_analysis_setup, "mpi_win_rank_dynamic.ll", "-OI"

      it "will ignore changing rank-based constraints" do
        expect(free_of_violations).to be(true)
      end
    end
  end

  context "analysis: stuck barriers" do
    include_context :mpi_analysis_setup, "mpi_win_stuck_barrier.ll"

    it "detects the stucking barrier & fence call" do
      expect(grouped_violations.length).to eq(2)
      expect(grouped_violations.all? { |v| v[:violations].length == 1}).to be(true)
      expect(grouped_violations.all? { |v| v[:violations].all? { |vl| vl == "waits_forever" }}).to be(true)
      expect(grouped_violations.any? { |v| v[:code_lines].first == "mpi_win_stuck_barrier.cpp:44: main" }).to be(true)
      expect(grouped_violations.any? { |v| v[:code_lines].first == "mpi_win_stuck_barrier.cpp:57: main" }).to be(true)
    end
  end

  context "analysis: stuck fences" do
    include_context :mpi_analysis_setup, "mpi_win_stuck_fence.ll"

    it "detects the stucking fence call" do
      expect(grouped_violations.length).to eq(1)
      expect(grouped_violations.first[:violations].length).to eq(1)
      expect(grouped_violations.first[:violations].first).to eq("waits_forever")
      expect(grouped_violations.first[:code_lines].first).to eq("mpi_win_stuck_fence.cpp:58: main")
    end
  end

  context "analysis: unsafe local access" do
    include_context :mpi_analysis_setup, "mpi_win_unsafe_local_access.ll"

    it "detects an unsafe local access" do
      expect(grouped_violations.length).to eq(2)
      expect(grouped_violations.all? { |v| v[:violations].length == 1}).to be(true)
      expect(grouped_violations.all? { |v| v[:violations].all? { |vl| vl == "unsafe_local_memory_access" }}).to be(true)
      expect(grouped_violations.any? { |v| v[:code_lines].first == "mpi_win_unsafe_local_access.cpp:36: main" }).to be(true)
      expect(grouped_violations.any? { |v| v[:code_lines].first == "mpi_win_unsafe_local_access.cpp:49: main" }).to be(true)
    end
  end

  context "analysis: unsafe local access over blind index" do
    include_context :mpi_analysis_setup, "mpi_win_unsafe_local_blind.ll"

    it "detects an unsafe local access on an arbitrary index" do
      expect(grouped_violations.length).to eq(1)
      expect(grouped_violations.first[:violations].length).to eq(1)
      expect(grouped_violations.first[:violations].first).to eq("unsafe_local_memory_access")
      expect(grouped_violations.first[:code_lines].first).to eq("mpi_win_unsafe_local_blind.cpp:41: main")
    end
  end

  context "analysis: window mode memory handling" do
    include_context :mpi_analysis_setup, "mpi_win_dyn_memory.ll"

    it "detects the use of window memory alteration despite wrong mode" do
      expect(grouped_violations.length).to eq(1)
      expect(grouped_violations.first[:violations].length).to eq(1)
      expect(grouped_violations.first[:violations].first).to eq("illegal_dynamic_memory_use")
      expect(grouped_violations.first[:code_lines].first).to eq("mpi_win_dyn_memory.cpp:39: main")
    end
  end

  context "analysis: window modes" do
    include_context :mpi_analysis_setup, "mpi_win_incompat_mode.ll"

    it "detects the mismatching window types" do
      expect(grouped_violations.length).to eq(2)
      expect(grouped_violations.all? { |v| v[:violations].length == 1 }).to be(true)
      expect(grouped_violations.all? { |v| v[:violations].first == "window_flavors_incompatible" }).to be(true)
    end
  end

  context "barriers" do
    include_context :mpi_analysis_setup, "mpi_win_barrier.ll"

    it "recognizes barriers" do
      expect(grouped_windows.count).to eq(1)
      expect(grouped_windows.all? { |w| w[:status] == "valid" }).to be(true)
      expect(grouped_windows.all? { |w| w[:code_lines].include?("mpi_win_barrier.cpp:34: main") }).to be(true)
    end
  end

  context "barriers, multiple ones" do
    include_context :mpi_analysis_setup, "mpi_win_barriers.ll"

    it "recognizes barriers that are correctly assigned to the window's comm." do
      expect(grouped_windows.count).to eq(1)
      expect(grouped_windows.none? { |w| w[:code_lines].include?("mpi_win_barriers.cpp:42: main") }).to be(true)
      expect(grouped_windows.all? { |w| w[:code_lines].include?("mpi_win_barriers.cpp:43: main") }).to be(true)
    end
  end

  context "Fortran/Flang quirks" do
    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_fortran.ll", "-e MAIN_"

      it "generates three valid windows" do
        expect(grouped_windows.count).to eq(3)
        expect(grouped_windows.all? { |w| w[:status] == "valid" }).to be(true)
      end

      it "only passes the call in the nested loop once" do
        expect(grouped_windows.select { |w| w[:code_lines].include?("mpi_win_fortran.f90:25: main") }.length).to eq(1)
      end
    end

    context "lazy mode" do
      include_context :mpi_analysis_setup, "mpi_win_fortran.ll", "-e MAIN_ -L"

      it "generates three valid windows" do
        expect(grouped_windows.count).to eq(3)
        expect(grouped_windows.all? { |w| w[:status] == "valid" }).to be(true)
      end

      it "only passes the call in the nested loop once" do
        expect(grouped_windows.select { |w| w[:code_lines].include?("mpi_win_fortran.f90:25: main") }.length).to eq(1)
      end
    end
  end

  context "interproc. window setup" do
    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_interproc.ll"

      it "generates two valid execution paths" do
        expect(grouped_windows.count).to eq(2)
        expect(grouped_windows.all? { |w| w[:status] == "valid" }).to be(true)
      end
    end

    context "lazy mode" do
      include_context :mpi_analysis_setup, "mpi_win_interproc.ll", "-L"

      it "generates two valid execution paths" do
        expect(grouped_windows.count).to eq(2)
        expect(grouped_windows.all? { |w| w[:status] == "valid" }).to be(true)
      end
    end
  end

  context "interproc. window setup with comm param" do
    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_interproc_comm.ll"

      it "generates 3 paths, two of them invalid" do
        expect(grouped_windows.count).to eq(3)
        expect(grouped_windows.select { |w| w[:status] == "valid" }.count).to eq(1)
        expect(grouped_windows.select { |w| w[:status] == "violation" }.count).to eq(2)
      end
    end

    context "lazy mode" do
      include_context :mpi_analysis_setup, "mpi_win_interproc_comm.ll", "-L"

      it "generates 3 paths, two of them invalid" do
        expect(grouped_windows.count).to eq(3)
        expect(grouped_windows.select { |w| w[:status] == "valid" }.count).to eq(1)
        expect(grouped_windows.select { |w| w[:status] == "violation" }.count).to eq(2)
      end
    end
  end

  context "loops (do-loop), early exit" do
    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_do_loop.ll"

      it "generates 5 paths, three of them invalid" do
        expect(grouped_windows.count).to eq(5)
        expect(grouped_windows.select { |w| w[:status] == "valid" }.count).to eq(2)
        expect(grouped_windows.select { |w| w[:status] == "violation" }.count).to eq(3)

        # do loops must be passed!
        expect(grouped_windows.all? do |w|
          w[:instructions].any? { |i| i =~ /@MPI_Put/ } ||
          w[:instructions].any? { |i| i =~ /@MPI_Get/ }
        end).to be(true)
      end
    end

    context "lazy mode" do
      include_context :mpi_analysis_setup, "mpi_win_do_loop.ll", "-L"

      it "generates 5 paths, three of them invalid" do
        expect(grouped_windows.count).to eq(5)
        expect(grouped_windows.select { |w| w[:status] == "valid" }.count).to eq(2)
        expect(grouped_windows.select { |w| w[:status] == "violation" }.count).to eq(3)

        # do loops must be passed!
        expect(grouped_windows.all? do |w|
          w[:instructions].any? { |i| i =~ /@MPI_Put/ } ||
          w[:instructions].any? { |i| i =~ /@MPI_Get/ }
        end).to be(true)
      end
    end
  end

  context "loops (while-loop), early exit" do
    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_loop.ll"

      it "generates 7 paths, three of them invalid" do
        expect(grouped_windows.count).to eq(7)
        expect(grouped_windows.select { |w| w[:status] == "valid" }.count).to eq(4)
        expect(grouped_windows.select { |w| w[:status] == "violation" }.count).to eq(3)
      end
    end

    context "lazy mode" do
      include_context :mpi_analysis_setup, "mpi_win_loop.ll", "-L"

      it "generates 7 paths, three of them invalid" do
        expect(grouped_windows.count).to eq(7)
        expect(grouped_windows.select { |w| w[:status] == "valid" }.count).to eq(4)
        expect(grouped_windows.select { |w| w[:status] == "violation" }.count).to eq(3)
      end
    end
  end

  context "MPI integration" do
    include_context :mpi_analysis_setup, "mpi_win_fence_flags.ll"

    it "generates two paths, one valid, one with a validation on linked MPI (may fail if not compiled with MPICH!)" do
      expect(grouped_windows.count).to eq(2)
      expect(grouped_windows.select { |w| w[:status] == "valid" }.count).to eq(1)
      expect(grouped_windows.select { |w| w[:status] == "violation" }.count).to eq(1)

      failed_path = grouped_windows.select { |w| w[:status] == "violation" }.first
      expect(failed_path[:code_lines]).to include("mpi_win_fence_flags.cpp:35: main")
    end
  end

  context "parallel, overlapping windows" do
    include_context :mpi_analysis_setup, "mpi_win_parallel.ll"

    it "detects two overlapping windows per branch being stuck by their creation order" do
      expect(grouped_violations.length).to eq(2)
      expect(grouped_violations.all? { |w| w[:violations].all? { |vl| vl == "waits_forever" } }).to be(true)
      expect(grouped_violations.any? { |w| w[:code_lines].all? { |cl| cl == "mpi_win_parallel.cpp:44: main" }}).to be(true)
      expect(grouped_violations.any? { |w| w[:code_lines].all? { |cl| cl == "mpi_win_parallel.cpp:78: main" }}).to be(true)
    end
  end

  context "rank skipping" do
    context "enabled" do
      include_context :mpi_analysis_setup, "mpi_wormhole.ll", "-OR"

      it "does not recognize any integer assignments" do
        expect(statistics_sheet.first[4].to_i).to eq(0)
      end
    end

    context "disabled" do
      include_context :mpi_analysis_setup, "mpi_wormhole.ll"

      it "normally collects all integer assignments" do
        expect(statistics_sheet.first[4].to_i).to eq(9)
      end
    end
  end

  context "structs, handles" do
    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_struct.ll"

      it "finds window handles used in pointered structs (test uses aliases, thus incomplete track)" do
        expect(grouped_windows.count).to eq(1)
        expect(grouped_windows.first[:status]).to eq("violation")
        expect(grouped_windows.first[:code_lines].length).to eq(2)
        expect(grouped_windows.first[:code_lines][0]).to eq("mpi_win_struct.cpp:28: main")
        expect(grouped_windows.first[:code_lines][1]).to eq("mpi_win_struct.cpp:50: main")
      end
    end

    context "lazy mode" do
      include_context :mpi_analysis_setup, "mpi_win_struct.ll", "-L"

      it "finds window handles used in pointered structs (test uses aliases, thus incomplete track)" do
        expect(grouped_windows.count).to eq(1)
        expect(grouped_windows.first[:status]).to eq("violation")
        expect(grouped_windows.first[:code_lines].length).to eq(2)
        expect(grouped_windows.first[:code_lines][0]).to eq("mpi_win_struct.cpp:28: main")
        expect(grouped_windows.first[:code_lines][1]).to eq("mpi_win_struct.cpp:50: main")
      end
    end
  end

  context "subsequent windows" do
    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_subseq.ll"

      it "correctly splits two windows following each other" do
        expect(grouped_windows.count).to eq(4)
        expect(grouped_windows.all? { |w| w[:status] == "valid" }).to be(true)
        expect(grouped_windows.select { |w| w[:code_lines].include?("mpi_win_subseq.cpp:47: main") }.count).to eq(2)
        expect(grouped_windows.select { |w| w[:code_lines].include?("mpi_win_subseq.cpp:75: main") }.count).to eq(2)
      end
    end

    context "lazy mode" do
      include_context :mpi_analysis_setup, "mpi_win_subseq.ll", "-L"

      it "correctly splits two windows following each other" do
        expect(grouped_windows.count).to eq(4)
        expect(grouped_windows.all? { |w| w[:status] == "valid" }).to be(true)
        expect(grouped_windows.select { |w| w[:code_lines].include?("mpi_win_subseq.cpp:47: main") }.count).to eq(2)
        expect(grouped_windows.select { |w| w[:code_lines].include?("mpi_win_subseq.cpp:75: main") }.count).to eq(2)
      end
    end
  end

  context "wormholing" do
    context "enabled" do
      include_context :mpi_analysis_setup, "mpi_wormhole.ll", "-s -OW"

      it "shortens the graph into one relevant branch" do
        expect(statistics_sheet.first[2].to_i).to eq(1)
      end
    end

    context "disabled" do
      include_context :mpi_analysis_setup, "mpi_wormhole.ll"

      it "leaves all branches available" do
        expect(statistics_sheet.first[2].to_i).to eq(6)
      end
    end
  end

  context "branch-wormholing" do
    context "enabled" do
      include_context :mpi_analysis_setup, "mpi_wormhole2.ll", "-s -OW"

      it "skips disjunctive and conjunctive conditions around MPI" do
        expect(statistics_sheet.first[2].to_i).to eq(3)
      end
    end

    context "disabled" do
      include_context :mpi_analysis_setup, "mpi_wormhole2.ll"

      it "leaves all branches available" do
        expect(statistics_sheet.first[2].to_i).to eq(8)
      end
    end
  end

  context "window-picking" do
    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_parallel.ll", "-w 3"

      it "only picks the the third window out of 4" do
        expect(grouped_windows.count).to eq(1)
        expect(grouped_windows.first[:code_lines].first).to eq("mpi_win_parallel.cpp:53: main")
      end
    end

    context "default mode" do
      include_context :mpi_analysis_setup, "mpi_win_parallel.ll", "-w 3 -L"

      it "only picks the the third window out of 4" do
        expect(grouped_windows.count).to eq(1)
        expect(grouped_windows.first[:code_lines].first).to eq("mpi_win_parallel.cpp:53: main")
      end
    end
  end

  context "switch rewriting" do
    include_context :mpi_analysis_setup, "switch.ll"

    it "rewrites the switch instruction correctly" do
      expect(statistics_sheet.first[2].to_i).to eq(2)
    end
  end
end
