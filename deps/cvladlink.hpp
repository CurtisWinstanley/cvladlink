#ifndef CVLADLINK_HPP
#define CVLADLINK_HPP

#include <iostream>
#include <list>
#include <map>
//#include "cvladlinkproc.hpp"

class CVLADLINKPROC;
/*
	Contains general interface for cvlad_link protocol

*/
enum STATUS
{
	IN_PROGRESS = 0,
	SENT = 2,
	ACCEPTED = 4,
	FAILED = 6,
};

/* Global Variables */
const uint16_t STATUS_REQUEST_ID = 99;

static std::map<int, CVLADLINKPROC*> recvMap;


void sendreliable(uint8_t dest_sys_id, uint8_t dest_comp_id, uint8_t message_id,
	uint32_t timeout, uint32_t payload_len, char* payload, void(callback_function)(STATUS));


void receiveAndDecide(CVLADLINKPROC* _packet);


#endif