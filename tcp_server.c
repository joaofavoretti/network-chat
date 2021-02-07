/* tc_serve_toupper.c */

#include "unix_headers.h"

#define SHOW_MESSAGES 1

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("usage: ./tcp_server port\n");
        return 1;
    }

    // srand(time(NULL));
    // char ways_to_say_goodbye[][20] = {"Byte!\n\0", "Adios!\n\0", "Arrivederci!\n\0", "Au Revoir!\n\0", "Adeus!\n\0", "Auf Wiedersehen!\n\0", "Sayōnara!\n\0", "Do svidaniya!\n\0", "Annyeong!\n\0", "Slan!\n\0", "Tot ziens!\n\0"};

    printf("(setup) Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, argv[1], &hints, &bind_address);
    
    printf("(setup) Local address: ");
    char local_addr[100];
    getnameinfo(bind_address->ai_addr, bind_address->ai_addrlen, local_addr, 100, 0, 0, NI_NUMERICHOST);
    printf("%s (%s)\n", local_addr, argv[1]);

    printf("(setup) Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
    if (!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "(error) socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }


    printf("(setup) Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "(error) bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    freeaddrinfo(bind_address);

    printf("(setup)Listening...\n");
    if (listen(socket_listen, 10) < 0) {
        fprintf(stderr, "(error) listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("(setup) Waiting for connections...\n");
    while(1) {
        fd_set reads;
        reads = master;
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0) {
            fprintf(stderr, "(error) select() failed. (%d)\n", GETSOCKETERRNO());
            return 1;
        }

        SOCKET sender_it;
        for (sender_it = 1; sender_it <= max_socket; ++sender_it) {
            if (FD_ISSET(sender_it, &reads)) {
                if (sender_it == socket_listen) {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);

                    SOCKET socket_client = accept(socket_listen, (struct sockaddr *) &client_address, &client_len);
                    if (!ISVALIDSOCKET(socket_client)) {
                        fprintf(stderr, "(error) accept() failed. (%d)\n", GETSOCKETERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket) max_socket = socket_client;

                    char wellcome_message[] = "=======\nWelcome to the chat.\n=======\n\0";
                    send(socket_client, wellcome_message, strlen(wellcome_message) + 1, 0);

                    char address_buffer[100];
                    getnameinfo((struct sockaddr *) &client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
                    printf("(new_connection) New connection from %s\n", address_buffer);
                } else {
                    char message[4096];
                    int bytes_received = recv(sender_it, message, 4096, 0);

                    if (bytes_received <= 0) {
                        FD_CLR(sender_it, &master);
                        CLOSESOCKET(sender_it);
                        continue;
                    }

                    SOCKET receiver_it;
                    for (receiver_it = 1; receiver_it <= max_socket; ++receiver_it) {
                        if (FD_ISSET(receiver_it, &master)) {
                            if (receiver_it == socket_listen && SHOW_MESSAGES) {
                                printf(">> %s", message);
                            }
                            if ((receiver_it == socket_listen) || receiver_it == sender_it) {
                                continue;
                            } else {
                                send(receiver_it, message, bytes_received, 0);
                            }
                        }
                    }
                }
            }
        }
    }
    printf("(close) Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

    printf("(close) Finished.\n");
    return 0;
}
