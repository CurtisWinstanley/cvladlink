#include "cvladlinkproc.hpp"
#include "cvladlink.hpp"
#include <vector>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string.h>
#include <bitset>

using std::cout;
using std::endl;
using std::to_string;


CVLADLINKPROC::CVLADLINKPROC() //constructor
{


}
/*Private Functions*/
char* CVLADLINKPROC::serializeData(char* _s, int _payloadLen)
{
	cout << strlen(_s) <<"packet contents to serialize:" << (char*)_s << std::endl;
	int n = strlen(_s);

	std::byte* bytes = new std::byte[n];
	char* serData = new char[n*2]; //*2 because each byte is rep by 2 characters
	for (size_t i = 0; i < n; i++) {
		bytes[i] = std::byte(_s[i]);
		//cout << i <<"byte->" << std::hex << std::to_integer<int>(bytes[i]) << std::endl; // used to verify if the byte conversions work
	}

	int k = 0;
	for(size_t j = 0; j < n*2; j = j+2)
	{
		std::ostringstream byteHexRep;
		
		byteHexRep << std::hex << std::to_integer<int>(bytes[k]);
		std::string strByteHexRep = byteHexRep.str();
		serData[j] = strByteHexRep[0];
		serData[j+1] = strByteHexRep[1];
		//cout << "coped Byte ->" << serData[j] << serData[j+1] << std::endl;
		k++;
	}
 
	int totalPacketLen = ((14 + payload_len) * 2) + 2;
	serData[totalPacketLen] = '\0'; //gets rid of unwanted characters at end of char array
	return serData;

}

char* CVLADLINKPROC::deserializeData(char* _r, int _packetLen)
{
	int n = _packetLen;
    cout << "n" << n << std::endl;

	std::string strMsgHexRep(_r); //turns the recived data into a string
	std::string receivedUTF8MsgStr;
	for(size_t j = 0; j < n; j = j+2)
	{
		
		std::string strByteHexRep = "0x";
		strByteHexRep.push_back(strMsgHexRep[j]);
		strByteHexRep.push_back(strMsgHexRep[j+1]);
		
		cout << "coped Byte ->" << strByteHexRep << std::endl;
		int byteDecValue;

		std::istringstream ost(strByteHexRep); //take hex string and make it into a decimal value
		ost >> std::hex >> byteDecValue;

		receivedUTF8MsgStr.push_back(static_cast<char>(byteDecValue)); //static cast will do UTF8 conversion
		//cout << "int" << byteDecValue << "letter" << static_cast<char>(byteDecValue) <<std::endl; //for debugging
	}
	//cout << "recieved UTF8 Message:" << receivedUTF8MsgStr << std::endl;

	int k = receivedUTF8MsgStr.length(); //put the received string into a char array ptr
	char* receivedUTF8Msg = new char[k + 1];
	strcpy(receivedUTF8Msg, receivedUTF8MsgStr.c_str());

	return receivedUTF8Msg;
}

/*Public functions*/
int CVLADLINKPROC::getMessage()
{
	return 0;
}

char* CVLADLINKPROC::prepareLinkPacket()
{

	/*
	This section of code checks how many zeros should be padded on each header field based on its length in bytes
	*/
	std::string valueToBePadded = std::to_string(payload_len);
	std::string newHeaderValue;
	int paddedZeros = 4 - valueToBePadded.length();
	for(int  k = 0; k < paddedZeros; k++)
	{
		newHeaderValue.append("0");
	}
	newHeaderValue.append(valueToBePadded);
	cout << "padded" << newHeaderValue << std::endl;


	/*
	
	*/
	std::string tempstr = std::to_string(src_sys_id) + std::to_string(src_comp_id) + std::to_string(dest_sys_id)
	+ std::to_string(dest_comp_id) + std::to_string(ack_req) + std::to_string(message_id) + std::to_string(timeout)
	+ newHeaderValue + payload;
	//cout <<std::to_string(payload_len) + payload << "and " << unsigned(payload_len) << std::endl;

	int n = tempstr.length();
	char* packet = new char[n+1]; //packet length plus 1
	strcpy(packet, tempstr.c_str());


	cout << "Packet Contents: "<<(char*)packet << std::endl;
	char* serializedPacket = serializeData(packet, payload_len);
	cout << "New Packet Contents: "<<(char*)serializedPacket << std::endl; //just to check to see if the serialization worked
	return (char*)serializedPacket;
	///PUT INTO BYTE ARRAY
}

void CVLADLINKPROC::parseLinkPacket(char* _r)
{
	//now parse the packet
	src_sys_id = uint8_t(_r[0]);
	src_comp_id = uint8_t(_r[1]);
	dest_sys_id = uint8_t(_r[2]);
	dest_comp_id = uint8_t(_r[3]);
	ack_req = uint8_t(_r[4]);

	std::string tempA;
	tempA.push_back(_r[5]);
	tempA.push_back(_r[6]);
	message_id = std::stoi(tempA);

	std::string tempB;
	tempB.push_back(_r[7]);
	tempB.push_back(_r[8]);
	tempB.push_back(_r[9]);
	tempB.push_back(_r[10]);
	timeout = std::stoi(tempB);

	std::string tempC;
	tempC.push_back(_r[11]);
	tempC.push_back(_r[12]);
	tempC.push_back(_r[13]);
	tempC.push_back(_r[13]);
	payload_len = std::stoi(tempC);

	std::string tempD(_r);
	tempD = tempD.substr(15, 15 + payload_len - 1);
	int n = tempD.length();
	char* pyld = new char[n+1]; //packet length plus 1
	strcpy(pyld, tempD.c_str());
	payload = pyld;
	//cout << "SRCSYS " << (char*)payload << std::endl;
}

int CVLADLINKPROC::chooseDestPort(int _dest_comp_id)
{
	cout << "DST:" << _dest_comp_id << std::endl;
	if(_dest_comp_id == 2){ //2 means Supervisor
		return 7777; //the Supervisor listens on port 7777 for CVLAD Link
	}
	return 0; //0 means we have no idea what machine is trying to speak to us! do not service request
}

