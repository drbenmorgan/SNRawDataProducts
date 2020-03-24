===========================================
The ``snfee-cbd2rhd`` program
===========================================

:Author: F.Mauger <mauger@lpccaen.in2p3.fr>
:Date: 2020-02-06

The  ``snfee-cbd2rhd``  program  converts  the  *commissioning raw
native binary data*  (``CBD``) from the test  data acquisition program
provided by  the LAL  team (*SN  CRATE SOFTWARE*,  version >=  3.2, by
Jihane Maalmi,  LAL Orsay) to  the official SuperNEMO data  format for
*raw  hit data*  (``RHD``)  as defined  in the  SNFrontEndElectronics_
library.

.. _SNFrontEndElectronics: https://gitlab.in2p3.fr/SuperNEMO-DBD/SNFrontEndElectronics

.. contents::

-------
   
Description of the data model
=============================

The ``CBD`` data
----------------

The ``CBD`` data  is based on native binary  formatted files generated
by the SN CRATE SOFTWARE (version  > 3.2) through the SNComDAQ library
(version >=  0.1.2).  These files  have been designed for  testing and
diagnose the behaviour of  the front-end electronics (firmware).  They
are  not  intended  for   production  operations,  only  commissioning
tests. However, it is possible to use these files in such a way we can
extract  from  them collected  raw  hits  informations and  perform  a
conversion into the offical SuperNEMO ``RHD`` data model.


The program
===========

The ``snfee-cbd2rhd``  converts a  single input datafile  using the
``CBD`` data format  to an output file using  the serialization system
implemented  in the  ``SNFEE`` library  which is  compatible with  the
Falaise  serialization  system  (based on  Bayeux/Boost  serialization
libraries).

The  output file  must use  one of  the available  formats in  Bayeux,
automatically selected from the file extension:

* ``.xml``,  ``.xml.gz``,  ``.xml.bz2``  :  plain  or  compressed  XML
  portable text  format (for  test and debugging  purpose, to  be used
  with small amount of data only),
* ``.txt``,  ``.txt.gz``, ``.txt.bz2``  :  plain  or compressed  ASCII
  portable text format,
* ``.data``,  ``.data.gz``,   ``.data.bz2``  :  plain   or  compressed
  portable binary format (for production).


Usage
-----

Online help can be printed with:

.. code:: bash

   $ snfee-cbd2rhd --help
..

Example
-------

Processing a data file from a calorimeter crate
-----------------------------------------------

The  following  command illustrates  how  to  convert a  commissioning
tracker raw data file to the official format, using an arbitrary crate
number (from 0 to 2):

.. code:: bash

   $ snfee-cbd2rhd \
      --logging "warning" \
      --crate-number 0 \
      --input-file "/data/SuperNEMO/Com2020/Run_1000/Calo/snemo_run-1000.bin" \
      --output-file "snemo_run-1000_rhd_calo-0.xml"
..


Processing a data file from a tracker crate
-------------------------------------------

The  following  command illustrates  how  to  convert a  commissioning
tracker  raw data  file to  the  official format,  using an  arbitrary
tracker crate number (from 0 to 2):

.. code:: bash

   $ snfee-cbd2rhd \
      --logging "warning" \
      --crate-number 2 \
      --input-file "/data/SuperNEMO/Com2020/Run_1000/tracker/snemo_run-1000.bin" \
      --output-file "snemo_run-1000_rhd_calo-0.xml"
..


.. end
   
