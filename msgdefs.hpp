#ifndef __MSG_DEFS_HPP__
#define __MSG_DEFS_HPP__

#include <zmq.hpp>


namespace prime_request
{
// Create a new zmq request for a prime number
zmq::message_t make(uint64_t suspect)
{
	// ToDo: Endian conversion required for cross platform (ntohll required)
	// - prefer to migrate to strings or something like Google Protocol Buffers
	zmq::message_t request(8);

	// Request data is unaligned
	memcpy(request.data(), &suspect, 8);
	return request;
}

// Extract the data from a zmq prime request
bool extract(zmq::message_t &request, uint64_t &suspect)
{
	// TodDo: Endian conversion required for cross platform (ntohll required)
	// - prefer to migrate to strings or something like Google Protocol Buffers
	if (request.size() != 8)
		return false;

	// Request data is unaligned
	memcpy(&suspect, request.data(), 8);
	return true;
}
} // end namespace prime_request


namespace prime_response
{
// Create a new zmq response
zmq::message_t make(bool result)
{
	zmq::message_t response(1);
	char *data = static_cast<char *>(response.data());
	data[0] = result ? 1 : 0;
	return response;
}

// Extract the data from a zmq prime response
bool extract(zmq::message_t &response, bool &result)
{
	if (response.size() != 1)
		return false;
	char *data = static_cast<char *>(response.data());
	result = data[0] == 1;
	return true;
}
}// end namespace prime_response

#endif
