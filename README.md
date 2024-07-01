# DLMS-COSEM

The goal of this project is to create an open reference implementation of a DLMS-COSEM compliant communications stack. 

To build:

    mkdir build
    cd build
    cmake ..
    make

To run:

Open a console.

	socat -v -x PTY,link=/tmp/ttyS10 PTY,link=/tmp/ttyS11

Open another console for the Server.

	cd [DLMS-COSEM Directory]/build/src/Linux
	./Linux -S

Open another console for the Client.

	cd [DLMS-COSEM Directory]/build/src/Linux
	./Linux

The Client and Server applications are menu-driven.  Start the server listening first, then TCP Connect, COSEM Open, COSEM Get, and COSEM Release.

The Server supports the following objects:

	0-0:96.1.{0-9}*255 [IData Class: 1]
		- Fully implemented for GET and SET.

	0-0:1.0.0*255 [IClock Class: 8]
		- Available with hooks for GET, SET, and ACTION.

	0-0:40.0.{0-1}*255 [IAssociationLN Class: 15]
		- Available for GET

These classes are not fully implemented, but they give the reader an indication of how to complete.  

To run unit tests.

	cd [DLMS-COSEM Directory]/build/test/DLMS-COSEM-TEST
	./DLMS-COSEM-TEST

To create doxygen documentation:

	cd [DLMS-COSEM Directory]
	doxygen

	The output will be located in [DLMS-COSEM Directory]/docs. 

For additional detailed information regarding the project, please refer to the Developer Guide.

![Developer and Maintainer Guide](DeveloperGuide.pdf)
	

