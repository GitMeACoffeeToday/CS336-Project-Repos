#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "cJSON.h"
#include "cJSON.c"

#include <sys/time.h>

long timeInMilliseconds() {
    struct timeval time;

    gettimeofday(&time,NULL);
    return (time.tv_sec*1000)+(time.tv_usec/1000);
}


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

void establishConfiguration(struct configs* serverConfig){
	//char server_message[300] = "You have reached the server.\n";
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
	//send(client_socket, server_message, sizeof(server_message), 0);
	recv(client_socket, client_response, sizeof(client_response), 0); // Recieve config file.

	setConfig(serverConfig, sizeof(client_response), client_response);

	// close the socket when done.
	close(server_socket);
}

void sendToClientTCP(char* BUFFER, int buffer_length){
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
	send(client_socket, BUFFER, buffer_length, 0);

	// close the socket when done.
	close(server_socket);
}

void serverProbingPhase(struct configs* serverConfig){

	// creates the server socket
	char client_message[100];
	int server_socket;
	server_socket = socket(AF_INET, SOCK_DGRAM, 0);

	// define server address
	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	inet_pton(AF_INET, "192.168.128.2", &(server_address.sin_addr)); // Change IP to that of the CLIENT.

	// bind the socket to an address
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

	long low_entropy_seconds1;
	long low_entropy_seconds2;	

	long high_entropy_seconds1;
	long high_entropy_seconds2;

	// recieve low entropy packets
	for(int i = 0; i < serverConfig->numUDPPackets; i++){
		recvfrom(server_socket, NULL, NULL, 0, (struct sockaddr*) &server_address, NULL);
		if(i == 0){
			low_entropy_seconds1 = timeInMilliseconds();
		}
		if(i == serverConfig->numUDPPackets - 1){
			low_entropy_seconds2 = timeInMilliseconds();
		}
	}

	for(int i = 0; i < serverConfig->numUDPPackets; i++){
		recvfrom(server_socket, NULL, NULL, 0, (struct sockaddr*) &server_address, NULL);
		if(i == 0){
			high_entropy_seconds1 = timeInMilliseconds();
		}
		if(i == serverConfig->numUDPPackets - 1){
			high_entropy_seconds2 = timeInMilliseconds();
		}
	}


	int compressionThresholdTime = abs((high_entropy_seconds2 - high_entropy_seconds1) - (low_entropy_seconds2 - low_entropy_seconds1);

	printf("Final Calculation: %d\n", abs((high_entropy_seconds2 - high_entropy_seconds1) - (low_entropy_seconds2 - low_entropy_seconds1)));

	sendToClientTCP(abs((high_entropy_seconds2 - high_entropy_seconds1) - (low_entropy_seconds2 - low_entropy_seconds1) + '0', 1));

	//recvfrom(server_socket, client_message, 100, 0, (struct sockaddr*) &server_address, NULL);
	//printf("Client Response: %s\n", client_message);
	close(server_socket);
}


int main(){
	struct configs serverConfig;
	establishConfiguration(&serverConfig);
	serverProbingPhase(&serverConfig);
}
