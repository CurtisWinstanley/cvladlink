#include <cassert>
#include <string>

#include "cvladlink.hpp"

using std::cout;
using std::endl;

extern "C" {
#include "socket.h"
#include "winsock2.h"
}

STATUS requestStatus = IN_PROGRESS; //default status of a message

void callback(STATUS s) 
{
    cout << requestStatus << std::endl;
    requestStatus = s;
}


int main()
{
    std::string msg = "hello world"; //test input data/message
    std::string myHost = "127.0.0.1";
    char* buff = new char[50];
    buff = (char*)msg.c_str(); //copy the contents from mbs into the buffer

    

    packet_t* pack = new packet_t();
    //pack->socket = sock;
    pack->data = buff; // set the packet data = to the buffer NOTE: must typecast (char*)pack->data if the actual contents is desired
    pack->size = strlen((char*)pack->data); // size of the packet data


    //destSysID 1, destCompID2, messageID, 1000ms timout, size of the packet, payload, and the callback function to be executed when the request is serviced
    sendreliable(1, 2, 50, 1000, pack->size, (char*)pack->data, callback);
    //int sendpacket = send(pack->socket->s, (char*)pack->data, pack->size, 0);

    //cout << "sent " << (char*)pack->data << " successfully if " << pack->size << "=" << sendpacket <<std::endl;
    while(true)
    {
        cout << "sleep for 1000ms" << std::endl;
        Sleep(1000);
    }
    printf("after connection\n");

}
