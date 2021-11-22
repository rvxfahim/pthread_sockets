#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>  // for open
#include <unistd.h> // for close
#include <pthread.h>

/* void * cientThread(void *arg)

{
  printf("In thread\n");
  char message[1000];
  char buffer[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  // Create the socket. 
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  //Configure settings of the server address
 // Address family is Internet 
  serverAddr.sin_family = AF_INET;

  //Set port number, using htons function 
  serverAddr.sin_port = htons(7799);

 //Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
    strcpy(message,"this is a message from one of the client ");

   
   
       if( send(clientSocket , message , strlen(message) , 0) < 0)
    {
            printf("Send failed\n");
    }

    
    
   //Read the message from the server into the buffer
    if(recv(clientSocket, buffer, 1024, 0) < 0)
    {
       printf("Receive failed\n");
    }
    //Print the received message
    printf("Data received: %s\n",buffer);
    close(clientSocket);
    pthread_exit(NULL);
} */
pthread_mutex_t mutex;
pthread_mutex_t mutex1;
int east_count=0;
void *sendmessage(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        char message[1000];
        char buffer[1024];
        int clientSocket;
        struct sockaddr_in serverAddr;
        socklen_t addr_size;
        printf("inside sendmessage function east\n");
        // Create the socket.
        clientSocket = socket(PF_INET, SOCK_STREAM, 0);

        //Configure settings of the server address
        // Address family is Internet
        serverAddr.sin_family = AF_INET;

        //Set port number, using htons function
        serverAddr.sin_port = htons(7799);

        //Set IP address to localhost
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

        //Connect the socket to the server using the address
        addr_size = sizeof serverAddr;
        if (connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size) == 0)
        {
            printf("Connection established\n");
            while (1)
            {
                if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0)
                {
                    printf("Receive failed\n");
                }
                printf("waiting for permission to send request\n");
                //Print the received message

                printf("Data received: %s\n", buffer);
                printf("checking whether the string is "
                       "send_your_request"
                       " \n");
                //wait for server to acknowledge the connection and allow to send message
                if (strncmp(buffer, "send_your_request",17) == 0)
                {
                    printf("got permission\n");
                    buffer[0] = '\0';
                    break;
                }
            }
        }
        else
        {
            printf("Connection not established\n");
        }
        char cl_message[1000];
        sprintf(cl_message, "east");
        strcpy(message, cl_message);

        if (send(clientSocket, message, strlen(message), 0) < 0)
        {
            printf("Send failed\n");
        }
        //wait for server response before talking again.
        while (1)
        {
            if (recv(clientSocket, buffer, 1024, 0) < 0)
            {
                printf("Receive failed\n");
            }
            //Print the received message
            if(strlen(buffer)>0)
            {
                printf("got response from server\n");
                printf("Data received: %s\n", buffer);
                break;
            }
            
            
        }

        close(clientSocket);
        usleep(500 * 1000);
        printf("End client message Thread");
        pthread_mutex_unlock(&mutex);
        if(strncmp(buffer,"allow",5)==0)
        {   east_count++;
         printf("east_count is %d\n",east_count);
            
        }
        if(strncmp(buffer,"deny",4)==0)
        {   
            printf("no space left\n");
            printf("east_count is %d\n",east_count);
        }
        buffer[0] = '\0';
    }
}

void *sendmessage2(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex1);
        char message[1000];
        char buffer[1024];
        int clientSocket;
        struct sockaddr_in serverAddr;
        socklen_t addr_size;
        printf("inside sendmessage function 2\n");
        // Create the socket.
        clientSocket = socket(PF_INET, SOCK_STREAM, 0);

        //Configure settings of the server address
        // Address family is Internet
        serverAddr.sin_family = AF_INET;

        //Set port number, using htons function
        serverAddr.sin_port = htons(7799);

        //Set IP address to localhost
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

        //Connect the socket to the server using the address
        addr_size = sizeof serverAddr;
        if (connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size) == 0)
        {
            printf("Connection established\n");
            while (1)
            {
                if (recv(clientSocket, buffer, 1024, 0) < 0)
                {
                    printf("Receive failed\n");
                }
                //Print the received message
                if (recv(clientSocket, buffer, 1024, 0) > 0)
                {
                    //wait for server to acknowledge the connection and allow to send message
                    if (strcmp(buffer, "send_your_request") == 0)
                    {

                        break;
                    }
                }
            }
        }
        else
        {
            printf("Connection not established\n");
        }
        char cl_message[1000];
        sprintf(cl_message, "this is a message from second client %lu", pthread_self());
        strcpy(message, cl_message);

        if (send(clientSocket, message, strlen(message), 0) < 0)
        {
            printf("Send failed\n");
        }

        //Read the message from the server into the buffer
        if (recv(clientSocket, buffer, 1024, 0) < 0)
        {
            printf("Receive failed\n");
        }
        //Print the received message
        printf("Data received: %s\n", buffer);
        close(clientSocket);
        //sleep(1);
        pthread_mutex_unlock(&mutex1);
    }
}
int main()
{

    pthread_t thread1;
    pthread_t thread2;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex1, NULL);
    pthread_create(&thread1, NULL, sendmessage, NULL);
    //pthread_create(&thread2, NULL, sendmessage2, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex1);
    pthread_join(thread1, NULL);
    //pthread_join(thread2, NULL);
    //pthread_exit(NULL);

    return 0;
}