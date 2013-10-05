#include "stdio.h"
#include "stdlib.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

int fishTotal = 0;
char bukkitName[512];

void PrintMenu(int socketFD)
{
    char menu[] = "You be stealin' mah bukkit?!\n" 
        "\t1) Put fish into bukkit\n"
        "\t2) Name the bukkit\n"
        "\t3) STEAL THE BUKKIT!\n"
        "\t4) Quit\n";
    write(socketFD, menu, sizeof(menu));
}

void PutFishIntoBukkit(int socketFD)
{
    char intro[] = "How many fish do you want to put into the bukkit?\n";
    write(socketFD, intro, sizeof(intro));

    char input[32];
    memset(input, '\0', sizeof(input));
    read(socketFD, input, sizeof(input));

    int num_fish = atoi(input);
    fishTotal += num_fish;
}

void NameMahBukkit(int socketFD)
{
    char intro[] = "What do you want to name the bukkit?\n";
    write(socketFD, intro, sizeof(intro));

    memset(bukkitName, '\0', sizeof(bukkitName));
    read(socketFD, bukkitName, sizeof(bukkitName));
}

void STEAL_BUKKIT(int socketFD)
{
    char welcome[] = "YOU BE STEALIN' MAH BUKKIT!\n";
    write(socketFD, welcome, sizeof(welcome));

    char output[256];
    sprintf(output, "You stole %u fish inside %s\n", fishTotal, bukkitName);
    write(socketFD, output, strlen(output));
}

void HandleClient(int socketFD)
{
    char input[32];

    fishTotal = 0;
    strcpy(bukkitName, "mah bukkit");

    int bytes = 1;
    while(bytes > 0)
    {
        PrintMenu(socketFD);

        memset(input, '\0', sizeof(input));
        bytes = read(socketFD, input, 50);

        int choice = atoi(input);
        if(choice == 1)
        {
            PutFishIntoBukkit(socketFD);
        }
        else if(choice == 2)
        {
            NameMahBukkit(socketFD);
        }
        else if(choice == 3)
        {
            STEAL_BUKKIT(socketFD);
            fishTotal = 0;
            strcpy(bukkitName, "mah bukkit");
        }
        else if(choice == 4)
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            char error[] = "That is not an option!\n";
            write(socketFD, error, sizeof(error));
        }
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr, cli_addr;    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset((char *) &serv_addr, '\0',sizeof(serv_addr));
    int portno = 6632;
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

