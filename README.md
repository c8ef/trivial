# TrivialCompiler

TrivialCompiler is a compiler written in C++17 that translates SysY (a C-like toy language) into ARM-v7a assembly.

## Architecture

![Architecture of TrivialCompiler](architecture.png)

Errata: the `.bc` should be `.ll` in the picture.

## Usage

```
./TrivialCompiler [-l ir_file] [-S] [-p] [-d] [-o output_file] [-O level] input_file
```

Options:

- `-p`: print the names of all passes to run and exit
- `-d`: enable debug mode (WARNING: will produce excessive amount of output)
- `-O`: set optimization level to `level` (no effect on behaviour currently)
- `-l`: dump LLVM IR (text format) to `ir_file` and exit (by running frontend only)
- `-o`: write assembly to `output_file`

You must specify either `-l` or `-o`, or nothing will actually happen.

You could refer to `CMakeLists.txt` on how to converting LLVM IR or assembly to executable file on `ARM-v7a` by using `llc` or `gcc` for assembling and linking.

## Testing

We use `ctest` to automatically test `TrivialCompiler` against several modern compilers. For running tests you need to install the following additional packages and their dependencies:

- `llvm` (to test IR output)
- `g++-arm-linux-gnueabihf`
- `qemu-user` (if not running on ARM-v7a architecture)

Several test cases and corresponding configurable CMake flags are provided:

- `FUNC_TEST` (default `ON`): function test cases provided by the contest committee
- `PERF_TEST` (default `ON`): performance test cases provided by the contest committee
- `CUSTOM_TEST` (default `ON`): test cases written by the authors

And there are more flags to configure whether to use modern compilers to compare with TrivialCompiler

- `GCC` (default `OFF`): use GCC to compile (`-Ofast`) to compare
- `CLANG` (default `OFF`): use Clang (`-Ofast`) to compare, needs `clang` to be installed

After configuring CMake, use `ctest` under your build directory to run all tests.

The results containing stdout and stderr can be located at `build/Testing/Temporary/LastTest.log`. You could use `utils/extract_result.py` to analyze the results and write it into a JSON file.

