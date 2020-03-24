#!/usr/bin/env bash

# devel/test script
exit 0

SNFEE_PATH=/home/mauger/Documents/Private/Software/GitLab.IN2P3/SNFrontEndElectronics/snfee.alt/_build.d
     
URHD_PATH="/tmp/mauger/Mount/caelogin.in2p3.fr/data/nemo/scratch/mauger/snemo_data/raw_data/uRHD/run_104"
RHD_PATH=.
RHD_PATH="/tmp/mauger/Mount/caelogin.in2p3.fr/data/nemo/scratch/mauger/snemo_data/raw_data/RHD/run_104"
CRD_PATH="/tmp/mauger/Mount/caelogin.in2p3.fr/data/nemo/scratch/mauger/snemo_data/raw_data/CRD/run_104"

CRD_PATH="/data/mauger/SuperNEMO/raw_data/CRD/run_104"
URHD_PATH="/data/mauger/SuperNEMO/raw_data/uRHD/run_104"
RHD_PATH="/data/mauger/SuperNEMO/raw_data/RHD/run_104"

reset; ./snfee-crd2rhd \
     --crate-number 0 \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat" \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat_0001" \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat_0002" \
     --max-records-per-file 50000 \
     --dynamic-output-files \
     --output-file "${URHD_PATH}/snemo_run-104_urhd_crate-0.xml" 


reset; ./snfee-rhdsort \
	   --evaluation \
	   --input-file "${URHD_PATH}/snemo_run-104_urhd_crate-0_part-0.xml"



reset; ./snfee-rhdsort \
	   --input-file "${URHD_PATH}/snemo_run-104_urhd_crate-0_part-0.xml" \
	   --input-file "${URHD_PATH}/snemo_run-104_urhd_crate-0_part-1.xml" \
	   --input-file "${URHD_PATH}/snemo_run-104_urhd_crate-0_part-2.xml" \
	   --max-records-per-file 50000 \
	   --output-file "${RHD_PATH}/snemo_run-104_rhd_crate-0_part-0.xml" \
	   --output-file "${RHD_PATH}/snemo_run-104_rhd_crate-0_part-1.xml" \
	   --output-file "${RHD_PATH}/snemo_run-104_rhd_crate-0_part-2.xml" \
	   --sort-buffer-size 1000 \
	   --log-modulo 1000 \
	   --logging debug



     
reset; ./snfee-crd2rhd \
     --crate-number 0 \
     --max-records-per-file 50000 \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat" \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat_0001" \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat_0002" \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat_0003" \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat_0004" \
     --input-file "${CRD_PATH}/RunData_12052018_12h58min53s_test2_Ascii.dat_0005" \
     --dynamic-output-files \
     --output-file "${URHD_PATH}/snemo_run-104_urhd_crate-0.xml" 




reset; ./snfee-rhdsort \
	   --no-store    \
	   --input-file  "${URHD_PATH}/snemo_run-104_urhd_crate-0_part-0.xml" \
	   --sort-buffer-size 10 \
	   --log-modulo 10 \
	   --logging debug

snfee-rhdsort \
  --input-file  "${RHD_PATH}/snemo_run-104_rhd_crate-0_part-0.data.gz" \
  --output-file "${RHD_OUT_PATH}/snemo_run-104_rhd-sorted_crate-0_part-0.data.gz" \
  --sort-buffer-size 5000 \
  --max-records-per-file 1000000 \
  --log-modulo 10000

snfee-rhdsort \
  --input-file  "${RHD_PATH}/snemo_run-104_rhd_crate-0_part-0.data.gz" \
  --output-file "${RHD_OUT_PATH}/snemo_run-104_rhd-sorted_crate-0_part-0.data.gz" \
  --sort-buffer-size 5000 \
  --max-records-per-file 1000000 \
  --log-modulo 10000

snfee-rhdsort \
  --input-file ${RHD_IN_PATH}/snemo_run-104_rhd_crate-0_part-0.data.gz \
  --input-file ${RHD_IN_PATH}/snemo_run-104_rhd_crate-0_part-1.data.gz \
  --input-file ${RHD_IN_PATH}/snemo_run-104_rhd_crate-0_part-2.data.gz \
  --input-file ${RHD_IN_PATH}/snemo_run-104_rhd_crate-0_part-3.data.gz \
  --sort-buffer-size 5000 \
  --output-file "snemo_run-104_rhd-sorted_crate-0_part-0.data.gz" \
  --output-file "snemo_run-104_rhd-sorted_crate-0_part-1.data.gz" \
  --output-file "snemo_run-104_rhd-sorted_crate-0_part-2.data.gz" \
  --output-file "snemo_run-104_rhd-sorted_crate-0_part-3.data.gz" \
  --max-records-per-file 1000000 \
  --log-modulo 10000

exit 0

# end
