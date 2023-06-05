#!/usr/bin/python3

import os
from os import path
import subprocess
from subprocess import TimeoutExpired
import sys

dirs = [
    '../testcase/functional',
    '../testcase/performance',
]

exe = 'temp'
asm = 'temp.s'
mmcc = f'../build/trivial -o {asm} -i'
cc = f'./aarch32-gcc/bin/arm-none-linux-gnueabihf-gcc -x assembler -z noexecstack {asm} -O3 -Werror -o {exe} -static -Laarch32 -lsysy'


# print to stderr
def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)
    sys.stderr.flush()


# run single test case
def run_case(sy_file, in_file, out_file):
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
                            timeout=60)
    trimed = result.stdout.decode('utf-8').strip('\n')
    out = f'{trimed}\n{result.returncode}'
    out = out.strip()
    # compare to reference
    with open(out_file) as f:
        ref = f.read().strip()
    if out != ref:
        eprint("out:")
        eprint(out)
        eprint("ref:")
        eprint(ref)
    return out == ref


# run all test cases
def run_test(cases):
    total = 0
    passed = 0
    failed_list = []
    timeout_list = []
    error_list = []

    try:
        for sy_file, in_file, out_file in cases:
            # run test case
            eprint(f'running test "{sy_file}" ... ')
            try:
                if run_case(sy_file, in_file, out_file):
                    eprint(f'\033[0;32mPASS\033[0m')
                    passed += 1
                else:
                    eprint(f'\033[0;31mFAIL\033[0m')
                    failed_list.append(sy_file)
            except TimeoutExpired:
                eprint(f'\033[0;31mTIMEOUT\033[0m')
                timeout_list.append(sy_file)
            except Exception as e:
                eprint(f'\033[0;31mERROR\033[0m')
                eprint(e)
                error_list.append(sy_file)
            total += 1
    except KeyboardInterrupt:
        eprint(f'\033[0;33mINTERRUPT\033[0m')

    # remove temporary file
    if path.exists(exe):
        os.unlink(exe)
    if path.exists(asm):
        os.unlink(asm)
    # print result
    if passed == total:
        eprint(f'\033[0;32mPASS\033[0m ({passed}/{total})')
    else:
        eprint(f'\033[0;31mFAIL\033[0m ({passed}/{total})')
        if len(failed_list) > 0:
            eprint("failed:")
            for ele in failed_list:
                eprint(f'  {ele}')
        if len(timeout_list) > 0:
            eprint("timeout:")
            for ele in timeout_list:
                eprint(f'  {ele}')
        if len(error_list) > 0:
            eprint("error:")
            for ele in error_list:
                eprint(f'  {ele}')


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
    parser.add_argument('-i',
                        '--input',
                        default='',
                        help='specify input C source file, ' +
                        'default to empty, that means run ' +
                        'files in script configuration')
    # parse arguments
    args = parser.parse_args()
    # start running
    if args.input:
        # check if input test cast is valid
        if not args.input.endswith('.c'):
            eprint('input must be a C source file')
            exit(1)
        if not path.exists(args.input):
            eprint(f'file "{args.input}" does not exist')
            exit(1)
        # get absolute path & change cwd
        sy_file = path.abspath(args.input)
        os.chdir(path.dirname(path.realpath(__file__)))
        # get test case
        case = get_case(sy_file)
        if not path.exists(case[2]):
            eprint(f'output file "{case[2]}" does not exist')
            exit(1)
        # run test case
        run_test([case])
    else:
        # change cwd to script path
        os.chdir(path.dirname(path.realpath(__file__)))
        # run test cases in configuration
        run_test(scan_cases(dirs))
