#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){

	// creating a socket...

	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0); // Network socket created

	// give the socket an address

	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	inet_pton(AF_INET, "192.168.128.2/24", &(server_address.sin_addr)); // Change IP to that of the SERVER.

	int connect_success = connect(network_socket, (struct sockaddr) &server_address, sizeof(server_address));

	char server_response[300];

	if(connect_success == -1){
		printf("Client failed to connect to the Server!\n");
	}

	else if(connect_success == 0){
		// Client recieves data from Server which is stored in server_response
		recv(network_socket, &server_response, sizeof(server_response), 0);
		printf("Server Data: %s\n", server_response);
	}
	
	// Close the socket when done...
	close(sock);

	return 0;
}