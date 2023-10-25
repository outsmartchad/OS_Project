#!/usr/bin/env python
import os
import sys
import json
import random

seed = 10000079
buffer_size = 1000000

def generate_matrix(expr, mat_shape, path):
    with open(path, 'w') as f:
        buffer = ''
        buffer += str(expr + '\n')
        for shape in mat_shape:
            n, m = shape
            buffer += str(n) + '\t' + str(m) + '\n'
            for i in range(n):
                for j in range(m):
                    val = random.randint(-10, 10)
                    buffer += str(val) + '\t'
                    if len(buffer) > buffer_size:
                        f.write(buffer)
                        buffer = ''
                buffer += '\n'
        if buffer != '':
            f.write(buffer)

def generate_stdin(parmas, data_dir):
    expr = parmas['expr']
    max_size = params['max_size']
    matrix = [expr[i] for i in range(0, len(expr), 2)]
    ops = [expr[i] for i in range(1, len(expr), 2)] + [None]
    assert len(matrix) == len(ops)
    n = random.randint(max(max_size - 200, max_size // 2), max_size)
    m = random.randint(max(max_size - 200, max_size // 2), max_size)
    mat_shape0 = []
    mat_shape1 = []
    for op in ops:
        if op == '+' or op == '-' or op == None:
            mat_shape1.append(m)
        elif op == '*':
            p = random.randint(max(max_size - 200, max_size // 2), max_size)
            mat_shape1.append(p)
        else:
            assert False, f'Unrecognized operator {op}'
    ops = [None] + ops[:-1]
    for i, op in enumerate(ops):
        if op == '+' or op == '-' or op == None:
            mat_shape0.append(n)
        elif op == '*':
            mat_shape0.append(mat_shape1[i - 1])
        else:
            assert False, f'Unrecognized operator {op}'
    ops = ops[1:]
    matrix_path = os.path.join(data_dir, 'input.txt')
    mat_shape = [shape for shape in zip(mat_shape0, mat_shape1)]
    generate_matrix(expr, mat_shape, matrix_path)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        sys.exit('usage: generator.py <config_location> <stdin_dir> <stdout_dir>\n example: python generator.py 1.config stdin stdout')

    config_path = sys.argv[1]
    stdin_dir = sys.argv[2]
    stdout_dir = sys.argv[3]
    if not os.path.exists(stdout_dir):
        os.makedirs(stdout_dir)
    with open(config_path, 'r') as f:
        config = json.load(f)
        if 'seed' in config:
            seed = config['seed']
        random.seed(seed)
        params = config['params']
        case = config_path.split('/')[-1][:-5]
        stdin_path = os.path.join(stdin_dir, case)
        if not os.path.exists(stdin_path):
            os.makedirs(stdin_path)
        print('Generate data based on ' + config_path)
        generate_stdin(params, stdin_path)

