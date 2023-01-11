#pragma once
#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")
#define WIN32_LEAN_AND_MEAN  // Prevent windows.h from including winsock...put this in the project defines
#include <windows.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif



#define SEND_TIMEOUT			1000
#define RCV_TIMEOUT				500000

////
// Closing socket macro. The function is not the same under windows & unix.
//
#ifdef _WIN32
#define CLOSESOCKET(sockfd) (closesocket(sockfd))
#else
#define CLOSESOCKET(sockfd) (close(sockfd))
#endif

typedef struct {
	unsigned short port;
	char hostname[16];
	struct sockaddr_in dest_addr;
	struct sockaddr_in local_addr;
	int s;
	int id;
} socket_t;

typedef struct {
	socket_t *socket;
	void *data;
	int size;
	int pending;
} packet_t;


int Open_Send_Socket(socket_t *);
int Open_Recv_Socket(socket_t *);
int Open_RT_Send_Socket(socket_t *, int);
int Open_Recv_Socket_Timeout(socket_t *sock_data, int timeout);


/* Added by Gubbs after the great socket code cleanup of 2020: */
#ifdef _WIN32
void initialize_windows_sockets(void);
#endif

void DieWithError(char *errorMessage);
void processRcvFail(const char * sock_name, socket_t *sock_data, int timeout);
