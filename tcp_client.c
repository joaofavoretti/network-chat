/* tcp_client.c */

#include "unix_headers.h"

int main(int argc, char **argv) {

	if (argc < 4) {
		fprintf(stderr, "usage: username tcp_client hostname port\n");
		return 1;
	}

	printf("Confirguring remote address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo *peer_address;
	if (getaddrinfo(argv[2], argv[3], &hints, &peer_address)) {
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}
	
	printf("Remote address is: ");
	char address_buffer[100];
	char service_buffer[100];
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST);
	printf("%s %s\n", address_buffer, service_buffer);

	printf("Creating socket...\n");
	SOCKET socket_peer;
	socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_peer)) {
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}

	printf("Connecting...\n");
	if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen)) {
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		return 1;
	}

	freeaddrinfo(peer_address);

	printf("Connected.\n");
    printf("To send data, enter text followed by ENTER\n");

	while (1) {
		fd_set reads;
		FD_ZERO(&reads);
		FD_SET(socket_peer, &reads);

		#if !defined(_WIN32)
			FD_SET(fileno(stdin), &reads);
		#endif

		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;
		if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0) {
			fprintf(stderr, "select() failed. (%d).\n", GETSOCKETERRNO());
			return 1;
		}

		if (FD_ISSET(socket_peer, &reads)) {
			char read[4096];
			int bytes_received = recv(socket_peer, read, 4096, 0);
			if (bytes_received < 1) {
				printf("Connection closed by peer.\n");
				break;
			}
			printf("[%d bytes] > %.*s", bytes_received, bytes_received, read);
		}

		if(FD_ISSET(0, &reads)) {
            char message[4096] = { 0 };
            char send_data[3840];
            if (!fgets(send_data, 3840, stdin)) break;
            strcat(message, argv[1]);
            strcat(message, ": ");
            strcat(message, send_data);
            int bytes_sent = send(socket_peer, message, strlen(message), 0);
            printf("[%d bytes] > %.*s", bytes_sent, bytes_sent, message);
		}
	}

	printf("Closing socket...\n");
	CLOSESOCKET(socket_peer);

	printf("Finished.\n");

	return 0;

}
