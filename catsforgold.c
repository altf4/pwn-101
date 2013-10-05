#include "stdio.h"
#include "stdlib.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

int goldTotal = 0;
char *cats[] = {"      \\    /\\\n"
    "       )  ( ')\n"
    "      (  /  )\n"
    "       \\(__)|\n"};

void PrintWelcome(int socketFD)
{
    char welcome[] = "Welcome to Cats For Gold! (TM)\n\tTrade in your unwanted valuables for felines!\n";
    write(socketFD, welcome, sizeof(welcome));
}

void PrintMenu(int socketFD)
{
    char menu[] = "What would you like to do?\n" 
        "\t1) Deposit Gold\n"
        "\t2) Withdraw Gold\n"
        "\t3) Trade Gold in for a Cat\n"
        "\t4) Quit\n";
    write(socketFD, menu, sizeof(menu));
}

void Depositgold(int socketFD)
{
    goldTotal += 50;
    char deposited[] = "Deposited!\nYou now have this much gold: ";
    write(socketFD, deposited, sizeof(deposited));
    char gold_string[24];
    snprintf(gold_string, sizeof(gold_string), "%d\n", goldTotal);
    write(socketFD, gold_string, strlen(gold_string));
}

void Withdrawgold(int socketFD)
{
    goldTotal -= 50;
    if(goldTotal < 0)
        goldTotal = 0;

    char withdraw[] = "Withdrawl!\nYou now have this much gold: ";
    write(socketFD, withdraw, sizeof(withdraw));
    char gold_string[24];
    snprintf(gold_string, sizeof(gold_string), "%d\n", goldTotal);
    write(socketFD, gold_string, strlen(gold_string));
}

void ObtainCat(int socketFD)
{
    char intro[] = "One cute kitty, coming right up!\n";
    write(socketFD, intro, sizeof(intro));
    write(socketFD, cats[0], strlen(cats[0]));
}

void Quit(int socketFD)
{
    exit(1);
}

void PrintFlag(int socketFD)
{
    char buffer[128];
    FILE *file = fopen("flag.txt", "r");
    fread(buffer, 1, sizeof(buffer), file);
    write(socketFD, buffer, strlen(buffer));
}

void PrintError(int socketFD)
{
    char error[] = "That is not an option!\n";
    write(socketFD, error, sizeof(error));
}

void HandleClient(int socketFD)
{
    int bytes = 1;
    void *functionTable[5];
    char input[32];
    functionTable[0] = Depositgold;
    functionTable[1] = Withdrawgold;
    functionTable[2] = ObtainCat;
    functionTable[3] = Quit;
    functionTable[4] = PrintError;

    PrintWelcome(socketFD);

    while(bytes > 0)
    {
        PrintMenu(socketFD);
        
        memset(input, '\0', sizeof(input));
        bytes = read(socketFD, input, 100);

        int choice = atoi(input);
        choice --;

        if((choice >= 0) && (choice <= 3))
        {
            void (*foo)(int) = functionTable[choice];
            foo(socketFD);
        }
        else
        {
            void (*foo)(int) = functionTable[4];
            foo(socketFD);
        }
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr, cli_addr;    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset((char *) &serv_addr, '\0',sizeof(serv_addr));
    int portno = 6543;
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