```cmake
foreach(case_file ${all_test_cases})

    get_filename_component(case_name "${case_file}" NAME_WE)
    string(REGEX REPLACE ".sy$" ".in" case_input ${case_file})
    string(REGEX REPLACE ".sy$" ".out" case_output ${case_file})

    # check our compiler
    # .sy -> .ll
    add_custom_command(OUTPUT "${case_name}.ll"
            COMMAND ${run_command_prefix} ./${project_name} -l "${case_name}.ll" "${case_file}"
            DEPENDS ./${project_name} "${case_file}")
    add_custom_target("llvm_ir_${case_name}" DEPENDS "${case_name}.ll")
    # .sy -> .S
    add_custom_command(OUTPUT "${case_name}.S"
            COMMAND ${run_command_prefix} ./${project_name} -o "${case_name}.S" "${case_file}"
            DEPENDS ./${project_name} "${case_file}")
    add_custom_target("asm_${case_name}" DEPENDS "${case_name}.S")

    if (RUN_GCC)
        # use GCC to generate exe
        # .sy -> .o
        add_custom_command(OUTPUT "${case_name}_gcc.o"
            COMMAND arm-linux-gnueabihf-g++ -x c++ -c -Ofast -g -marm -mcpu=cortex-a72 -mfpu=neon -mfloat-abi=hard -static -include "${CMAKE_CURRENT_SOURCE_DIR}/custom_test/sylib.h" "${case_file}" -o "${case_name}_gcc.o"
            DEPENDS "${case_file}" "${case_file}")
        # .o -> exe
        add_custom_target("${case_name}_gcc"
            COMMAND arm-linux-gnueabihf-gcc -u getint -g -marm -mfpu=neon -mfloat-abi=hard -static "${case_name}_gcc.o" "${CMAKE_CURRENT_SOURCE_DIR}/sysyruntimelibrary/libsysy.a" -o "${case_name}_gcc"
            DEPENDS "${case_name}_gcc.o")
        # run exe with qemu to test
        add_custom_target("test_${case_name}_gcc"
            COMMAND ${test_command} "./${case_name}_gcc" "${case_input}" "${case_name}_gcc.out" "${case_output}"
            DEPENDS "${case_name}_gcc")
        if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            add_test(NAME check_run_gcc_${case_name}
                COMMAND make "test_${case_name}_gcc")
        endif()
    endif()

    if (RUN_CLANG)
        # use Clang to generate exe
        # .sy -> .o
        if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "armv7l")
            add_custom_command(OUTPUT "${case_name}_clang.o"
                COMMAND clang++ -x c++ -c -Ofast -g -mcpu=cortex-a72 -mfpu=neon -mfloat-abi=hard -static -include "${CMAKE_CURRENT_SOURCE_DIR}/custom_test/sylib.h" "${case_file}" -o "${case_name}_clang.o"
                DEPENDS "${case_file}" "${case_file}")
        else ()
            add_custom_command(OUTPUT "${case_name}_clang.o"
                COMMAND clang++ -x c++ -c -Ofast -g --sysroot /usr/arm-linux-gnueabihf --target=armv7-unknown-linux-eabi -mcpu=cortex-a72 -mfpu=neon -mfloat-abi=hard -static -include "${CMAKE_CURRENT_SOURCE_DIR}/custom_test/sylib.h" "${case_file}" -o "${case_name}_clang.o"
                DEPENDS "${case_file}" "${case_file}")
        endif ()
        # .o -> exe
        add_custom_target("${case_name}_clang"
            COMMAND arm-linux-gnueabihf-gcc -u getint -g -marm -mfpu=neon -mfloat-abi=hard -static "${case_name}_clang.o" "${CMAKE_CURRENT_SOURCE_DIR}/sysyruntimelibrary/libsysy.a" -o "${case_name}_clang"
            DEPENDS "${case_name}_clang.o")
        # run exe with qemu to test
        add_custom_target("test_${case_name}_clang"
            COMMAND ${test_command} "./${case_name}_clang" "${case_input}" "${case_name}_clang.out" "${case_output}"
            DEPENDS "${case_name}_clang")
        if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            add_test(NAME check_run_clang_${case_name}
                COMMAND make "test_${case_name}_clang")
        endif()
    endif()

    # use LLVM to generate exe from IR
    # .ll -> .o
    add_custom_command(OUTPUT "${case_name}_llvm.o"
            COMMAND llc -O3 -march=arm -mcpu=cortex-a72 -float-abi=hard -filetype=obj "${case_name}.ll" -o "${case_name}_llvm.o"
            DEPENDS "${case_name}.ll")
    # .o -> exe
    add_custom_target("${case_name}_llvm"
            COMMAND arm-linux-gnueabihf-gcc -u getint -g -marm -mfpu=neon -mfloat-abi=hard -static "${case_name}_llvm.o" "${CMAKE_CURRENT_SOURCE_DIR}/sysyruntimelibrary/libsysy.a" -o "${case_name}_llvm"
            DEPENDS "${case_name}_llvm.o")
    # run exe with qemu to test
    add_custom_target("test_${case_name}_llvm"
            COMMAND ${test_command} "./${case_name}_llvm" "${case_input}" "${case_name}_llvm.out" "${case_output}"
            DEPENDS "${case_name}_llvm")
    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        add_test(NAME check_run_llvm_${case_name}
                COMMAND make "test_${case_name}_llvm")
    endif()

    # use our compiler to generate exe
    # .S -> .o
    add_custom_command(OUTPUT "${case_name}_tc.o"
            COMMAND arm-linux-gnueabihf-as -g -march=armv7-a -mfloat-abi=hard "${case_name}.S" -o "${case_name}_tc.o"
            DEPENDS "${case_name}.S")
    # .o -> exe
    add_custom_target("${case_name}_tc"
            COMMAND arm-linux-gnueabihf-gcc -g -marm -march=armv7-a -mfpu=neon -mfloat-abi=hard -static "${case_name}_tc.o" "${CMAKE_CURRENT_SOURCE_DIR}/sysyruntimelibrary/libsysy.a" -o "${case_name}_tc"
            DEPENDS "${case_name}_tc.o")
    # run exe with qemu to test
    add_custom_target("test_${case_name}_tc"
            COMMAND ${test_command} "./${case_name}_tc" "${case_input}" "${case_name}_tc.out" "${case_output}"
            DEPENDS "${case_name}_tc")
    if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        add_test(NAME check_run_tc_${case_name}
                COMMAND make "test_${case_name}_tc")
    endif()

endforeach()
```
