#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "cJSON.h"
#include "cJSON.c"

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

void setConfig(struct configs* a, long int file_size, char* BUFFER){ // Initializes the config struct for later use.

	cJSON* config = cJSON_Parse(BUFFER);

	// Copy in data.
	strcpy(a->serverIPAddr, cJSON_GetObjectItemCaseSensitive(config, "serverIPAddr")->valuestring);
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "sourcePortNum")->valuestring, "-1") != 0){
		a->sourcePortNum = atoi(cJSON_GetObjectItemCaseSensitive(config, "sourcePortNum")->valuestring);
	}
	else{
		a->sourcePortNum = 9876;
	}
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "destPortNumUDP")->valuestring, "-1") != 0){
		a->destPortNumUDP = atoi(cJSON_GetObjectItemCaseSensitive(config, "destPortNumUDP")->valuestring);
	}
	else{
		a->destPortNumUDP = 8765;
	}
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "destPortNumTCPHeadSYN")->valuestring, "-1") != 0){
		a->destPortNumTCPHeadSYN = atoi(cJSON_GetObjectItemCaseSensitive(config, "destPortNumTCPHeadSYN")->valuestring);
	}
	else{
		a->destPortNumTCPHeadSYN = 9999;
	}
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "destPortNumTCPTailSYN")->valuestring, "-1") != 0){
		a->destPortNumTCPTailSYN = atoi(cJSON_GetObjectItemCaseSensitive(config, "destPortNumTCPTailSYN")->valuestring);
	}
	else{
		a->destPortNumTCPTailSYN = 8888;
	}
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "portNumTCPPreProbe")->valuestring, "-1") != 0){
		a->portNumTCPPreProbe = atoi(cJSON_GetObjectItemCaseSensitive(config, "portNumTCPPreProbe")->valuestring);
	}
	else{
		a->portNumTCPPreProbe = 7777;
	}
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "portNumTCPPostProbe")->valuestring, "-1") != 0){
		a->portNumTCPPostProbe = atoi(cJSON_GetObjectItemCaseSensitive(config, "portNumTCPPostProbe")->valuestring);
	}
	else{
		a->portNumTCPPostProbe = 6666;
	}
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "UDPPayloadSize")->valuestring, "-1") != 0){
		a->UDPPayloadSize = atoi(cJSON_GetObjectItemCaseSensitive(config, "UDPPayloadSize")->valuestring);
	}
	else{
		a->UDPPayloadSize = 1000;
	}
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "interMeasureTime")->valuestring, "-1") != 0){
		a->interMeasureTime = atoi(cJSON_GetObjectItemCaseSensitive(config, "interMeasureTime")->valuestring);
	}
	else{
		a->interMeasureTime = 15;
	}
	//##################################//##################################//##################################
	if(strcmp(cJSON_GetObjectItemCaseSensitive(config, "numUDPPackets")->valuestring, "-1") != 0){
		a->numUDPPackets = atoi(cJSON_GetObjectItemCaseSensitive(config, "numUDPPackets")->valuestring);
	}
	else{
		a->numUDPPackets = 6000;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "UDPPacketTTL")->valueint != -1){
		a->UDPPacketTTL = atoi(cJSON_GetObjectItemCaseSensitive(config, "UDPPacketTTL")->valuestring);
	}
	else{
		a->UDPPacketTTL = 255;
	}
	//##################################//##################################//##################################
	cJSON_Delete(config); // Deallocate memory afterwards when done setting up config file.
}


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
			fseek(configfile, 0, SEEK_END);
			long file_size = ftell(configfile);
			fseek(configfile, 0, SEEK_SET);

			char BUFFER[file_size];
			fread(BUFFER, sizeof(char), file_size, configfile);


			struct configs configuration; // Declaring the struct
			setConfig(&configuration, file_size, BUFFER);

			printf("IP ADDRESS: %s\n", configuration.serverIPAddr);
			printf("Source Port Number: %d\n", configuration.sourcePortNum);

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
				printf("Server Response: %s\n", server_response);
				send(network_socket, &BUFFER, file_size, 0); // Send config file.
			}
	
			// Close the socket when done...
			close(network_socket);

		}
		fclose(configfile); // close file afterwards.
	}
	return 0;
}