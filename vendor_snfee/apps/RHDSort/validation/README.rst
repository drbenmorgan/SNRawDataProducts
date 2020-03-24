===================================================
Study of the ``snfee-rhdsort`` program on real data
===================================================



Check with commissioning run 104
================================

Run 121 has used a high threshold (50 mV).

1. Convert CRD files to unsorted RHD files with ``snfee-crd2rhd``
2. Run ``snfee-rhdsort`` in evaluation mode:

   .. code:: bash

      $ snfee-rhdsort \
	 --evaluation \
         --input-file /data/mauger/SuperNEMO/raw_data/uRHD/run_104/snemo_run-104_urhd_crate-0_part-0.xml \
	 --input-file /data/mauger/SuperNEMO/raw_data/uRHD/run_104/snemo_run-104_urhd_crate-0_part-1.xml \
	 --input-file /data/mauger/SuperNEMO/raw_data/uRHD/run_104/snemo_run-104_urhd_crate-0_part-2.xml \
	 --report-file run-104-evaluation.report
      ...
   ..

   **Results:**

      * Number of detected inversions: 148206
      * Maximum inversion distance detected for the 150000 first calorimeter hits: 1102
      * Histogram of maximum inversion distances:

	.. image:: results/snfee-rhdsort_run-104_evaluation.pdf
	   :width: 75%


3. Run ``snfee-rhdsort`` in sort mode:

   .. code:: bash

      $ snfee-rhdsort \
	 --sort \
	 --sort-buffer-size 4000 \
         --input-file /data/mauger/SuperNEMO/raw_data/uRHD/run_104/snemo_run-104_urhd_crate-0_part-0.xml \
	 --input-file /data/mauger/SuperNEMO/raw_data/uRHD/run_104/snemo_run-104_urhd_crate-0_part-1.xml \
	 --input-file /data/mauger/SuperNEMO/raw_data/uRHD/run_104/snemo_run-104_urhd_crate-0_part-2.xml \
	 --max-records-per-file 50000 \
	 --output-file /data/mauger/SuperNEMO/raw_data/RHD/run_104/snemo_run-104_rhd_crate-0_part-0.xml \
	 --output-file /data/mauger/SuperNEMO/raw_data/RHD/run_104/snemo_run-104_rhd_crate-0_part-1.xml \
	 --output-file /data/mauger/SuperNEMO/raw_data/RHD/run_104/snemo_run-104_rhd_crate-0_part-2.xml \
	 --report-file run-104-sort.report \
	 --log-modulo 1000
      ...
   ..


   
Check with commissioning run 121
================================

Run 121 has used a high threshold (300 mV).

1. Convert CRD files to unsorted RHD files with ``snfee-crd2rhd``

   .. code:: bash
      $ export CRD_DIR="/tmp/mauger/Mount/caelogin.in2p3.fr/data/nemo/scratch/mauger/snemo_data/raw_data/CRD/run_121"
      $ export URHD_DIR="/tmp/mauger/Mount/caelogin.in2p3.fr/data/nemo/scratch/mauger/snemo_data/raw_data/uRHD/run_121"
      $ export RHD_DIR="/tmp/mauger/Mount/caelogin.in2p3.fr/data/nemo/scratch/mauger/snemo_data/raw_data/RHD/run_121"
      $ snfee-crd2rhd \
	     --crate-number 0 \
             --input-file ${CRD_DIR}/run_121.dat \
             --input-file ${CRD_DIR}/run_121.dat_0001 \
             --input-file ${CRD_DIR}/run_121.dat_0002 \
             --input-file ${CRD_DIR}/run_121.dat_0003 \
	     --max-records-per-file 100000 \
	     --dynamic-output-files \
             --output-file ${URHD_DIR}/snemo_run-121_urhd_crate-0.xml
      ...
   ..

   
2. Run ``snfee-rhdsort`` in evaluation mode:

   .. code:: bash

      $ snfee-rhdsort \
	 --evaluation \
         --input-file ${URHD_DIR}/snemo_run-121_urhd_crate-0_part-0.xml \
	 --report-file run-121-evaluation.report
      ...
   ..

   **Results:**

      * Number of detected inversions: 1232
      * Maximum inversion distance detected for the 100000 first calorimeter hits: 71

   **Conclusion:** At low trigger rate (high threshold), we observe far less inversions of hits in the collected flow.
