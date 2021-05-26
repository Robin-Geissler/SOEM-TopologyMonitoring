# Master Library for Topology Monitoring with EterhCAT

This is a Master Library based on SOEM. It provides functions to build an EtherCAT Master which can perform Topology Monitoring. Also functions for Topology Visualization are provided.

BUILDING
========


Prerequisites for all platforms
-------------------------------

 * CMake 2.8.0 or later



Linux & macOS
--------------
   Build the Master:
   * `mkdir build`
   * `cd build`
   * `cmake ..`
   * `make`

   Install GraphViz for generating the Graph file:

   * `sudo apt-get install graphviz`

   Make sure that the "dot" program can be found under /usr/bin/dot (that should be the case normally). If the program is located somewhere else you can change the value of the execv() call in visualizeTopology() acordingly.

Run Master
==========

   * `cd build/src`
   * `sudo ./promodularSOEM networkInterface`

   For *networkInterface* use the name of your network interface (mostly eth0). You can use the `ifconfig` or `ip link show` command to read out the correct interface name.

Documentation
-------------

Get the original version of SOEM: https://openethercatsociety.github.io/doc/soem/

The src/main.c provides an example that was used to do a performance evaluation of the library.

