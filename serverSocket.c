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
	fread(BUFFER, sizeof(char), file_size, configfile);

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


int main(){

	char server_message[300] = "You have reached the server.\n";
	char client_response[2000];

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
	recv(client_socket, client_response, sizeof(client_response), 0);

	printf("Client Response: %s\n", client_response);

	char client_response[2000];



	// close the socket when done.
	close(server_socket);
}
