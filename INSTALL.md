
# Building and Installation

## Dependencies

Ensure the development version of the dependencies are installed. These are as follows:

* [libhdbpp](https://github.com/tango-controls-hdbpp/libhdbpp) - (libhdb++6). Build and install according its instructions.
* Tango Controls 9 or higher.
* omniORB release 4 - libomniorb4 and libomnithread.
* libzmq - libzmq3-dev or libzmq5-dev.

If they have not been installed in a standard location, then use standard CMake flags below to inform the build where to search for them.

The library can be built with Tango Controls install via debian package or source distribution. Supported Tango Controls release is currently 9.2.5a. Ensure build flags are used if tango is installed to a custom location.

Toolchain dependencies:

* CMake 3.0.0 or greater is required to perform the build.

Other dependencies:

* REST client for C++, https://github.com/mrtazz/restclient-cpp


## Standard flags

The build system is CMake therefore standard CMake flags can be used to influence the build and installation process. Several custom flags are defined to build the correct library. They are:

| Flag | Default | Use |
|------|---------|-----|
| HDBPP_CASS_BUILD_SHARED | ON | Build the shared library. This will also be installed if make install is run. |
| HDBPP_CASS_BUILD_STATIC | OFF | Build the static library. This will also be installed if make install is run. |
| HDBPP_CASS_BUILD_TESTS | OFF | Build unit tests |
| HDBPP_CASS_DEV_INSTALL | OFF | Install development files and libraries |
| HDBPP_CASS_INSTALL_SCRIPTS | OFF | Install cql script to /usr/share/libdhb++cassandra |

The following is a list of common useful CMake flags and their use:

| Flag | Use |
|------|-----|
| CMAKE_INSTALL_PREFIX | Standard CMake flag to modify the install prefix. |
| CMAKE_INCLUDE_PATH | Standard CMake flag to add include paths to the search path. |
| CMAKE_LIBRARY_PATH | Standard CMake flag to add paths to the library search path |

Using the above CMake flags above its possible to use tango and other libraries from non-standard locations. This is the preferred method, i.e. add all library and include paths to the above flags. 

For convenience there also exists five path flags to define the install location of various dependencies. This is to aid a situation where everything is installed non-standard. These can be passed in like normal CMake flags using the -D syntax, or exported into the local bash environment where they will be picked up by the build system.

| Path Flag | Description |
|-----------|-------------|
| HDBPP_CASS_TANGO_INSTALL_DIR | Install root for Tango Controls |
| HDBPP_CASS_OMIORB_INSTALL_DIR| Install root for Omniorb |
| HDBPP_CASS_CPP_INSTALL_DRIVER_DIR | Install root for the Datastax CPP Driver | 
| HDBPP_CASS_LIBHDBPP_INSTALL_DIR | Install root for Libhdbpp |
| HDBPP_CASS_ZMQ_INSTALL_DIR | Install root for libzmq |

### Passing CMake Lists

Note: to pass multiple paths (i.e. a string list to cmake), either an escaped semi colon must be used, or the list must be enclosed in quotes. Examples: 

* `-DCMAKE_INCLUDE_PATH=/here/there\;/some/where/else`
* `"-DCMAKE_INCLUDE_PATH=/here/there;/some/where/else"`
* `'-DCMAKE_INCLUDE_PATH=/here/tehre;/some/where/else'`

## Building

### Building Against Tango Controls 9.2.5a

**The debian package and source install place the headers under /usr/include/tango, yet the code includes tango via `#include <tango.h>` (to be compatible with Tango Controls 10 when it is released), so its likely you will need to pass at least one path via CMAKE_INCLUDE_PATH. In this case, set CMAKE_INCLUDE_PATH=/usr/include/tango or CMAKE_INCLUDE_PATH=/usr/local/include/tango, depending on your install method. Example:**

```bash
cmake -DCMAKE_INCLUDE_PATH=/usr/include/tango ..
```

### Example Build Sequence

First clone the repository:

```
git clone https://github.com/tango-controls-hdbpp/libhdbpp-elk.git
```

An out of source build is required by the CMakeLists file, so create a build directory to run CMake from:

```
cd libhdbpp-elk/build
```

Then configure with cmake:

```bash
cmake ..
```

Or for something with non-standard dependencies:

```bash
cmake \
    -DHDBPP_CASS_BUILD_TESTS=ON \
    -DCMAKE_INSTALL_PREFIX=/my/custom/location \
    -DCMAKE_INCLUDE_PATH=/path/to/custom/include \
    -DCMAKE_LIBRARY_PATH=/path/to/custom/library \
    ..
```

Then build:

```bash
make
```

## Installation

After the build has completed, simply run:

```
make install
```
