#ifndef TCP_RDT_RECEIVER_H
#define TCP_RDT_RECEIVER_H
#include<map>
#include "RdtReceiver.h"
class TCPRdtReceiver :public RdtReceiver
{
private:
	map<int,Message*> numToMessage;
	int expectSequenceNumberRcvd;	// �ڴ��յ�����һ���������
	Packet lastAckPkt;				//�ϴη��͵�ȷ�ϱ���

public:
	TCPRdtReceiver();
	virtual ~TCPRdtReceiver();

public:
	
	void receive(const Packet &packet);	//���ձ��ģ�����NetworkService����
};

#endif
