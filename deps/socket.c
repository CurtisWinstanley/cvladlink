/* Socket code was cleaned up by Gubbs in 2020 to reinforce some changes made for windoze 
   compatibility, and also error checking was improved in the recieve socket with timeout
   code */

#include <stdint.h>
#include "socket.h"
#include <errno.h>
#include <stdio.h> // For printf
#include <string.h> //For memset
#ifdef _WIN32
// Not sure if we need to add a library for close() here
#else 
#include <unistd.h> // for close
#endif

#ifdef _WIN32
uint32_t initialize_stupid_windoze_sockets = 0;  // On windows, in order to use socket() you need to first start the winsocket service by calling WSAStartup
#endif


int Open_Send_Socket(socket_t *sock_data)
{
	int ret, errval, errsize;
	int on = 1;
	struct timeval send_timeout;

#ifdef _WIN32 // On windows, in order to use socket() you need to first start the winsocket service by calling WSAStartup
	if (!initialize_stupid_windoze_sockets) {
		initialize_windows_sockets();
		initialize_stupid_windoze_sockets = 1;
	}
#endif
	
	// Initialize sockaddr_in structures
    memset(&sock_data->dest_addr, 0, sizeof (struct sockaddr_in));
    memset(&sock_data->local_addr, 0, sizeof (struct sockaddr_in));

	// Specify port and address for sending packets	
	sock_data->local_addr.sin_family = AF_INET;
	sock_data->local_addr.sin_addr.s_addr = INADDR_ANY;
    sock_data->local_addr.sin_port = 0; //htons((sock_data->port)*10);		// This is the line that gets us in trouble with sending/receiving on the same port-> so I multiply by 10)

	sock_data->dest_addr.sin_family = AF_INET;
#ifndef _WIN32
	inet_aton(sock_data->hostname, &sock_data->dest_addr.sin_addr);
#else
	sock_data->dest_addr.sin_addr.s_addr = inet_addr(sock_data->hostname);
#endif
	sock_data->dest_addr.sin_port = htons(sock_data->port);

	// Initialize socket
	sock_data->s = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock_data->s < 0 ) {
		errsize = sizeof(errval);
		#ifdef _WIN32
		//KE replaced socklen_t with size_t below (Feb 4 2020) as they're effectively the same
		ret = getsockopt(sock_data->s, SOL_SOCKET, SO_ERROR, (void *)&errval, (size_t*)&errsize); 
		#else
		ret = getsockopt(sock_data->s, SOL_SOCKET, SO_ERROR, (void *)&errval, (socklen_t*)&errsize); 
		#endif
		printf("socket initialization failed, returned %d\n",sock_data->s);
		perror("Socket Error");
		return -1; 
	}
	setsockopt(sock_data->s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	setsockopt(sock_data->s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	
	send_timeout.tv_sec = 0;
	send_timeout.tv_usec = SEND_TIMEOUT;
	setsockopt(sock_data->s, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout));

	// Bind socket to local address

	ret = bind(sock_data->s, (struct sockaddr *) &sock_data->local_addr, sizeof(struct sockaddr_in));
	if (ret) {
		printf("Bind send socket failed with error %d\n",ret);
        perror("Bind: ");
		printf("localhostname = %s\n",sock_data->hostname);
	}
	
	if( ret ) return -2; // was -1

	// Specify destination address for socket
#define CONNECT_SEND_SOCKET
#ifdef CONNECT_SEND_SOCKET
	ret = connect(sock_data->s, (struct sockaddr *) &sock_data->dest_addr, sizeof(struct sockaddr_in));
	if( ret ) {
		printf("Connect socket failed with error %d\n",ret);	
	}
	if( ret ) return -3; // was -1
#endif
	
	return 0;
}


