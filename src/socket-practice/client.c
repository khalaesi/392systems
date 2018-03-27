#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "my.h" 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       my_str("usage ");
       my_str(argv[0]);
       my_str(" hostname port\n");
       exit(0);
    }
    portno = my_atoi(argv[2]);

    // Prompts the user for a username
    char username[256];
    int isValid = 0;
    // Username cannot be empty
    while(isValid == 0) {
        my_str("Please enter a username: ");
        bzero(username, 256);
        read(0, username, 255);
        // Sets the new line character to a null terminator
        username[my_strlen(username) - 1] = '\0';
        if (my_strlen(username) == 0) {
            isValid = 0;
            my_str("Your username is invalid.\n");
        }
        else {
            isValid = 1;
        }
        my_str("The length of your username is: ");
        my_int(my_strlen(username));
        my_char('\n');
    }
    my_str("Your username is: ");
    my_str(username);
    my_char('\n');

    // Creates the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        error("ERROR, no such host\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    // Connect the socket to the address of the server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    // Receive welcome message
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    if (n < 0) error("ERROR writing to socket");
    my_str(buffer);
    my_char('\n');

    // Prompt for message to send
    while(1) {
        my_str("Please enter the message: ");
        bzero(buffer,256);
        read(0,buffer,255);

        // Sends a message to the server
        n = write(sockfd,buffer,my_strlen(buffer));
        if (n < 0) error("ERROR writing to socket");
        bzero(buffer,256);

        // Receives confirmation from the server that message was received
        n = read(sockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        my_str(buffer);
    }

    // Receives farewell message
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");
    my_str(buffer);
    my_char('\n');

    // Closes the connection
    close(sockfd);
    return 0;
}
