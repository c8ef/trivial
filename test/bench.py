#!/usr/bin/python3

import os
from os import path
import subprocess
import sys
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path

bench_dirs = [
    '../testcase/performance/',
]

exe = 'temp'
asm = 'temp.s'
cc = f'arm-linux-gnueabihf-gcc -x assembler -z noexecstack {asm} -O3 -Werror -o {exe} -static -Laarch32 -lsysy'

mmcc = f'../build/compiler -o {asm}'
gcc_O1 = f'arm-linux-gnueabihf-g++ -include sysy.h -O1 -static -o {exe} sysy.c'
gcc_O2 = f'arm-linux-gnueabihf-g++ -include sysy.h -O2 -static -o {exe} sysy.c'
gcc_O3 = f'arm-linux-gnueabihf-g++ -include sysy.h -O3 -static -o {exe} sysy.c'
clang_O1 = f'clang++ -target arm-linux-gnueabihf -Wno-deprecated -include sysy.h -O1 -static -o {exe} sysy.c'
clang_O2 = f'clang++ -target arm-linux-gnueabihf -Wno-deprecated -include sysy.h -O2 -static -o {exe} sysy.c'
clang_O3 = f'clang++ -target arm-linux-gnueabihf -Wno-deprecated -include sysy.h -O3 -static -o {exe} sysy.c'

bench_data = []
indexes = []
columns = [
    'gcc-O1',
    'gcc-O2',
    'gcc-O3',
    'flame',
    'clang-O3',
    'clang-O2',
    'clang-O1',
]


# print to stderr
def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)
    sys.stderr.flush()


def bench_sys_compiler(compiler, sy_file, in_file):
    mmcc_cmd = compiler.split(' ') + [sy_file]

    result = subprocess.run(mmcc_cmd, timeout=60)
    if result.returncode:
        return False

    if in_file:
        with open(in_file) as f:
            inputs = f.read().encode('utf-8')
    else:
        inputs = None
    result = subprocess.run(f'./{exe}',
                            input=inputs,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            timeout=60)
    return int(result.stderr.decode('utf-8').strip('\n'))


# run single test case
def bench_case(sy_file, in_file, out_file):
    single_data = []

    # compile to executable
    mmcc_cmd = mmcc.split(' ') + [sy_file]

    result = subprocess.run(mmcc_cmd, timeout=60)
    if result.returncode:
        return False

    result = subprocess.run(cc.split(' '))
    if result.returncode:
        return False
    # run compiled file
    if in_file:
        with open(in_file) as f:
            inputs = f.read().encode('utf-8')
    else:
        inputs = None
    result = subprocess.run(f'./{exe}',
                            input=inputs,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE,
                            timeout=60)
    trimed = result.stdout.decode('utf-8').strip('\n')
    elaps = result.stderr.decode('utf-8').strip('\n')

    out = f'{trimed}\n{result.returncode}'
    out = out.strip()
    # compare to reference
    with open(out_file) as f:
        ref = f.read().strip()

    if out == ref:
        single_data.append(bench_sys_compiler(gcc_O1, sy_file, in_file))
        single_data.append(bench_sys_compiler(gcc_O2, sy_file, in_file))
        single_data.append(bench_sys_compiler(gcc_O3, sy_file, in_file))
        single_data.append(int(elaps))
        single_data.append(bench_sys_compiler(clang_O3, sy_file, in_file))
        single_data.append(bench_sys_compiler(clang_O2, sy_file, in_file))
        single_data.append(bench_sys_compiler(clang_O1, sy_file, in_file))

        bench_data.append(single_data)
        indexes.append(Path(sy_file).name)

    return out == ref


# run all test cases
def run_bench(cases):
    total = 0
    passed = 0

    try:
        for sy_file, in_file, out_file in cases:
            # run test case
            eprint(f'running test "{sy_file}" ... ')
            try:
                if bench_case(sy_file, in_file, out_file):
                    eprint(f'\033[0;32mPASS\033[0m')
                    passed += 1
                else:
                    eprint(f'\033[0;31mFAIL\033[0m')
            except subprocess.TimeoutExpired:
                eprint(f'\033[0;31mTIMEOUT\033[0m')
            except Exception as e:
                eprint(f'\033[0;31mERROR\033[0m')
                eprint(e)
            total += 1
    except KeyboardInterrupt:
        eprint(f'\033[0;33mINTERRUPT\033[0m')

    # remove temporary file
    if path.exists(exe):
        os.unlink(exe)
    if path.exists(asm):
        os.unlink(asm)
    # print result
    eprint(bench_data)
    if passed == total:
        eprint(f'\033[0;32mPASS\033[0m ({passed}/{total})')
    else:
        eprint(f'\033[0;31mFAIL\033[0m ({passed}/{total})')


# get single test case by '*.c' file
def get_case(sy_file):
    in_file = f'{sy_file[:-2]}.in'
    out_file = f'{sy_file[:-2]}.out'
    if not path.exists(in_file):
        in_file = None
    return sy_file, in_file, out_file


# scan & collect test cases
def scan_cases(dirs):
    cases = []
    # scan directories
    for i in dirs:
        for root, _, files in os.walk(i):
            for f in sorted(files):
                # find all '*.c' files
                if f.endswith('.c'):
                    sy_file = path.join(root, f)
                    # add to list of cases
                    cases.append(get_case(sy_file))
    return cases


if __name__ == '__main__':
    import argparse
    # initialize argument parser
    parser = argparse.ArgumentParser()
    parser.formatter_class = argparse.RawTextHelpFormatter
    parser.description = 'An auto-test tool for MimiC project.'
    # parse arguments
    args = parser.parse_args()
    # start running
    # change cwd to script path
    os.chdir(path.dirname(path.realpath(__file__)))
    # run test cases in configuration
    run_bench(scan_cases(bench_dirs))
    df = pd.DataFrame(bench_data, index=indexes, columns=columns)
    df.plot(kind='bar')
    plt.tight_layout()
    plt.savefig('bench.png')
