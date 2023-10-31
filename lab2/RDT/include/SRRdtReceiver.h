#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
#include<map>
class SRRdtReceiver :public RdtReceiver
{
private:
	int expectSequenceNumberRcvd;
	std::map<int,Message* >numToMessage;
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	SRRdtReceiver();
	virtual ~SRRdtReceiver();

public:
	
	void receive(const Packet &packet);	//���ձ��ģ�����NetworkService����
};

#endif
