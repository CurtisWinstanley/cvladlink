#include "cvladlink.hpp"
#include "cvladlinkproc.hpp"

#include <thread>
#include <cassert>
#include <string.h>

extern "C" {
#include "socket.h"
#include "winsock2.h"

}


using std::cout;
using std::endl;
using std::list;
using std::map;
using std::to_string;

std::string myHost = "127.0.0.1";

void receiveAndDecide(CVLADLINKPROC* _packet)
{
	//NOW we add the packet to a data structure that stores all requests/received packets from the last X seconds.
    //This will allow us to implement the reliability of the protocol so when a sender times out and asks how 
    //the request for a mode change (or what any other functionality) is going, we check the message ID and see it is a timeout
    //message and loop through the list of packets, grab its STATUS and send it to whomever is requesting it. We store this in a
	//map data structure where the key is the src_comp_id concat with the message ID to make a packetID.

    if(_packet->message_id != STATUS_REQUEST_ID) //if the packet is not a status request
    {
		int mapIndex;
		// Convert both the integers to string
    	std::string srcComp = to_string(_packet->src_comp_id);
    	std::string msgID = to_string(_packet->message_id);
 
    	// Concatenate both strings
    	std::string packetID = srcComp + msgID;
 
    	// Convert the concatenated string
    	// to integer
    	mapIndex = std::stoi(packetID);

		recvMap.insert({mapIndex, _packet});
        //recvList.push_back(_packet);
		cout << "Ele Payload at index "<< mapIndex<< " is: " << recvMap[mapIndex]->payload <<std::endl;
    }
    else //if the packet IS a status request
    {
		//check payload for message ID being requested
		bool placeholder = true;
    }

	//OLD REFERENCE CODE BELOW
	//int ind = 0; //packet at desired index
    //std::list<CVLADLINKPROC*>::iterator it = recvList.begin();
    //std::advance(it, ind); // advance it ptr to desired index, may use later
    //NOTE: it->reqList[0]->CVLADLINKPROC so we must use double dereference (**)

}

void handleRequest(uint8_t _dest_sys_id, uint8_t _dest_comp_id, uint8_t _message_id,
	uint32_t _timeout, uint32_t _payload_len, char* _payload)
{

    char* buff = (char*)malloc((int)_payload_len);
    memcpy(buff, _payload, (int)_payload_len);

	CVLADLINKPROC* cvladLinkPacket = new CVLADLINKPROC();
	cvladLinkPacket->src_sys_id = 1; //HARDCODED
	cvladLinkPacket->src_comp_id = 5; //HARDCODED
	cvladLinkPacket->dest_sys_id = _dest_sys_id;
	cvladLinkPacket->dest_comp_id = _dest_comp_id;
	cvladLinkPacket->ack_req = 1; //HARDCODED
	cvladLinkPacket->message_id = _message_id;
	cvladLinkPacket->timeout = _timeout;
	cvladLinkPacket->payload_len = _payload_len;
	cvladLinkPacket->payload = buff;
	cvladLinkPacket->requestStatus = IN_PROGRESS;

    char* packet  = cvladLinkPacket->prepareLinkPacket();
    int destPort = cvladLinkPacket->chooseDestPort(cvladLinkPacket->dest_comp_id);

    /* pointer to socket_t struct */
    socket_t* sock = new socket_t();
    sock->port = destPort; //dest port
    strcpy(sock->hostname, myHost.c_str()); //dest IP
    sock->id = 99; //arbitrary ID for now

    int connectionResult = Open_Send_Socket(sock); //prepare a socket to send data on port X, returns 0 if OK
    int sentPacketSize = send(sock->s, (char*)packet, strlen((char*)packet), 0);

	cout << "testing thread" << std::endl;
	//std::this_thread::sleep_for(std::chrono::milliseconds(5000)); //sleep for 5s just to make sure the multithreading is working
	Sleep(5000);
	cout << "woke up!" << std::endl;

	return;
}


void sendreliable(uint8_t _dest_sys_id, uint8_t _dest_comp_id, uint8_t _message_id,
	uint32_t _timeout, uint32_t _payload_len, char* _payload, void(callback_function)(STATUS))
{
	cout << "entering sendreliable" << std::endl;
	std::thread t(handleRequest, _dest_sys_id, _dest_comp_id, _message_id, _timeout, _payload_len, _payload);
	cout << "hi" << std::endl;
    t.detach(); //???
	return;
}
