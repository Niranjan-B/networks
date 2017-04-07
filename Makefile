all: networks

server_or: server_or_c
	./server_or

server_and: server_and_c
	./server_and

edge: edge_c
	./edge
	

networks: server_or_c server_and_c edge_c client_c

server_or_c: server_or.c
	gcc server_or.c -o server_or

server_and_c: server_and.c
	gcc server_and.c -o server_and

edge_c: edge.c
	gcc edge.c -o edge

client_c: client.c
	gcc client.c -o client

clean:
	rm client edge server_and server_or
