#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"
#include <queue>
#include <map>
class SRRdtSender :public RdtSender
{
private:
	std::map<int,Packet* >numToPacket;
	std::map<int,bool> packetAccepted;  
	std::queue<Packet* > packetWin;        // �����е�packet
	int expectSequenceNumberSend;	// ��һ���������
	int winSize;                    // ���ڴ�С  

public:
	bool getWaitingState();
	bool send(const Message &message);						//����Ӧ�ò�������Message����NetworkServiceSimulator����,������ͷ��ɹ��ؽ�Message���͵�����㣬����true;�����Ϊ���ͷ����ڵȴ���ȷȷ��״̬���ܾ�����Message���򷵻�false
	void receive(const Packet &ackPkt);						//����ȷ��Ack������NetworkServiceSimulator����	
	void timeoutHandler(int seqNum);					//Timeout handler������NetworkServiceSimulator����

public:
	SRRdtSender();
	virtual ~SRRdtSender();
};

#endif