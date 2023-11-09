/* https://gist.github.com/pixyj/5202810#file-http_client-c */

#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <unistd.h>

#include <stdlib.h>

/************************************************************************************************
 *  Try it out:
 *  $ git clone https://gist.github.com/5202810.git http_client
 *  $ cd http_client
 *  $ python -m SimpleHTTPServer
 *  $ make
 *  $ ./http_client
 *
 ************************************************************************************************/

/************************************************************************************************
 *   Blocking call to send HTTP Request and Get Response.
 *
 *   'request' must contain necessary http request data including http version,
 *method and data 'response' is allocated dynamically by the function. Must be
 *freed by caller
 *
 *   Output: 0  - FAILURE in creating sockets/sending data
 *           1  - SUCCESS
 *
 ************************************************************************************************/

#define RECV_SIZE 1024

int sendHttpDataAndGetResponse(const char *ipAddress, int port, char *request,
                               char **response) {

  int socketDesc;
  struct sockaddr_in server;

  char *recvData = calloc(RECV_SIZE, 1);

  socketDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (socketDesc == -1) {
    printf("%s Could not create socket\n", __FUNCTION__);
    return 0;
  }

  server.sin_addr.s_addr = inet_addr(ipAddress);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  if (connect(socketDesc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    puts("connect error");
    return 0;
  }

  if (send(socketDesc, request, strlen(request), 0) < 0) {
    printf("%s Send failed\n", __FUNCTION__);
    return 0;
  }

  int iteration = 1;
  char *intermediateData = recvData;
  while (recv(socketDesc, intermediateData, RECV_SIZE, 0) > 0) {
    // printf("intermediateData %s\n", intermediateData);
    iteration += 1;
    recvData = realloc(recvData, iteration * RECV_SIZE);
    memset(recvData + (iteration - 1) * RECV_SIZE, 0, RECV_SIZE);
    // intermediateData = recvData + (iteration-1)*RECV_SIZE-1;
    intermediateData = recvData + strlen(recvData);
  }
  close(socketDesc);

  *response = recvData;
  return 1;
}

int main(int argc, char *argv[]) {
  const char *addr = argv[1];
  int port = atoi(argv[2]);

  char *response = NULL;
  sendHttpDataAndGetResponse(addr, port, "GET /Makefile HTTP/1.1\r\n\r\n",
                             &response);
  printf("Response received\n %s\n", response);
  free(response);

  sendHttpDataAndGetResponse(addr, port, "GET / HTTP/1.1\r\n\r\n", &response);
  printf("Second response\n %s\n", response);
  free(response);

  return 0;
}
