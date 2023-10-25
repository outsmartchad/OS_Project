#! /bin/bash -f
TEST_HOME=${PWD}/tests
bin_dir=${TEST_HOME}/bin
stdin_dir=${TEST_HOME}/stdin
config_dir=${TEST_HOME}/config
stdout_dir=${TEST_HOME}/stdout

for file in ${config_dir}/*.json
do
    python3 ${bin_dir}/generator.py ${file} ${stdin_dir} ${stdout_dir}
done
