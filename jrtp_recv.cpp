/* *************************************************************************
 * In this file, JRTPLIB, an RTP/RTCP protocol stack, is used to receive   *	
 * Rtp data from the Internet.						   *
 * 									   *
 * Author: Lor(a.k.a ButcherCat);					   *
 * Date: 2014-06-17;							   *
 * E-mail: wt_lor@163.com (Maybe ButcherCat@lortech.com in near future)    *
 * *************************************************************************/

#include <rtpsession.h>
#include <rtppacket.h>
#include <rtpudpv4transmitter.h>
#include <rtpipv4address.h>
#include <rtpsessionparams.h>
#include <rtperrors.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <rtpsourcedata.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

#define PATH_TO_SAVE "./target.h264"

using namespace jrtplib;

void checkerror(int rtperr)
{
	if(rtperr < 0){
		std::cout << "Error:" << RTPGetErrorString(rtperr) << std::endl;
		exit(-1);
	}
}

class OwnRtpSession : public RTPSession {
protected:
	void OnNewSource(RTPSourceData *srcdat){
		if(srcdat->IsOwnSSRC())	
			return;
		uint32_t ip;
		uint16_t port;

		if(srcdat->GetRTPDataAddress() != 0){
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(srcdat->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else if(srcdat->GetRTCPDataAddress() != 0){
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(srcdat->GetRTCPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else
			return;

		RTPIPv4Address dest(ip,port);
		AddDestination(dest);

		struct in_addr inaddr;
		inaddr.s_addr = htonl(ip);
		std::cout << "Adding Destination " << std::string(inet_ntoa(inaddr)) << ":" << port << std::endl;
	}

	void OnBYEPacket(RTPSourceData *dat){
		if(dat->IsOwnSSRC())
			return;
		uint32_t ip;
		uint16_t port;

		if(dat->GetRTPDataAddress() != 0){
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else if(dat->GetRTCPDataAddress() != 0){
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else 
			return;

		RTPIPv4Address dest(ip,port);
		DeleteDestination(dest);

		struct in_addr inaddr;
		inaddr.s_addr = htonl(ip);
		std::cout << "Deleting destination " << std::string(inet_ntoa(inaddr)) << ":" << port << std::endl;
	}

	void OnRemoveSource(RTPSourceData *dat){
		if(dat->IsOwnSSRC())
			return;
		if(dat->ReceivedBYE())
			return;
		uint32_t ip;
		uint16_t port;

		if(dat->GetRTPDataAddress() != 0){
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else if(dat->GetRTCPDataAddress() != 0){
			const RTPIPv4Address *addr = (const RTPIPv4Address *)(dat->GetRTCPDataAddress());
			ip = addr->GetIP();
			port = addr->GetPort();
		}
		else 
			return;

		RTPIPv4Address dest(ip,port);
		DeleteDestination(dest);

		struct in_addr inaddr;
		inaddr.s_addr = htonl(ip);
		std::cout << "Deleting destination " << std::string(inet_ntoa(inaddr)) << ":" << std::endl;
	}
};


int main()
{
	FILE *fp = NULL;
	fp = fopen(PATH_TO_SAVE,"a+");
	OwnRtpSession sess;
	uint16_t portbase;
	std::string ipstr;
	int status,i,num;
	if(fp == NULL){
		printf("Error: failed to open target file\n");
		exit(1);
	}

	// Frist, we'll ask for the necessary information
	std::cout << "Enter local portbase:" << std::endl;
	std::cin >> portbase;
	std::cout << std::endl;

	std::cout << "Number of seconds you wish to wait:" << std::endl;
	std::cin >> num;
	std::cout << std::endl;

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;

	sessparams.SetOwnTimestampUnit(1.0/90000.0);

	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams,&transparams);

	checkerror(status);
	for(i = 1; i < num ; i++){
		sess.BeginDataAccess();

		if(sess.GotoFirstSourceWithData()){
			do {
				RTPPacket *packet;
				while((packet = sess.GetNextPacket()) != NULL){
				// Process the incoming data
					fwrite(packet->GetPayloadData(),packet->GetPayloadLength(),1,fp);
				}
			} while(sess.GotoNextSourceWithData());
		}

		sess.EndDataAccess();

#ifndef RTP_SUPPORT_THREAD
		status = sess.Poll();
		checkerror(status);
#endif	//RTP_SUPPORT_THREAD
		RTPTime::Wait(RTPTime(1,0));
	}

	sess.BYEDestroy(RTPTime(10,0),0,0);
	fclose(fp);
}
