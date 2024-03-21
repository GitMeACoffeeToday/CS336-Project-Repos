#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "/Home/Documents/CS336 CLIENT C Files/cJSON.h"

struct configs{ // Default value set.
	char serverIPAddr[100];
	int sourcePortNum;

	int destPortNumUDP;

	int destPortNumTCPHeadSYN;
	int destPortNumTCPTailSYN;

	int portNumTCPPreProbe;
	int portNumTCPPostProbe;

	int UDPPayloadSize;

	int interMeasureTime;

	int numUDPPackets;
	int UDPPacketTTL;
};

int main(int argc, char *argv[]){

	if(argc != 2){
		printf("Invalid File Name Arguement.\n");
		exit(1);
	}
	else{
		FILE* configfile = fopen(argv[1], "r");
		if(configfile == 0){
			printf("Error with Opening File.\n");
			exit(1);
		}
		else{
			fgets(BUFFER, 1000, configfile); // Pass the JSON file as text to cJSON.
			cJSON* config = cJSON_Parse(BUFFER);

			fseek(configfile, 0, SEEK_END);
			long file_size = ftell(configfile);
			fseek(configfile, 0, SEEK_SET);

			char BUFFER[file_size];
			fread(BUFFER, sizeof(char), file_size, configfile);

			char* serverIP = cJSON_GetObjectItemCaseSensitive(config, "serverIPAddr")->valuestring;
			printf("serverIP from CONFIG: %s\n", serverIP);

			cJSON_Delete(config); // Deallocate memory afterwards.
		}
		close(configfile); // close file afterwards.
	}

	// creating a socket...
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0); // Network socket created

	// give the socket an address

	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	inet_pton(AF_INET, "192.168.128.2", &(server_address.sin_addr)); // Change IP to that of the SERVER.

	int connect_success = connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address));

	char server_response[300];

	if(connect_success == -1){
		printf("Client failed to connect to the Server!\n");
	}

	else if(connect_success == 0){
		printf("...Connect Success!\n");
		
		// Client recieves data from Server which is stored in server_response
		recv(network_socket, &server_response, sizeof(server_response), 0);
		printf("Server Data: %s\n", server_response);
	}
	
	// Close the socket when done...
	close(network_socket);

	return 0;
}