#include <cassert>
#include <string>
#include <iostream>
#include <string.h>
#include <thread>
#include <list>
#include <vector>
#include "cvladlink.hpp"

extern "C" {
#include "socket.h"
#include "winsock2.h"

}

using namespace std;

#include "iostream"
#include <../deps/cvladlinkproc.hpp>


const int SupervisorPort = 7777;


/*
*   handleRequest is a function used for the threads
*   char* buff is a pointer to the message we are handling, len is the size of that message, and
*   fromMbs is a flag that will indicate if we are dealing with a packetbuilder packet or a CVLAD link packet.
*/
void handleRequest(char* buff, int len)
{
    cout << " Handling thread: " << this_thread::get_id() << std::endl;
    cout << "received " << (char*)buff << " with size " << len << std::endl; //DEBUG

    std::string myHost = "127.0.0.1"; //FOR NOW ALL REQUESTS ARE SENT THROUGH LOCAL HOST

    CVLADLINKPROC* packet = new CVLADLINKPROC(); //Make a new CVLAD Link object
    char* receivedUTF8Data = packet->deserializeData(buff, len); //deserialize the byte stream

    cout << "UTF8 Data Received: " << (char*)receivedUTF8Data << std::endl; // DEBUGING
    packet->parseLinkPacket(receivedUTF8Data); //parse the data and set up the CVLADLINKPROC data structure
    cout << (char*)packet->payload << std::endl;

    receiveAndDecide(packet);


    std::this_thread::sleep_for(std::chrono::milliseconds(5000)); //sleep for 5s just to make sure the multithreading is working

    cout << "end thread" << std::endl;

    return;
}


int main()
{


    std::string myHost = "127.0.0.1";
    char output[1024];

    /* pointer to socket_t struct */
    socket_t* sock = new socket_t();
    sock->port = SupervisorPort; //port to listen on
    strcpy(sock->hostname, myHost.c_str()); //IP to listen on
    sock->id = 99;


    packet_t* pack = new packet_t();
    pack->socket = sock;
    pack->data = output; // set the packet data = to the buffer NOTE: must typecast (char*)pack->data if the actual contents is desired
    pack->size = 0; // size of the packet data


    int connection = Open_Recv_Socket(sock); //listen for an incoming packet on port X
    while(true){
        cout << "LOOPING" <<std::endl;

        int recvpacket = recv(pack->socket->s, (char*)pack->data, strlen((char*)pack->data) , 0);
        pack->size = recvpacket;

        cout << "received " << (char*)pack->data << " with size " << recvpacket <<std::endl;  //DEBUG
    
        /* Send the data to a thread for handling */
        if (recvpacket > 0) { //if we recieved data then start a thread to service
            thread t(handleRequest, (char*)pack->data, recvpacket);
            t.join(); //allows execution to continue independantly
        }
        
    }
    closesocket(pack->socket->s);


}
