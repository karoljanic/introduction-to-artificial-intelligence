#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "Bot.hpp"


int main(int argc, char *argv[]) {
    if(argc != 5) {
        std::cout << "Invalid number of arguments. 5 arguments were excepted\n";
        return -1;
    }

    int socket_descriptor;
    struct sockaddr_in server_address;
    char server_message[16], client_message[16];

    // Creating socket
    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_descriptor < 0) {
        std::cout << "Unable to create socket\n";
        return -1;
    }
    std::cout << "Socket created successfully\n";

    // Set port and IP the same as server-side
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));
    server_address.sin_addr.s_addr = inet_addr(argv[1]);

    // Send connection request to server
    if(connect(socket_descriptor, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cout << "Unable to connect\n";
        return -1;
    }
    std::cout << "Connected with server successfully\n";

    // Receive the server message
    memset(server_message, '\0', sizeof(server_message));
    if(recv(socket_descriptor, server_message, sizeof(server_message), 0) < 0) {
        std::cout << "Error while receiving server's message\n";
        return -1;
    }
    std::cout << "Server message: " << server_message << '\n';

    memset(client_message, '\0', sizeof(client_message));
    strcpy(client_message, argv[3]);

    // Send the message to server
    if(send(socket_descriptor, client_message, strlen(client_message), 0) < 0) {
        std::cout << "Unable to send message\n";
        return -1;
    }

    // Initializing bot
    int player{atoi(argv[3])};
    unsigned short depth{atoi(argv[4])};
    Bot bot{depth, HeuristicType::SUCCESS_OR_DEFEAT, player == 1};

    // Game loop
    bool endGame{false};

    int msg, move;

    while(!endGame) {
        memset(server_message, '\0', sizeof(server_message));
        if(recv(socket_descriptor, server_message, sizeof(server_message), 0) < 0) {
            printf("Error while receiving server's message\n");
            return -1;
        }

        printf("Server message: %s\n", server_message);
        msg = atoi(server_message);
        move = msg % 100;
        msg = msg / 100;
        
        if(move != 0) {
            int row = move / 10;
            int col = move % 10;

            bot.opponentMove(row, col);
        }

        if(msg == 0 || msg == 6) {
            printf("Your move:\n");
    
            move = bot.move();
            //move = bot.randomMove();

            memset(client_message, '\0', sizeof(client_message));
            sprintf(client_message, "%d", move);
            if(send(socket_descriptor, client_message, strlen(client_message), 0) < 0) {
                printf("Unable to send message\n");
                return -1;
            }
            printf("Client message: %s\n", client_message);
        }
        else {
            endGame = true;
            switch( msg ) {
                case 1 : printf("You won.\n"); break;
                case 2 : printf("You lost.\n"); break;
                case 3 : printf("Draw.\n"); break;
                case 4 : printf("You won. Opponent error.\n"); break;
                case 5 : printf("You lost. Your error.\n"); break;
            }
        }
   }


    // Close socket
    close(socket_descriptor);

    return 0;

}