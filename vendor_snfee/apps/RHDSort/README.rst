===========================================
The ``snfee-rhdsort`` program
===========================================

:Author: F.Mauger <mauger@lpccaen.in2p3.fr>
:Date: 2019-05-13

The ``snfee-rhdsort``  program sorts the  *raw hit  data*
(``RHD``) from the SuperNEMO data acquisition program with respect
to the trigger ID of the raw hits.

.. _SNFrontEndElectronics: https://gitlab.in2p3.fr/SuperNEMO-DBD/SNFrontEndElectronics

.. contents::

-------

The program
===========

The ``snfee-rhdsort`` reads  a bunch of files  storing ``RHD`` records
associated to a same run. 

The input files must  use one of the  available formats in
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

   $ snfee-rtdsort --help
..

Example
-------

TBD
