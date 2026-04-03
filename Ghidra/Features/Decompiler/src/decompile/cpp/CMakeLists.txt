cmake_minimum_required(VERSION 3.10)
project(ghidra_decompiler CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core source files (no platform-specific dependencies)
set(CORE_SOURCES
    xml.cc marshal.cc space.cc float.cc address.cc pcoderaw.cc translate.cc
    opcodes.cc globalcontext.cc)

set(DECCORE_SOURCES
    capability.cc architecture.cc options.cc graph.cc cover.cc block.cc
    cast.cc typeop.cc database.cc cpool.cc comment.cc stringmanage.cc
    modelrules.cc fspec.cc action.cc loadimage.cc grammar.cc varnode.cc
    op.cc type.cc variable.cc varmap.cc jumptable.cc emulate.cc
    emulateutil.cc flow.cc userop.cc expression.cc multiprecision.cc
    funcdata.cc funcdata_block.cc funcdata_op.cc funcdata_varnode.cc
    unionresolve.cc pcodeinject.cc heritage.cc prefersplit.cc rangeutil.cc
    ruleaction.cc subflow.cc blockaction.cc merge.cc double.cc transform.cc
    constseq.cc bitfield.cc coreaction.cc condexe.cc override.cc dynamic.cc
    crc32.cc prettyprint.cc printlanguage.cc printc.cc printjava.cc
    memstate.cc opbehavior.cc paramid.cc signature.cc)

set(SLEIGH_SOURCES
    sleigh.cc pcodeparse.cc pcodecompile.cc sleighbase.cc slghsymbol.cc
    slghpatexpress.cc slghpattern.cc semantics.cc context.cc slaformat.cc
    compression.cc filemanage.cc)

set(EXTRA_SOURCES
    callgraph.cc ifacedecomp.cc ifaceterm.cc inject_sleigh.cc interface.cc
    libdecomp.cc loadimage_xml.cc raw_arch.cc rulecompile.cc sleigh_arch.cc
    testfunction.cc unify.cc xml_arch.cc)

# BFD-dependent sources (Unix only)
set(BFD_SOURCES
    loadimage_bfd.cc bfd_arch.cc analyzesigs.cc codedata.cc)

# Test sources
set(TEST_MAIN test.cc)
file(GLOB UNITTEST_SOURCES ../unittests/*.cc)

# Determine platform-specific sources and libraries
if(WIN32)
    # On Windows, skip BFD-dependent files and link zlib only
    set(PLATFORM_SOURCES "")
    set(PLATFORM_LIBS "")
    find_package(ZLIB REQUIRED)
    list(APPEND PLATFORM_LIBS ZLIB::ZLIB)
    add_definitions(-DWINDOWS -D_WINDOWS -DWIN32 -D_WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_definitions(-DWIN64 -D_WIN64)
    endif()
else()
    set(PLATFORM_SOURCES ${BFD_SOURCES})
    set(PLATFORM_LIBS bfd z)
endif()

# All decompiler library sources
set(ALL_DECOMP_SOURCES
    ${CORE_SOURCES} ${DECCORE_SOURCES} ${SLEIGH_SOURCES}
    ${EXTRA_SOURCES} ${PLATFORM_SOURCES})

# Test executable
add_executable(decomp_test
    ${ALL_DECOMP_SOURCES} ${TEST_MAIN} ${UNITTEST_SOURCES})
target_compile_definitions(decomp_test PRIVATE __TERMINAL__)
target_include_directories(decomp_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(decomp_test PRIVATE ${PLATFORM_LIBS})

# Command-line decompiler
add_executable(decomp
    ${ALL_DECOMP_SOURCES} consolemain.cc)
target_compile_definitions(decomp PRIVATE __TERMINAL__)
target_include_directories(decomp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(decomp PRIVATE ${PLATFORM_LIBS})

# CTest integration.
# SLEIGHHOME must point to the Ghidra source root so tests can find processor specs.
# Set it via: cmake -DSLEIGHHOME=/path/to/ghidra, or the environment variable.
if(NOT SLEIGHHOME)
    # Default: assume we're building inside the Ghidra source tree
    get_filename_component(SLEIGHHOME "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../.." ABSOLUTE)
endif()
set(DATATEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../datatests")

enable_testing()
add_test(NAME unittests COMMAND decomp_test -usesleighenv unittests)
add_test(NAME datatests COMMAND decomp_test -usesleighenv -path ${DATATEST_DIR} datatests)
set_tests_properties(unittests datatests PROPERTIES ENVIRONMENT "SLEIGHHOME=${SLEIGHHOME}")
