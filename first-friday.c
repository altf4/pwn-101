#include "stdio.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

void PrintWelcome(int socketFD)
{
    char welcome[] = "Hey, did you know that the Phoenix 2600 meets every first friday of the month?\n";
    write(socketFD, welcome, sizeof(welcome));
}

void HandleClient(int socketFD)
{
    int bytes = 1;
    char input_month[64];
    char input_year[64];
    char format[128];
    char output[512];

    PrintWelcome(socketFD);

    while(bytes > 0)
    {
        memset(input_month, '\0', sizeof(input_month));
        memset(input_year, '\0', sizeof(input_year));
        memset(format, '\0', sizeof(format));
        memset(output, '\0', sizeof(output));

        char month_message[] = "What month would you like to come?\n";
        write(socketFD, month_message, sizeof(month_message));
        bytes = read(socketFD, input_month, sizeof(input_month)-1 );

        char year_message[] = "What year would you like to come?\n";
        write(socketFD, year_message, sizeof(year_message));
        bytes = read(socketFD, input_year, sizeof(input_year)-1 );

        char display_message[] = "As a reminder, here is the month that you should come! See you there!\n";
        write(socketFD, display_message, sizeof(display_message));


        input_month[strlen(input_month) -1] = '\0';
        input_year[strlen(input_year) -1] = '\0';

        sprintf(format, "cal %s %s", input_month, input_year);

        FILE *file = popen(format, "r");
        fread(output, sizeof(output)-1, 1, file) ;
        pclose(file);

        write(socketFD, output, sizeof(output));
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr, cli_addr;    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset((char *) &serv_addr, '\0',sizeof(serv_addr));
    int portno = 7654;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on bind");
        exit(-1);
    }
    if(listen(sockfd,5) < 0)
        perror("ERROR on listen");
    int clilen = sizeof(cli_addr);

    while (1)
    {
        int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
        }
        int pid = fork();
        srand (time(NULL));
        if (pid == 0)
        {
            //Closes the old socket, not ours
            struct timeval tv;
            tv.tv_sec = 20;
            tv.tv_usec = 0;
            setsockopt(newsockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
            close(sockfd);
            
            HandleClient(newsockfd);

            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    }
}

