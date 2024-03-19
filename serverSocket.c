#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cJSON.h>


struct configs{ // Default value set.
	serverIPAddr = "192.168.128.2",
	int sourcePortNum = 9876

	int destPortNumUDP = 8765

	int destPortNumTCPHeadSYN = 9999;
	int destPortNumTCPTailSYN = 8888;

	int portNumTCPPreProbe = 7777;
	int portNumTCPPostProbe = 6666;

	int UDPPayloadSize = 1000;

	int interMeasureTime = 15;

	int numUDPPackets = ;
	int UDPPacketTTL = 255;
};

int main(){

	char server_message[300] = "You have reached the server.\n";

	// creates the server socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// define server address
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	inet_pton(AF_INET, "192.168.128.2", &(server_address.sin_addr)); // Change IP to that of the CLIENT.

	// bind the socket to an address
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

	// Listen for incoming connections, set backlog to 5 for now.
	listen(server_socket, 5);

	// client socket
	int client_socket;
	client_socket = accept(server_socket, NULL, NULL); // Other two fields left as NULL for now.

	// send message
	send(client_socket, server_message, sizeof(server_message), 0);

	// close the socket when done.
	close(server_socket);
}
