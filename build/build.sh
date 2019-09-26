cmake "-DCMAKE_INCLUDE_PATH=/usr/local/include/tango;/home/osboxes/tango-dev/HDB++" "-DCMAKE_LIBRARY_PATH=/home/osboxes/tango-dev/HDB++/libhdbpp-master/lib;/usr/local/lib" "-DCMAKE_CXX_FLAGS=-ggdb3" ..
make
make install
