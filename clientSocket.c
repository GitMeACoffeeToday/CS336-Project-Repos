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

void setConfig(struct configs* a, FILE* configfile){ // Initializes the config struct for later use.
	fseek(configfile, 0, SEEK_END);
	long file_size = ftell(configfile);
	fseek(configfile, 0, SEEK_SET);

	char BUFFER[file_size];

	cJSON* config = cJSON_Parse(BUFFER);

	// Copy in data.
	strcpy(a->serverIPAddr, cJSON_GetObjectItemCaseSensitive(config, "serverIPAddr")->valuestring);
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "sourcePortNum")->valueint != -1){
		a->sourcePortNum = cJSON_GetObjectItemCaseSensitive(config, "sourcePortNum")->valueint;
	}
	else{
		a->sourcePortNum = 9876;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "destPortNumUDP")->valueint != -1){
		a->destPortNumUDP = cJSON_GetObjectItemCaseSensitive(config, "destPortNumUDP")->valueint;
	}
	else{
		a->destPortNumUDP = 8765;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "destPortNumTCPHeadSYN")->valueint != -1){
		a->destPortNumTCPHeadSYN = cJSON_GetObjectItemCaseSensitive(config, "destPortNumTCPHeadSYN")->valueint;
	}
	else{
		a->destPortNumTCPHeadSYN = 9999;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "destPortNumTCPTailSYN")->valueint != -1){
		a->destPortNumTCPTailSYN = cJSON_GetObjectItemCaseSensitive(config, "destPortNumTCPTailSYN")->valueint;
	}
	else{
		a->destPortNumTCPTailSYN = 8888;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "portNumTCPPreProbe")->valueint != -1){
		a->portNumTCPPreProbe = cJSON_GetObjectItemCaseSensitive(config, "portNumTCPPreProbe")->valueint;
	}
	else{
		a->portNumTCPPreProbe = 7777;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "portNumTCPPostProbe")->valueint != -1){
		a->portNumTCPPostProbe = cJSON_GetObjectItemCaseSensitive(config, "portNumTCPPostProbe")->valueint;
	}
	else{
		a->portNumTCPPostProbe = 6666;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "UDPPayloadSize")->valueint != -1){
		a->UDPPayloadSize = cJSON_GetObjectItemCaseSensitive(config, "UDPPayloadSize")->valueint;
	}
	else{
		a->UDPPayloadSize = 1000;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "interMeasureTime")->valueint != -1){
		a->interMeasureTime = cJSON_GetObjectItemCaseSensitive(config, "interMeasureTime")->valueint;
	}
	else{
		a->interMeasureTime = 15;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "numUDPPackets")->valueint != -1){
		a->numUDPPackets = cJSON_GetObjectItemCaseSensitive(config, "numUDPPackets")->valueint;
	}
	else{
		a->numUDPPackets = 6000;
	}
	//##################################//##################################//##################################
	if(cJSON_GetObjectItemCaseSensitive(config, "UDPPacketTTL")->valueint != -1){
		a->UDPPacketTTL = cJSON_GetObjectItemCaseSensitive(config, "UDPPacketTTL")->valueint;
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
			/*
			fseek(configfile, 0, SEEK_END);
			long file_size = ftell(configfile);
			fseek(configfile, 0, SEEK_SET);

			char BUFFER[file_size];

			fread(BUFFER, sizeof(char), file_size, configfile);

			cJSON* config = cJSON_Parse(BUFFER);

			char* serverIP = cJSON_GetObjectItemCaseSensitive(config, "serverIPAddr")->valuestring;
			printf("serverIP from CONFIG: %s\n", serverIP);

			cJSON_Delete(config); // Deallocate memory afterwards.
			*/
			struct configs configuration; // Declaring the struct
			setConfig(&configuration, configfile);

			printf("IP ADDRESS: %s\n", configuration->serverIPAddr);
			printf("Source Port Number: %d\n", configuration->sourcePortNum);

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

		}
		fclose(configfile); // close file afterwards.
	}
	return 0;
}