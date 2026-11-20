//
// Created by root on 6/23/24.
//
// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define ADDRESS "10.0.0.211"

void flip_case(char *str, int len) {
	for (int i = 0; i < len; i++) {
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] = str[i] - 'a' + 'A';
		else if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] = str[i] - 'A' + 'a';
	}
}

int main() {
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;

	if (inet_pton(AF_INET, ADDRESS, &address.sin_addr) <= 0) {
		perror("Invalid address or Address not supported");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen failed");
		close(server_fd);
		exit(EXIT_FAILURE);
	}

	while (1) {
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
			perror("accept failed");
			close(server_fd);
			exit(EXIT_FAILURE);
		}

		for(int i = 0; i < BUFFER_SIZE; ++i) {
			buffer[i] = '\0';
		}

		int n = read(new_socket, buffer, BUFFER_SIZE);
		printf("Received: %s\n", buffer);
		flip_case(buffer, n);
		send(new_socket, buffer, n, 0);
		printf("Processed and sent back: %s\n", buffer);
		close(new_socket);
	}

	return 0;
}
