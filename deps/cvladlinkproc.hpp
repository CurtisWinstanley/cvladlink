#ifndef CVLADLINKPROC_HPP
#define CVLADLINKPROC_HPP
#include "cvladlink.hpp"


#include <cassert>
#include <string>
#include <string.h>

/*
	This class is the data structure for processing CVLAD Link requests and processing them

*/


class CVLADLINKPROC
{

private:
	char* serializeData(char*, int); //

public:
	CVLADLINKPROC(); //contructor
	int getMessage(); //For debugging, it just prints the message stored in the buffer
	char* deserializeData(char*, int); //
	char* prepareLinkPacket(); //used to prepare a packet before sending
	void parseLinkPacket(char*); //
    int chooseDestPort(int _src);


	/*CVLAD Link packet structure*/
    uint8_t src_sys_id;
	uint8_t src_comp_id;
	uint8_t dest_sys_id;
	uint8_t dest_comp_id;
	uint8_t ack_req;
	uint16_t message_id;
	uint32_t timeout;
	uint32_t payload_len;
	char* payload;
	STATUS requestStatus;

	//void(callback_function)(STATUS);
};

#endif