
.PHONY: all clean

all: zmqclient zmqserver test-primer

zmqclient: zmqclient.cpp msgdefs.hpp
	./build-zmq -g -p -l -I/usr/local/include -L/usr/local/lib -lzmq -std=c++11 -q $@

zmqserver: zmqserver.cpp primer.hpp msgdefs.hpp
	./build-zmq -g -p -l -I/usr/local/include -L/usr/local/lib -lzmq -std=c++11 -q $@

test-primer: test-primer.cpp primer.hpp msgdefs.hpp
	g++ test-primer.cpp -lcppunit -std=c++11 -ggdb -o $@

clean:
	rm zmqclient
	rm zmqserver
	rm test-primer
