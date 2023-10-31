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
	std::queue<Packet* > packetWin;        // 窗口中的packet
	int expectSequenceNumberSend;	// 下一个发送序号
	int winSize;                    // 窗口大小  

public:
	bool getWaitingState();
	bool send(const Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
	void receive(const Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
	void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用

public:
	SRRdtSender();
	virtual ~SRRdtSender();
};

#endif