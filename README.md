# primality
Client-Server Primality Tester Sample App

Functionality:
  - Calculates the primality of an unsigned integer on a remote server

Build Instructions:
 - requires ZeroMQ with C++ language abstraction (package libzmq3-dev has both)
 - requires cppUnit (package libcppunit-dev)
 - requires gcc with c++11 support (compiled on 4.8.2)
 - "make all" will create the client, server, and test suite.

Support Notes:
  - Further hardening to be done including error/exception handling and reporting errors after network failures

Design Notes:
  - Support for multiple simultaneous clients   
  - Proxy handles message distribution
  - Multiple worker threads for parallel request processing
  - Worker threads use concurrent async calls for parallel computation
```  
#---------#   #---------#   #---------#
| Client  |   | Client  |   | Client  |
#---------#   #---------#   #---------#
|   REQ   |   |   REQ   |   |   REQ   |
'----+----'   '----+----'   '----+----'
     |             |             |
     '-------------+-------------'
                   |
             .-----+-----.
             |  ROUTER   |
             +-----------+
             |   Proxy   |
             +-----------+
             |  DEALER   |
             '-----+-----'
                   |
     .-------------+-------------.
     |             |             |
     v             v             v
.---------.   .---------.   .---------.
|   REP   |   |   REP   |   |   REP   |
#---------#   #---------#   #---------#
|  Worker |   |  Worker |   |  Worker |
#---------#   #---------#   #---------#
```
