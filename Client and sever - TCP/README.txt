a printable characters counting (TCP) server. Clients connect to the server and send it a stream of bytes. The server counts how
many of the bytes are printable and returns that number to the client. The server also maintains
overall statistics on the number of printable characters it has received from all clients. When the
server terminates, it prints these statistics to standard output.

#Compile and run on Linux#
Compile with "gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 pcc_server.c" and "gcc -O3 -D_POSIX_C_SOURCE=200809 -Wall -std=c11 pcc_client.c".

Run the server with the following arguments: 
argv[1]: server’s port.

Run the client with the following arguments: 
argv[1]: server’s IP address.
argv[2]: server’s port.
argv[3]: path of the file to send. 

The client recieves a file path and sends it's content to the server, recieves statistics from the server and prints the no. of printable characters in the file.


