#! /bin/bash -f
TEST_HOME=${PWD}/tests
source_file=pmo.c
binary_file=$1
bin_dir=${TEST_HOME}/bin
stdin_dir=${TEST_HOME}/stdin
config_dir=${TEST_HOME}/config
stdout_dir=${TEST_HOME}/stdout

if [[ $2 == 'gen_std' ]]; then
  python3 ${bin_dir}/generic-tester.py --source_file ${source_file} \
    --binary_file ${binary_file} --test_dir ${stdin_dir} \
    --config_dir ${config_dir} --stdout_dir ${stdout_dir} \
    --start_test 1 --end_test 10 --build_flags="-pthread -Wall -g -O3" --gen_std
elif [[ $2 == 'build' ]]; then
  python3 ${bin_dir}/generic-tester.py --source_file ${source_file} \
    --binary_file ${binary_file} --test_dir ${stdin_dir} \
    --config_dir ${config_dir} --stdout_dir ${stdout_dir} \
    --start_test 1 --end_test 10 --build_flags="-pthread -Wall -g -O3" --build
else
  python3 ${bin_dir}/generic-tester.py --source_file ${source_file} \
    --binary_file ${binary_file} --test_dir ${stdin_dir} \
    --config_dir ${config_dir} --stdout_dir ${stdout_dir} \
    --start_test 1 --end_test 10 --build_flags="-pthread -Wall -g -O3"
fi

