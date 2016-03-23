#include <zmq.hpp>
#include <string>
#include <iostream>
#include "msgdefs.hpp"


int main (int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: zmqclient <start> <range>" << std::endl;
		return -1;
	}
	uint64_t start = std::strtoull(argv[1], nullptr, 10);
	uint64_t range = std::strtoull(argv[2], nullptr, 10);

	//  Prepare our context and socket
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REQ);
	socket.connect("tcp://localhost:9000");

	for (uint64_t i = 0; i < range; ++i)
	{
		// Send the request to the server
		zmq::message_t request = prime_request::make(start + i);
		socket.send(request);

		// Receive the response
		zmq::message_t response;
		socket.recv(&response);

		// Handle the results
		bool result;
		if (prime_response::extract(response, result))
		{
			std::cout << (start + i) << "=" << (result ? "T" : "F") << std::endl;
		}
	}
	return 0;
}