int Open_RT_Send_Socket(socket_t *sock_data, int priority)
{	// This variation of open send socket sets a "Real Time' priorty field for the 
	// siznet series Ethernet switches. The Bell 205 ICD has identified the following priorities higher = higher prio)
	//		ERROR_SEND_ENET_PRIO 192
	//		FLIGHT_CONTROLS_ENET_PRIO 128
	//		FBW_STATUS_ENET_PRIO		64
	
	int ret, errval, errsize;
	int on = 1;
	struct timeval send_timeout;
	
#ifdef _WIN32 // On windows, in order to use socket() you need to first start the winsocket service by calling WSAStartup
	if (!initialize_stupid_windoze_sockets) {
		initialize_windows_sockets();
		initialize_stupid_windoze_sockets = 1;
	}
#endif
	
	// Initialize sockaddr_in structures
    memset(&sock_data->dest_addr, 0, sizeof (struct sockaddr_in));
    memset(&sock_data->local_addr, 0, sizeof (struct sockaddr_in));

	// Specify port and address for sending packets	
	sock_data->local_addr.sin_family = AF_INET;
	sock_data->local_addr.sin_addr.s_addr = INADDR_ANY;
	sock_data->local_addr.sin_port = htons(sock_data->port);

	sock_data->dest_addr.sin_family = AF_INET;
#ifndef _WIN32 // Winsock doesn't supprot inet_aton
	inet_aton(sock_data->hostname, &sock_data->dest_addr.sin_addr);
#else
	sock_data->dest_addr.sin_addr.s_addr = inet_addr(sock_data->hostname);
#endif
	sock_data->dest_addr.sin_port = htons(sock_data->port);

	// Initialize socket
	sock_data->s = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock_data->s < 0 ) {
		errsize = sizeof(errval);
		//KE replaced socklen_t with size_t below (Feb 4 2020) as they're effectively the same
		#ifdef _WIN32
		ret = getsockopt(sock_data->s, SOL_SOCKET, SO_ERROR, (void *)&errval, (size_t*)&errsize);
		#else
		ret = getsockopt(sock_data->s, SOL_SOCKET, SO_ERROR, (void *)&errval, (socklen_t*)&errsize);
		#endif
		printf("socket initialization failed, returned %d\n",sock_data->s);
		perror("Socket Error");
		return -1; 
	}
	setsockopt(sock_data->s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	setsockopt(sock_data->s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
	
	send_timeout.tv_sec = 0;
	send_timeout.tv_usec = SEND_TIMEOUT;
	setsockopt(sock_data->s, SOL_SOCKET, SO_SNDTIMEO, &send_timeout, sizeof(send_timeout));

	// Now check the priority field to ensure it is valied
	if ((priority>0) && (priority<256)) {
#ifndef _WIN32 //Windows does not have IP_TOS support, so skip this line
		setsockopt(sock_data->s, IPPROTO_IP,IP_TOS, &priority, sizeof(priority));
#endif
	}

	// Bind socket to local address
	ret = bind(sock_data->s, (struct sockaddr *) &sock_data->local_addr, sizeof(struct sockaddr_in));
	if (ret) {
		printf("Send Socket Bind failed with error %d\n",ret);
		printf("localhostname = %s\n",sock_data->hostname);
	}
	
	if( ret ) return -2; // was -1

	// Specify destination address for socket
	ret = connect(sock_data->s, (struct sockaddr *) &sock_data->dest_addr, sizeof(struct sockaddr_in));
	if( ret ) {
		printf("Connect socket failed with error %d\n",ret);	
	}
	if( ret ) return -3; // was -1

	return 0;
}

int Open_Recv_Socket(socket_t *sock_data)
{
	int ret;
	int on = 1;
	struct timeval rcv_timeout;

#ifdef _WIN32 // On windows, in order to use socket() you need to first start the winsocket service by calling WSAStartup
	if (!initialize_stupid_windoze_sockets) {
		initialize_windows_sockets();
		initialize_stupid_windoze_sockets = 1;
	}
#endif
	
 	// Initialize sockaddr_in structure
	memset(&sock_data->local_addr, 0, sizeof (struct sockaddr_in));

	// Initialize socket properties
	sock_data->s = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock_data->s < 0 ) return -1;

	setsockopt(sock_data->s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	rcv_timeout.tv_sec = 0;
	rcv_timeout.tv_usec = RCV_TIMEOUT; // forces standard timeout, Open_Recv_Socket_Timeout() below has more options
	setsockopt(sock_data->s, SOL_SOCKET, SO_RCVTIMEO, &rcv_timeout, sizeof(rcv_timeout));

	// Specify port and IP address for packet source
	sock_data->local_addr.sin_family = AF_INET;
	sock_data->local_addr.sin_addr.s_addr = INADDR_ANY;
	sock_data->local_addr.sin_port = htons(sock_data->port);

	// Bind socket to address
	ret = bind(sock_data->s, (struct sockaddr *)(&sock_data->local_addr), sizeof(struct sockaddr_in));
	if (ret) {
		printf("Recv Socket Bind failed with error %d\n", ret);
		printf("localhostname = %s\n", sock_data->hostname);
		return -1;
	}
	
	return 0;
}

int Open_Recv_Socket_Timeout(socket_t *sock_data, int timeout)
{   // The timeout parameter is the time in microseconds before returning from recv() with no packets present...tune it to the requirements of the data
	int ret, iVal = 0;
	int on = 1;
	unsigned int sz = sizeof(iVal);
	struct timeval rcv_timeout;

#ifdef _WIN32 // On windows, in order to use socket() you need to first start the winsocket service by calling WSAStartup
	if (!initialize_stupid_windoze_sockets) {
		initialize_windows_sockets();
		initialize_stupid_windoze_sockets = 1;
	}
#endif
	
 	// Initialize sockaddr_in structure
	memset(&sock_data->local_addr, 0, sizeof (struct sockaddr_in));
    
	// Initialize socket properties
	sock_data->s = socket(AF_INET, SOCK_DGRAM, 0); // AF_INET is the same as PF_INET, you could use IPPROTO_UDP instead of 0 but result is the same
	if (sock_data->s < 0) {
		DieWithError("socket() creation failed with error");
		return -1;
	}
    
	if (setsockopt(sock_data->s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))){
		DieWithError("socket option re-use address failed with error");
		return -1;
	}
    
   
#ifdef _WIN32 // windows wants timeouts in milliseconds not micro seconds...
	if (timeout > 0) {
		iVal = (int)(timeout / 1000);
	}
	else {
		iVal = (int)( 2000 ); // default to 2 seconds
	}
	if (setsockopt(sock_data->s, SOL_SOCKET, SO_RCVTIMEO, (char *)&iVal, sz)) {
		DieWithError("Recv socket option set timeout failed with error");
		return -1;
	}
#else
	if (timeout > 0) {
		rcv_timeout.tv_sec = (int)timeout / 10000000;
		rcv_timeout.tv_usec = timeout - 10000000 * rcv_timeout.tv_sec;
	}
	else {
		rcv_timeout.tv_sec = 2;    // 2 second default
		rcv_timeout.tv_usec = 0.0;
	}
	if (setsockopt(sock_data->s, SOL_SOCKET, SO_RCVTIMEO, &rcv_timeout, sizeof(rcv_timeout))) {
		DieWithError("Recv socket option set timeout failed with error");
		return -1;
	}
#endif
	    
	// Specify port and IP address for packet source
	sock_data->local_addr.sin_family = AF_INET;
	sock_data->local_addr.sin_addr.s_addr = htonl(INADDR_ANY); // though not technically required it helps me to understand when we are consistent so I added htonl...
	sock_data->local_addr.sin_port = htons(sock_data->port);
    
	// Bind socket to address
	ret = bind(sock_data->s, (struct sockaddr *)(&sock_data->local_addr), sizeof(struct sockaddr_in));
	if (ret) {
		DieWithError("Recv socket bind failed with error");
		return -1;
	}
	
	return 0;
}

