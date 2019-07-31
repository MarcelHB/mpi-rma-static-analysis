# MPI RMA static analysis

An LLVM/Phasar-based tool to perform static analyses for different
properties of MPI RMA.

## License

Source code files without further copyright notices are subject to the MIT
license. Please consult the `LICENSE.txt` and `PHASAR_LICENSE.txt` 
file for details.

Parts of the code make use of a Phasar re-implementation, subject to:

```
Phasar Release License (MIT license)

Copyright (c) 2017 Philipp Schubert and others
```

## Requirements

The static analysis tool has been tested extensively under Linux. Although
platform-dependency is supposed to be optional, Linux is currently recommended.
The following software is required to be present on the computer when building
the tool from the source code:

* _CMake_ v3.10 or newer,
* a C++-14 compliant compiler,
* optionally, the MPI development package of the software to inspect.

## Setup

The following steps are mandatory to obtain a working binary program of 
the tool:

1. Obtain the source code of [Phasar](https://github.com/secure-software-engineering/phasar),
   checkout git tag `v1218` and install all of the required dependencies.
2. Build and install Phasar in its Release configuration.
3. Here again, execute: `$ mkdir build && cd build && cmake ..`
4. Check the output for matching dependencies and desired versions. When
   satisfied, run `$ make` (or the tool that CMake has set up).

## Tests

In order to run the test suite, a Ruby installation of v2.5 or newer is
required. The RSpec package is necessary and can be obtained by running
`$ gem install rspec`. Once done, tests can be started by launching a
CMake target (usually `$ make test`), or directly by entering the `tests/`
subdirectory and running `$ rspec test_suite.rb` 

## Usage

In the most general case, any LLVM IR file can be used directly with the
created binary. Refer to the next section for options provided by additional
tool flags. Running the tool, a decent level of activity is being printed
to the output by default. After finishing, there are up to three different
CSV files in the current directory:

* `windows.csv`: This file contains a list of window paths in the
  following columns:
    * `win_id`: This column groups instructions by the actual path identifier,
      with individual items sorted in ascending order by occurrence on the path.
    * `win_state`: The last known state of the window at the time of termination.
      It may be valid, or violation for having identified a violation no later
      than the last instruction of the current ID, or unfinished if no proper
      shutdown call was identified on this path.
    * `rank_pred`: If the path could be narrowed to match a certain rank criteria,
      this column contains a C-style operator as the predicate. `n/a` on failure
      or if the rank analysis is disabled.
    * `rank_value`: If the path could be narrowed to match a certain rank criteria,
      this column contains the integer value of the rank. Example: If `rank_pred: >=`,
      `rank_value: 1`, this path belongs to ranks greater or equal 1. To be ignored
      if the value of `rank_pred` is `n/a`.
    * `code_location`: If debug information is provided by the given IR file, the
      column shows the source code file name and a line number where to find the
      original code line.
    * `llvm_location`: This column prints a human-friendly LLVM IR instruction that
      has been identified as a path-relevant MPI action.
* `violations.csv`: This file lists any post-analysis violations or discards,
  i.e. FSM violations by illegal arrangement of instructions visible in
  `windows.csv` are not listed here, despite their violation status. The columns
  are given:
    * `win_id`: This is the path identifier where the violation has been observed;
      please refer to `windows.csv` for a complete look-up by this value.
    * `violation`: The violation code by any internal analysis.
    * `code_location`: If debug information is provided by the given IR file,
      the column shows the source code file name and a line number where to find
      the original code line.
    * `llvm_location`: This column prints a human-friendly LLVM IR instruction that
      has been identified as a path-relevant MPI action.
* `statistics.csv`: A machine-readable format containing the same number of
  statistics as given in the activity output of the preflight. Primarily intended
  for test purposes.

## Flags

When using the static analysis, several customizations of the analysis may come handy. Cur-
rently, the following flags can be passed to the tool:

**Flag** ......... **Description**

`-B1` ............ Running in benchmark mode B1. Skips writing anything to the CSV
                   files except the header row.

`-B2` ............ Running in benchmark mode B2. Includes the effect of `-B1` and skips
                   any post-analysis passes.

`-d` ............. Debug output. If set, Phasar prints its IFDS problem detail output,
                   suitable for inspecting fact lifetimes over individual instructions.

`-e ENTRY_POINT` Entry point, defaults to `main`. When entering the module anywhere but
                   `main`, then override this by `ENTRY_POINT`. For Fortran code, setting
                   `-e MAIN_` is likely to be required.

`-h, --help` ..... Prints an essential usage help to the output.

`-L` ............. Lazy mode IFDS problem model. Switches to the lazy IFDS problem path
                   sensitivity, currently skipping the post-IFDS problem analysis stage.

`-o PREFIX` ...... Output prefix, empty by default. Set `PREFIX` to some value if any
                   CSV file is supposed to be created in a path prefixed by `PREFIX`.

`-OI` ............ Optimization, assuming static rank values. If set, integer
                   assignments are considered immutable, resulting in a decrease of
                   facts to generate. Do not use if rank roles are reassigned.

`-OR` ............ Optimization, skip the inter-rank analysis passes. If set, no
                   actions are performed to attempt assigning paths to rank predicates,
                   and thus no inter-rank steps will take place.

`-OW` ............ Optimization, wormhole mode. If set, the tool tries to eliminate
                   sections from the CFG that do not contribute to any analysis.
                   This is a lossy optimization, but it may speed up the run time a
                   lot while possibly losing information such as local memory
                   operations. Refrain from using the option on the printing of
                   warnings about invalid IR.

`-s` ............. Preflight only. If set, stop after the initial analysis pass.
                   Useful for obtaining statistics to decide on further processing.

`-S` ............. Skip preflight. No statistics are collected.

`-v` ............. Prints version information, and also the MPI environment
                   information if it was built against some.

`-V` ............. Verbose progress. Linux only, displays a progress counter for each
                   created and finalized window during the IFDS problem reachability
                   analysis. Cannot be used together with `-S`.

`-w i` ........... Window isolation. A Win fact is generated on the i-th occurrence
                   of a window setup call only.

## Violations

In the file `violations.csv`, the column violation encodes the violation that has
been identified for the given instruction. These codes are given a description
below:

**Code** ...................... **Description**

`concurrent_local_access` ..... A local action takes place at the same time
                                when some other rank may trigger a virtual action
                                here. Under the separate memory model, this is not
                                allowed to happen, regardless of guarantees for
                                non-overlapping.

`elimination_by_rank` ......... This is not a violation but the information that a
                                window path described in windows.csv is no longer
                                valid for passing through contradicting rank predicates.

`fence_flags_mismatch` ........ For corresponding calls to `MPI_Win_fence`, there
                                is a mismatch in the flags with respect to
                                `MPI_MODE_NOPRECEDE` or `MPI_NODE_NOSUCCEED`.
                                This violation can be identified only if the analysis
                                tool was built against the correct MPI implementation.

`illegal_by_other_rank` ....... A call to `MPI_Win_lock_all` while there is at least
                                one path that is not lockable at that time.

`illegal_dynamic_memory_use` .. A call to `MPI_Win_attach`, `MPI_Win_detach` or
                                `MPI_Win_shared_query` that is illegal under
                                the setup call of the window.

`no_corresponding_rank` ....... A call for entering an access epoch without a
                                necessary window path that serves as a target.

`unsafe_local_memory_access` .. This violation indicates a conflict involving a
                                buffer action.

`waits_forever` ............... A call to `MPI_Barrier` or `MPI_Win_fence` without
                                a corresponding call on every other path of this
                                window or communicator.

`window_flavors_incompatible` . Not all windows at the same level uniformly follow
                                the shared or non-shared flavor.

