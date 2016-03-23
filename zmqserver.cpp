#include <zmq.hpp>
#include <string>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <future>
#include "msgdefs.hpp"
#include "primer.hpp"

// Worker handles request/response processing for a client
class Worker
{
public:
	Worker(int thread_num, zmq::context_t &context)
	: m_thread_num(thread_num)
	, m_context(context)
	{ }

	// Thread entry point for the worker thread
	void operator()()
	{
		// Setup the context and socket
		zmq::socket_t socket(m_context, ZMQ_REP);
		socket.connect("inproc://workers");

		while (true)
		{
			// Receive request from the client
			zmq::message_t request;
			socket.recv(&request);

			uint64_t suspect;
			if (prime_request::extract(request, suspect))
			{
				// Evaluate primality
				bool result = Primer::Check(suspect);

				//  Send reply back to client
				zmq::message_t response = prime_response::make(result);
				socket.send(response);
			}
		}
	}

private:
	// Thread number. Used for debugging only at this point
	int m_thread_num;

	// Zeromq processing context
	zmq::context_t &m_context;
};


int main (int argc, char *argv[])
{
	//  Prepare our context and sockets
	zmq::context_t context(1);

	// The frontend captures incoming requests from the clients
	zmq::socket_t frontend(context, ZMQ_ROUTER);
	frontend.bind("tcp://*:9000");

	// The backend distributes requests to the worker threads
	// - The worker threads are in the same process
	zmq::socket_t backend(context, ZMQ_DEALER);
	backend.bind("inproc://workers");

	// Create a pool of worker threads
	// - Allow us to support multiple simultaneous clients
	const auto max_workers = 2;
	std::vector<std::thread> pool;
	for (auto i = 0; i < max_workers; ++i)
		pool.push_back(std::thread{Worker(i, std::ref(context))});

	// Run the Proxy
	// - Distributes incoming requests to the worker threads
	// - This is a blocking call for the life of the server
	zmq::proxy(frontend, backend, NULL);

	// Wait for workers to complete
	for (auto &t : pool)
	{
		if (t.joinable())
			t.join();
	}

	return 0;
}




