/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h> /* required for the definition of bool in C99 */
#include "crypto/aes.h"
#include <math.h>

#include "mavlink_crypto.h"

#define BUFSIZE 2041 //1024

/*
 * error - wrapper for perror
 */
void udpservererror(char *msg) {
  perror(msg);
  exit(1);
}

int udpserver(int argc, char **argv) {



  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
   char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */

char *copy;

  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    udpservererror("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
         (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
       sizeof(serveraddr)) < 0) 
    udpservererror("ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1) {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
         (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      udpservererror("ERROR in recvfrom");
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t i;
    int j = 0;
int len;
       
 
uint8_t * a=(uint8_t *)buf;

        printf("\n--------------------------------------------------------\n");
    printf("Header: \n");
    for ( i = 0; i < 6; i++)
    {
              printf("%2x ", a[i]);
      }
    printf("\n\n"); 



copy = (char *)malloc (256* sizeof (*copy));
       printf("Datagram: \n");
for ( j = 0; j < n-8; j++){
for ( i = 6; i < n-2; i++)
    {

      printf("%2x ", buf[i]); 

     copy[j] = buf[i];
      j++;

  }} 

   printf("\n\n");
   printf("copy: \n");   

for ( i = 0; i < n-8; i++)
{
   printf("%2x ", copy[i]);
   }

    printf("\n\n");
printf("the size of copy is %d\n", strlen (copy));
  printf("\n\n");


aes_decrypt((uint8_t *)copy, strlen (copy), key, iv);
  printf("decrypted text:\n");
  for (i = 0; i <n-8; i++) {
      printf("%2x ",copy[i]);
    }
  printf("\n\n");
 	
     

    /* 
     * gethostbyaddr: determine who sent the datagram
     */
    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
              sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if (hostp == NULL)
      udpservererror("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      udpservererror("ERROR on inet_ntoa\n");
   // printf("server received datagram from %s (%s)\n",hostp->h_name, hostaddrp);
    //printf("server received %d/%d bytes: %s\n", strlen(buf), n, buf);
    
    /* 
     * sendto: echo the input back to the client 
     */
    n = sendto(sockfd, buf, n, 0, 
           (struct sockaddr *) &clientaddr, clientlen);
    if (n < 0) 
      udpservererror("ERROR in sendto");
  }
}


int main(int argc, char **argv) {
  int status;
  status = udpserver(argc, argv);
  
  return status;
}
