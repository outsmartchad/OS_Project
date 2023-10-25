import argparse
import os
import signal
import subprocess
import time
import json

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def read_file(f):
    with open(f, 'r') as content_file:
        content = content_file.read()
    return content

def write_file(f, data):
    with open(f, 'w') as content_file:
        content_file.write(data)

def print_failed(msg, expected, actual, test_passed):
    if test_passed: # up til now, no one said that the test failed
        print('RESULT failed')
    print(msg)
    if hasattr(expected, '__len__') and len(expected) > 1000:
        print('Showing first 500 bytes; use -vv to show full output')
        expected = expected[:500]
        actual = actual[:500]
    print('expected: [{}]'.format(expected))
    print('got:      [{}]\n'.format(actual))

# some code from:
# stackoverflow.com/questions/18404863/i-want-to-get-both-stdout-and-stderr-from-subprocess
def run_command(cmd, timeout_length):
    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)

    try:
        if timeout_length == -1:
            std_output, std_error = proc.communicate()
        else:
            std_output, std_error = proc.communicate(timeout=timeout_length)
    except subprocess.TimeoutExpired:
        print('WARNING test timed out (i.e., it took too long)')
        os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        std_output, std_error = proc.communicate()
    return std_output.decode(), std_error.decode(), int(proc.returncode)

def handle_outcomes(stdout_expected, stdout_actual, stderr_expected, stderr_actual, rc_expected, rc_actual, total_time):
    test_passed = True
    print('Test finished in {:.3f} seconds'.format(total_time))
    if rc_actual != rc_expected:
        print_failed('return code does not match expected', rc_expected, rc_actual, test_passed)
        test_passed = False
    stdout_actual = ' '.join(stdout_actual.replace('\t', ' ').replace('\n', ' ').strip().split())
    stdout_expected = ' '.join(stdout_expected.replace('\t', ' ').replace('\n', ' ').strip().split())
    if stdout_expected != stdout_actual:
        print_failed('standard output does not match expected', stdout_expected, stdout_actual, test_passed)
        test_passed = False
    if test_passed:
        print(bcolors.OKGREEN + 'RESULT passed' + bcolors.ENDC)
        return True
    return False

def save_output(test_number, stdout_dir, stdout_actual, stderr_actual, rc_actual, total_time):
    write_file('{}/{}.out'.format(stdout_dir, test_number), str(stdout_actual))
    write_file('{}/{}.rc'.format(stdout_dir, test_number), str(rc_actual))
    print('Total time {:.3f} sec'.format(total_time))
    return True

def get_test_description(base_desc, matrixsize, timeout):
    base_desc = base_desc.replace('${timeout}', str(timeout)).replace("${matrixsize}", str(matrixsize))
    return base_desc

def test_one(test_number, test_path, config_dir, stdout_dir, binary_file, save):
    config = json.load(open(os.path.join(config_dir, '{}.json'.format(test_number)), 'r'))
    test_desc = get_test_description(config['description'], config['params']['max_size'], config['timeout'])
    
    if not save:
        stdout_expected = read_file('{}/{}.out'.format(stdout_dir, test_number))
        stderr_expected = ''
        rc_expected = int(read_file('{}/{}.rc'.format(stdout_dir, test_number)))

    print('TEST {} - {}'.format(test_number, test_desc))
    if save:
        timeout_length = -1
    else:
        timeout_length = config['timeout']
    start_time = time.time()
    
    run_desc = './' + binary_file + ' < ' + test_path
    stdout_actual, stderr_actual, rc_actual = run_command(run_desc, timeout_length)
    total_time = time.time() - start_time

    if save:
        if rc_actual != 0:
            print(stderr_actual)
        return save_output(test_number, stdout_dir, stdout_actual, stderr_actual, rc_actual, total_time)
    else:
        return handle_outcomes(stdout_expected, stdout_actual, stderr_expected, stderr_actual, rc_expected, rc_actual, total_time)

parser = argparse.ArgumentParser()
parser.add_argument('--source_file', help='name of source file to test', type=str, required=True)
parser.add_argument('--binary_file', help='name of binary to produce', type=str, required=True)
parser.add_argument('--test_dir', help='path to location of tests', type=str, required=True)
parser.add_argument('--config_dir', help='path to location of configs', type=str, required=True)
parser.add_argument('--stdout_dir', help='path to location of standard outputs', type=str, required=True)
parser.add_argument('--start_test', help='start with this test number', type=int, default=1)
parser.add_argument('--end_test', help='end with this test number; -1 means go until done', type=int, default=10)
parser.add_argument('--build_flags', help='extra build flags for gcc', type=str, default='')
parser.add_argument('--build', action='store_true')
parser.add_argument('--gen_std', action='store_true')
args = parser.parse_args()

input_file = args.source_file
binary_file = args.binary_file
test_dir = args.test_dir
config_dir = args.config_dir
stdout_dir = args.stdout_dir
if not args.gen_std and args.build:
    print('TEST 0 - clean build (program should compile without errors or warnings)')
    start_time = time.time()
    stdout_actual, stderr_actual, rc_actual = run_command('gcc {} -o {} {}'.format(input_file, binary_file, args.build_flags), -1)
    total_time = time.time() - start_time
    if handle_outcomes('', stdout_actual, '', stderr_actual, 0, rc_actual, total_time) == False:
        exit(1)
    print('')

test_number = args.start_test
while True:
    test_path = os.path.join(test_dir, str(test_number) + '/input.txt')
    test_result = test_one(test_number, test_path, config_dir, stdout_dir, binary_file, args.gen_std)
    print('')
    
    if not test_result:
        exit(1)

    test_number += 1
    if args.end_test != -1 and test_number > args.end_test:
        break
