# Simple Open EtherCAT Master Library
[![Build Status](https://travis-ci.org/OpenEtherCATsociety/SOEM.svg?branch=master)](https://travis-ci.org/OpenEtherCATsociety/SOEM)
[![Build status](https://ci.appveyor.com/api/projects/status/bqgirjsxog9k1odf?svg=true)](https://ci.appveyor.com/project/hefloryd/soem-5kq8b)

BUILDING
========


Prerequisites for all platforms
-------------------------------

 * CMake 2.8.0 or later


Windows (Visual Studio)
-----------------------

 * Start a Visual Studio command prompt then:
   * `mkdir build`
   * `cd build`
   * `cmake .. -G "NMake Makefiles"`
   * `nmake`

Linux & macOS
--------------
   Build the Master:
   * `mkdir build`
   * `cd build`
   * `cmake ..`
   * `make`

   Install Dot for generating the Graph file:

   * `sudo apt-get install dot`


ERIKA Enterprise RTOS
---------------------

 * Refer to http://www.erika-enterprise.com/wiki/index.php?title=EtherCAT_Master


Run Master
==========

   * `cd build/src`
   * `sudo ./promodularSOEM eth0`

Documentation
-------------

See https://openethercatsociety.github.io/doc/soem/


Want to contribute to SOEM or SOES?
-----------------------------------

If you want to contribute to SOEM or SOES you will need to sign a Contributor
License Agreement and send it to us either by e-mail or by physical mail. More
information is available in the [PDF](http://openethercatsociety.github.io/cla/cla_soem_soes.pdf).
