#!/usr/bin/env bash

this_dir="$(pwd)"
echo >&2 "[info] Current directory : '${this_dir}'"

build_dir="$(pwd)/../../../../../_build.d"
if [ ! -d ${build_dir} ]; then
    echo >&2 "[error] No build directory!"
    exit 1
fi
cd ${build_dir}
build_dir=$(pwd)
echo >&2 "[info] Build directory : '${build_dir}'"
cd ${this_dir}

cbd_writer_bin="${build_dir}/sncomdaq_tests/sncomdaq-test_sncomdaq_run_writer"
if [ ! -x ${cbd_writer_bin} ]; then
    echo >&2 "[error] No '${cbd_writer_bin}' executable!"
    exit 1
fi

cbd2rhd_bin="${build_dir}/snfee-cbd2rhd"
if [ ! -x ${cbd2rhd_bin} ]; then
    echo >&2 "[error] No '${cbd2rhd_bin}' executable!"
    exit 1
fi

${cbd_writer_bin}

cbd_files=$(find . -name "snemo_run-100_part-?.bin" | sort -n)

for cbd_file in ${cbd_files} ; do
    echo >&2 "[info] CBD mock file : '${cbd_file}'"   
    ${cbd2rhd_bin} \
	--print-records \
	--input-file "snemo_run-100_part-0.bin" \
	--input-file "snemo_run-100_part-1.bin" \
	--input-file "snemo_run-100_part-2.bin" \
	--input-file "snemo_run-100_part-3.bin" \
	--input-file "snemo_run-100_part-4.bin" \
	--input-file "snemo_run-100_part-5.bin" \
	--crate-number 0 \
	--output-file "snemo_run-100_calo-0_rhd.xml"
    if [ $? -ne 0 ]; then
	echo >&2 "[error] cbd2rhd failed!"
	exit 1
    fi
done

exit 0

# end