#ifdef _WIN32 // On windows, in order to use socket() you need to first start the winsocket service by calling WSAStartup

void initialize_windows_sockets(void)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);	// We'll ask for ver 2.2
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
	}
}
#endif

void DieWithError(char *errorMessage)
{
#ifdef _WIN32
	printf("%s: %d\n", errorMessage, WSAGetLastError());
#else
	perror(errorMessage);
#endif
}


void processRcvFail(const char * sock_name, socket_t *sock_data, int timeout) {
	int reopen_socket;
	reopen_socket = 0;

#ifdef _WIN32
	if (WSAGetLastError() != WSAETIMEDOUT) { // don't print timeout errors - too common
		printf("SOCKET %s: \n", sock_name);
		DieWithError("Process recieve from failed with error code");
	} 
	if (WSAGetLastError() == WSAENOTCONN ) reopen_socket = 1;
	if (WSAGetLastError() == WSAEINVAL) reopen_socket = 1;
#else
	// This function is intended to process the possible reasons for failing to receive a packet
	if (errno == EACCES) printf("SOCKET %s Error EAccess: Permission to read socket was denied\n", sock_name);
	if (errno == EBADF) {
		printf("SOCKET %s Error Ebadf: Descriptor is not valid\n", sock_name);
		reopen_socket = 1;
	}
	if (errno == ECONNABORTED) printf("SOCKET %s Error Econaborted: Connection ended abnormally\n", sock_name);
	if (errno == ECONNRESET) printf("SOCKET %s Error Econreset: A connection with a remote socket was reset by that socket\n", sock_name);
	if (errno == EFAULT) printf("SOCKET %s Error EFault: Bad address while attempting to access buffer\n", sock_name);
	if (errno == EINTR) printf("SOCKET %s Error Eintr: Interrupted function call\n", sock_name);
	if (errno == EINVAL) printf("SOCKET %s Error Einval: Problem with one of the recv parameters\n", sock_name);
	if (errno == EIO) printf("SOCKET %s Error Eio: Input/Output error\n", sock_name);
	if (errno == ENOBUFS) printf("SOCKET %s Error Enobufs: Not enough buffer space\n", sock_name);
	if (errno == ENOTCONN) {
		printf("SOCKET %s Error Enotconn: The socket is not connected\n", sock_name);
		reopen_socket = 1;
	}
	if (errno == EOPNOTSUPP) printf("SOCKET %s Error Enotsupp: The requested operation is not supported\n", sock_name);

		// Comment out the timeout message since it can be pretty common
	if (errno == ETIMEDOUT) {
		//printf("%s Error Etimedout: Didn't get a response form the remote host\n",sock_name);
	}
	else {
		sleep(1);   // Sleep to allow other processes to continue working
	}
#endif

	if (reopen_socket) {
		printf("SOCKET Attempting to reopen socket: %s\n", sock_name);
		// Shut down the socket first
#ifdef _WIN32
		closesocket(sock_data->s); // windows version of close...
#else
		close(sock_data->s);
#endif
		if (Open_Recv_Socket_Timeout(sock_data, timeout) < 0) {
			printf("SOCKET Error opening %s socket, errono = %d\n", sock_name, errno);
		}
	}
}
