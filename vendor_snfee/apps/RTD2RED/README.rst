===========================================
The ``snfee-rtd2red`` program
===========================================

:Author: F.Mauger <mauger@lpccaen.in2p3.fr>, J.Hommet <hommet@lpccaen.in2p3.fr>
:Date: 2019-10-16

The ``snfee-rtd2red``  program converts  the *raw trigger  data*
(``RTD``) from the SuperNEMO data acquisition program to the  SuperNEMO
data format  for  *raw event  data*
(``RED``) as defined in the SNFrontEndElectronics_ library.

.. _SNFrontEndElectronics: https://gitlab.in2p3.fr/SuperNEMO-DBD/SNFrontEndElectronics

.. contents::

-------

The program
===========

The ``snfee-rtd2red`` reads  a bunch of files  storing ``RTD`` records
associated to a same run. The  ``RTD`` records are packed according to
some criteria  into ``RED`` records.  The ``RED``are finaly
stored in output files.  The  I/O system use the Falaise serialization
system (based on Bayeux/Boost serialization libraries).

The input  and output files must  use one of the  available formats in
Bayeux, automatically selected from the file extension:

* ``.xml``, ``.xml.gz``,  ``.xml.bz2`` : plain or  compressed XML text
  format,
* ``.txt``, ``.txt.gz``, ``.txt.bz2`` : plain or compressed ASCII text
  format,
* ``.data``,  ``.data.gz``,   ``.data.bz2``  :  plain   or  compressed
  portable binary format.

Usage
-----

Online help can be printed with:

.. code:: bash

   $ snfee-rtd2red --help
..

Example
-------

Processing a data file from a calorimeter crate
-----------------------------------------------

TBD
