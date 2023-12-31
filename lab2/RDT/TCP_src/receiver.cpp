
#include "Global.h"
#include "TCPRdtReceiver.h"


TCPRdtReceiver::TCPRdtReceiver():expectSequenceNumberRcvd(1)
{
	lastAckPkt.acknum = -1; //初始状态下，上次发送的确认包的确认序号为-1，使得当第一个接受的数据包出错时该确认报文的确认号为-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//忽略该字段
	for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);	
}


TCPRdtReceiver::~TCPRdtReceiver()
{
}

void TCPRdtReceiver::receive(const Packet &packet) {
	//检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(packet);

	//如果校验和正确，同时收到报文的序号等于接收方期待收到的报文序号一致
	if (checkSum == packet.checksum ) {
		pUtils->printPacket("接收方正确收到发送方的报文", packet);
		
		//取出Message，向上递交给应用层
		
		if(!numToMessage.count(packet.seqnum)) {
			numToMessage[packet.seqnum] = new Message();
			memcpy(numToMessage[packet.seqnum]->data, packet.payload, sizeof(packet.payload));
		}
		

		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
		while(numToMessage.count(expectSequenceNumberRcvd)){
			pns->delivertoAppLayer(RECEIVER, *numToMessage[expectSequenceNumberRcvd]);
			numToMessage.erase(expectSequenceNumberRcvd);
			expectSequenceNumberRcvd ++;

		}
		lastAckPkt.acknum = expectSequenceNumberRcvd - 1; //确认序号等于收到的报文序号
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("接收方发送确认报文", lastAckPkt);		
	}
	else {
		pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
	}
}