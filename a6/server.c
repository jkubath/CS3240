/*
	Jonah Kubath
	CS 3240
	12/10/2017
	A6
  Description:
    A simple server in the internet domain using TCP
    The port number is passed as an argument
*/
#define _BSD_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <magic.h>
#include <sys/stat.h>
#include "apue.h"


void error(char *msg)
{
        perror(msg);
        exit(1);
}

int main(int argc, char *argv[])
{
    //Size of the data to read from the browser
    int BUFFER_SIZE = 400;
    int sockfd, newsockfd, portno;
    unsigned int clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char errorMessage[] = "HTTP/1.1 200 OK\r\nServer:Flaky Server/1.0.0\r\nContent-Type:text/html; charset=ISO-8859-4 \r\nContent-Length: 23\r\n\r\n<h1>File not Found</h1>\0";
    if (argc < 2) {
            fprintf(stderr,"ERROR, no port provided\n");
            exit(1);
    }
    //Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
            err_sys("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    //Bind the port number to the socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            err_sys("ERROR on binding");
    //Listen for the client information
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
            err_sys("ERROR on accept");
    bzero(buffer,BUFFER_SIZE);

    char *get;
    char *fileRequested;
    long fsize = 0;
    char *fileData;
    const char *type;
    struct stat info;
    FILE *f;
    magic_t m;
    char *message = calloc(400, sizeof(char));
    int errReturn = 0;

    while (1)
    {

            bzero(message, 400);
            fsize = 0;
            //Reset the buffer
            bzero(buffer, BUFFER_SIZE);

            n = read(newsockfd,buffer, BUFFER_SIZE - 1);


            if (n < 0)
                    err_sys("ERROR reading from socket");
            //Copy the pointer to the read buffer
            char *copy = buffer;
            get = strsep(&copy, " ");

            if(strcmp(get, "GET") == 0) {
                //Read the file Requested
                fileRequested = strsep(&copy, " ");
                //Remove the / from the file
                fileRequested = fileRequested + 1;
                errReturn = printf("Get File - %s\n", fileRequested);
                if(errReturn < 0){
                  err_sys("Error Printing");
                }
                // open the magic library, with MAGIC_MIME_TYPE flag
                // so that the mime type of the file is returned instead
                // of human readable information
                m = magic_open(MAGIC_MIME_TYPE);

                // load the default database
                magic_load(m, NULL);

                // detect the mime type of a file named `fileRequested`
                type = magic_file(m, fileRequested);

                if (type) {
                        //Open the file in read binary
                        f = fopen(fileRequested, "rb");
                        // char *replace = NULL;
                        // if(strcmp(type, "audio/mpeg") == 0){
                        //   replace = calloc(strlen("audio/mpeg-3"), 1);
                        //   strncpy(replace, "audio/mpeg-3", strlen("audio/mpeg-3"));
                        //   type = replace;
                        // }

                        if(f == NULL) {
                                fsize = 0;
                        }
                        else {
                                //Get the size of the file
                                stat(fileRequested, &info);
                                //Read the file
                                fsize = info.st_size;
                                errReturn = printf("File size - %ld\n", fsize);
                                if(errReturn < 0){
                                  err_sys("Error Printing");
                                }
                                fileData = calloc(fsize * sizeof(char) + 2, sizeof(char));
                                long error = fread(fileData, 1, fsize, f);
                                if(error < fsize) {
                                        errReturn = printf("Did not read enough\n");
                                        if(errReturn < 0){
                                          err_sys("Error Printing");
                                        }
                                }


                                fclose(f);

                                errReturn = sprintf(message, "HTTP/1.1 200 OK\r\nServer:Flaky Server/1.0.0\r\nContent-Type:%s\r\nContent-Length:%ld\r\n\r\n", type, fsize);
                                if(errReturn < 0){
                                  err_sys("Error Generating Return Message");
                                }
                        }
                        // if(replace != NULL)
                        //   free(replace);
                }
                else {
                        errReturn = printf("An error occurred: %s\n", magic_error(m));
                        if(errReturn < 0){
                          err_sys("Error Printing");
                        }
                        strncpy(message, errorMessage, strlen(errorMessage));
                        fsize = strlen(errorMessage);
                }

                //Closes the magic database
                magic_close(m);

                //Get the file path
                //Write back to the browser
                errReturn = printf("Generating Message\n");
                if(errReturn < 0){
                  err_sys("Error Printing");
                }
                errReturn = printf("%s\n", message);
                if(errReturn < 0){
                  err_sys("Error Printing");
                }
                //printf("Bytes needed to write = %ld\n", fsize);
                n = write(newsockfd, message, strlen(message));
                if(fsize != 0) {
                        n = write(newsockfd, fileData, fsize);
                }
                else {
                  n = write(newsockfd, errorMessage, strlen(errorMessage));
                }
                //printf("Bytes written - %d\n", n);
                errReturn = printf("Message Sent\n\n");
                if(errReturn < 0){
                  err_sys("Error Printing");
                }

                if (n < 0) error("ERROR writing to socket");

            }// End of if
            else {
                    //printf("Not a GET Request\n\n");
                    fileData = NULL;
            }
            if(fileData != NULL)
              free(fileData);
            type = NULL;

            //free(message);
    } //Done reading

    free(message);
    close(newsockfd);
    return 0;
}
