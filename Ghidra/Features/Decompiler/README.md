# Decompiler

Native C++ decompilation engine used by Ghidra.

## Directory Layout

    src/decompile/cpp/          Core C++ source and headers
    src/decompile/unittests/    C++ unit test files
    src/decompile/datatests/    XML-based integration test files

## Prerequisites

- g++ with C++11 support
- libbfd-dev (binutils development headers)
- zlib (libz)
- bison, flex (for grammar regeneration only)

## Building

All build commands are run from `src/decompile/cpp/`.

    make decomp_dbg          # Command-line decompiler (debug)
    make decomp_opt          # Command-line decompiler (optimized)
    make decomp_test_dbg     # Test runner
    make ghidra_opt          # Ghidra decompiler daemon (optimized)

Parallel builds are supported:

    make -j$(nproc) decomp_test_dbg

To override compiler flags (the test target auto-rebuilds when flags change):

    make -j$(nproc) decomp_test_dbg DBG_CXXFLAGS="-O2 -g -Wall -Wno-sign-compare"

## Testing

The test runner (`decomp_test_dbg`) supports two kinds of tests:

### Unit Tests

Small C++ tests in `src/decompile/unittests/`. Each test uses the `TEST(name)`
macro from `test.hh` with `ASSERT`, `ASSERT_EQUALS`, and `ASSERT_NOT_EQUALS`.

    ./decomp_test_dbg unittests                   # Run all unit tests
    ./decomp_test_dbg unittests testname           # Run a specific test

### Data Tests

XML-based integration tests in `src/decompile/datatests/`. Each XML file
contains a binary image, a script of decompiler commands, and regex assertions
against the decompiled C output.

    ./decomp_test_dbg datatests                    # Run all data tests
    ./decomp_test_dbg datatests sparseswitch.xml   # Run a specific data test

Running both:

    ./decomp_test_dbg

### SLEIGH Path

The test runner locates SLEIGH processor specifications relative to the source
tree by default (`../../../../../../..`). Override with:

    ./decomp_test_dbg -sleighpath /path/to/ghidra/Ghidra/Processors datatests

Or set the environment variable:

    SLEIGHHOME=/path/to/ghidra ./decomp_test_dbg -usesleighenv datatests

### Writing a Data Test

Create an XML file in `src/decompile/datatests/`:

```xml
<decompilertest>
<binaryimage arch="x86:LE:64:default:gcc">
<bytechunk space="ram" offset="0x100000" readonly="true">
hexbyteshere
</bytechunk>
<symbol space="ram" offset="0x100000" name="myfunc"/>
</binaryimage>
<script>
  <com>option readonly on</com>
  <com>parse line extern int4 myfunc(int4 param_1);</com>
  <com>lo fu myfunc</com>
  <com>decompile</com>
  <com>print C</com>
  <com>quit</com>
</script>
<stringmatch name="Test #1" min="1" max="1">regex pattern</stringmatch>
</decompilertest>
```

Fields:
- `arch`: SLEIGH architecture ID (e.g., `x86:LE:64:default:gcc`)
- `bytechunk`: raw hex bytes of the function at the given address
- `script`: decompiler console commands to execute
- `stringmatch`: regex matched against each line of the decompiled output.
  `min`/`max` specify the expected number of matching lines.

### Writing a Unit Test

Add a `.cc` file in `src/decompile/unittests/`. It is automatically picked up
by the Makefile (via wildcard). Include `test.hh` and the headers you need:

```cpp
#include "coreaction.hh"
#include "test.hh"

namespace ghidra {

TEST(my_test_name) {
  ASSERT(1 + 1 == 2);
  ASSERT_EQUALS(4, 2 * 2);
}

} // namespace ghidra
```

## Command-Line Decompiler

`decomp_dbg` / `decomp_opt` provide an interactive console for loading
binaries and decompiling functions. Usage:

    SLEIGHHOME=/path/to/ghidra ./decomp_dbg

Then at the `[decomp]>` prompt:

    lo file /path/to/binary
    lo fu 0x401000
    decompile
    print C

## Profiling

The Makefile has commented-out flags for gprof and gcov in the `DBG_CXXFLAGS`
section. Alternatively, use `perf`:

    perf record -g ./decomp_test_dbg datatests mytest.xml
    perf report --stdio --no-children --no-call-graph -n

## Cleaning

    make clean               # Remove object files
    make reallyclean          # Also remove build directories and generated sources
