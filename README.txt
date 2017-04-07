Name -> Niranjan Bhaskar

ID -> 4134875355

Work Done -> created 3 server programs and 1 client program. Two of three 
server programs namely the OR and AND servers communicate via UDP, EDGE 
communicates via TCP to the client and UDP to the two servers. Learnt how sockets (TCP and UDP) are 
created at a level close to the kernel. Learnt how a server can act both
as a client and server according to specific scenarios. The client sending
large data files over TCP to the server at once rather than calling send()
multiple times like in UDP. How to receive and process data and send back 
the that data to the client. How to re-arrange packets on the EDGE server
when they go out of order. How to assign numbers to each computation line.
The client reads a text file via which is given via command-line, stores
the entire file into a buffer, sends it over to EDGE server via tcp socket.
In the EDGE server, each computation line is read, appended with a number,
stored in a buffer. Once all the lines are obtained, based on whether it's
an AND or OR operation, the computation line is sent to the respective server.
The AND/OR server processes it and sends it back to the edge server via the 
same socket descriptor.

Code Files -> 
1) server_and = program which takes a computation line from the edge server,
splits the operands based on ",", and's both operands character by character,
stores the result into a buffer. After AND operation, appends the computation
number got earlier and sends it to EDGE server.
2) server_or = program which takes a computation line from edge server, splits
the operand based on ",", or's both operands character by character, stores
the result into  a buffer. After OR operation, appends the computation number
got earlier and sends it to EDGE server
3) edge = this program gets all the buffer data from client, appends a packet
number to it, and sends it to either AND/OR server. Gets the computed result
from the AND/OR server, stores it in a buffer, forwards it to the client.
4) client = reads from a text file, pads 0's and forwards it to the EDGE.
Wait's for the result's from the EDGE server and display's it to the client.

To run the program -> 
1) make all
2) make server_or
3) make server_and
4) make edge
5) ./client <filename.txt>

Format of all messages exchanged ->
client to edge -> and,0101010101,0000000011,11 where 11 is the num(range from 0 - 99)
edge to server_and -> and,0000000000,1111111111,10
edge to server_or -> or,1111111111,0101010101,01
edge to client -> 1111111111 (the respective result with 10 characters)

Idiosyncrasy of project ->
Project does not fail under any test cases, however while displaying the result it displays
all 10 digits rather than limiting it the largest of two operands.

Reused code ->
Reused the string spliting code from stack-overflow. (Around 6 -7 lines)
