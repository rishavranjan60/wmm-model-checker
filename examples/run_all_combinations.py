#!/usr/bin/env python3
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Run all examples with all memory models and check results')
parser.add_argument('executable', help="Path to executor binary file")
parser.add_argument('-a', '--execute-all', action='store_true',
                    help="Execute all combinations regardless of errors")
args = parser.parse_args()

memory_models: [str] = ['seq_cst', 'tso', 'pso', 'sra', 'ra']
# (example name, threads count)
examples: [tuple[str, int]] = [
    ('dekkers_lock', 2),
    ('message_passing', 2),
    ('ww_reorder', 2),
    ('par_inc', 2),
    ('sc_fence', 2),
    ('rel_sequence', 3)
]
results: [[int]] = [
    # seq_cst tso pso sra ra
    # dekkers_lock:
    [0, 1, 1, 1, 1],
    # message_passing:
    [0, 0, 1, 0, 0],
    # ww_reorder:
    [0, 0, 1, 0, 1],
    # par_inc:
    [0, 0, 0, 0, 0],
    # sc_fence:
    [0, 0, 0, 0, 0],
    # rel_sequence:
    [0, 0, 1, 0, 0]
]

for ex_num, (example, threads_count) in enumerate(examples):
    for model_num, mem_model in enumerate(memory_models):
        res: int = subprocess.call([args.executable, '-c',
                                    '--memory-model=' + mem_model,
                                    '--thread-count=' + str(threads_count),
                                    example + '.txt'],
                                   stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
        if res != results[ex_num][model_num]:
            print(f'Error:\n\tExample: {example}\n\tMemory model: {mem_model}' +
                  f'\n\tExpected: {results[ex_num][model_num]}\n\tFound: {res}')
            if not args.execute_all:
                exit(1)
