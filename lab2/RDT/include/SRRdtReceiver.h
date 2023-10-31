#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
#include<map>
class SRRdtReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;
	std::map<int,Message* >numToMessage;
	Packet lastAckPkt;				//上次发送的确认报文

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:
	
	void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};

#endif
