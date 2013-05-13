					PLFS UPC-IO (PUPC-IO): A parallel I/O library for UPC
            						   Version 1.2, May 1st, 2013
					               Hugh Greenberg <hng@lanl.gov>

		     Based on the reference UPC-IO implementation from GWU: http://threads.hpcl.gwu.edu/sites/libupcio/

Changes
---------------
* Initial Release
* Uses PLFS instead of POSIX : http://institute.lanl.gov/plfs/


How To Use
-----------------
* Install upc-io PLFS branch: https://github.com/hugegreenbug/plfs-core
* Modify the Makefiles to point to the correct berkeley upcc compiler and translator: http://upc.lbl.gov/
* Modify the Makefiles to set NP to the number of threads you want to use
* Type: make  to build the library and all of tests
* Use the library in your UPC code.  Look at the tests for examples.

