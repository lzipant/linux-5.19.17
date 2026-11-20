//
// Created by root on 6/23/24.
//
// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define SERVER_IP "10.0.0.211"

int main() {
	int sock = 0;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	char message[BUFFER_SIZE] = {0};

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket creation error");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
		perror("Invalid address/ Address not supported");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		perror("Connection failed");
		return -1;
	}

	printf("Enter a message: ");
	fgets(message, BUFFER_SIZE, stdin);
	message[strcspn(message, "\n")] = 0; // Remove trailing newline

	send(sock, message, strlen(message), 0);
	read(sock, buffer, BUFFER_SIZE);
	printf("Response from server: %s\n", buffer);

	close(sock);

	return 0;
}
