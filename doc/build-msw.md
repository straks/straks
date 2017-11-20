WINDOWS BUILD NOTES
===================
For example following command adds user execute permission to an arbitrary file:

	git update-index --chmod=+x <file>


Compilers Supported
-------------------
TODO: What works?
Note: releases are cross-compiled using mingw running on Linux.


Dependencies
------------
Libraries you need to download separately and build:

	name            default path               download
	--------------------------------------------------------------------------------------------------------------------
	OpenSSL         \openssl-1.0.1c-mgw        http://www.openssl.org/source/
	Berkeley DB     \db-4.8.30.NC-mgw          http://www.oracle.com/technology/software/products/berkeley-db/index.html
	Boost           \boost-1.50.0-mgw          http://www.boost.org/users/download/
	miniupnpc       \miniupnpc-1.6-mgw         http://miniupnp.tuxfamily.org/files/

Their licenses:

	OpenSSL        Old BSD license with the problematic advertising requirement
	Berkeley DB    New BSD license with additional requirement that linked software must be free open source
	Boost          MIT-like license
	miniupnpc      New (3-clause) BSD license

Versions used in this release:

	OpenSSL      1.0.1c
	Berkeley DB  4.8.30.NC
	Boost        1.50.0
	miniupnpc    1.6


OpenSSL
-------
MSYS shell:

un-tar sources with MSYS 'tar xfz' to avoid issue with symlinks (OpenSSL ticket 2377)
change 'MAKE' env. variable from 'C:\MinGW32\bin\mingw32-make.exe' to '/c/MinGW32/bin/mingw32-make.exe'

	cd /c/openssl-1.0.1c-mgw
	./config
	make

Berkeley DB
-----------
MSYS shell:

	cd /c/db-4.8.30.NC-mgw/build_unix
	sh ../dist/configure --enable-mingw --enable-cxx
	make

Boost
-----
MSYS shell:

	downloaded boost jam 3.1.18
	cd \boost-1.50.0-mgw
	bjam toolset=gcc --build-type=complete stage

MiniUPnPc
---------
UPnP support is optional, make with `USE_UPNP=` to disable it.

MSYS shell:

	cd /c/miniupnpc-1.6-mgw
	make -f Makefile.mingw
	mkdir miniupnpc
	cp *.h miniupnpc/

Straks
-------
MSYS shell:

	cd \straks
	sh autogen.sh
	sh configure
	mingw32-make
	strip straks.exe
	
	
-------------------	compiling straks on windows
	
	./autogen.sh

	CPPFLAGS="-I/c/deps/db-4.8.30.NC/build_unix \
	-I/c/deps/openssl-1.0.2/include \
	-I/c/deps \
	-I/c/deps/protobuf-2.5.0/src \
	-I/c/deps/libpng-1.6.14 \
	-I/c/deps/qrencode-3.4.4" \
	LDFLAGS="-L/c/deps/db-4.8.30.NC/build_unix \
	-L/c/deps/openssl-1.0.2 \
	-L/c/deps/miniupnpc \
	-L/c/deps/protobuf-2.5.0/src/.libs \
	-L/c/deps/libpng-1.6.14/.libs \
	-L/c/deps/qrencode-3.4.4/.libs" \
	BOOST_ROOT=/c/deps/boost_1_55_0 \
	./configure \
	--with-qt-incdir=/c/Qt/5.3.2/include \
	--with-qt-libdir=/c/Qt/5.3.2/lib \
	--with-qt-plugindir=/c/Qt/5.3.2/plugins \
	--with-qt-bindir=/c/Qt/5.3.2/bin \
	--with-protoc-bindir=/c/deps/protobuf-2.5.0/src

	make

	strip src/straks-cli.exe
	strip src/straksd.exe
	strip src/qt/straks-qt.exe	
	
	
	
	
	
	
	
	
