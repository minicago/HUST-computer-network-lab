
#include "Global.h"
#include "SRRdtSender.h"


SRRdtSender::SRRdtSender():expectSequenceNumberSend(1),winSize(4)
{
}


SRRdtSender::~SRRdtSender()
{
}

bool SRRdtSender::getWaitingState(){
	return packetWin.size() >= winSize;
}


bool SRRdtSender::send(const Message &message) {
	if(packetWin.size() >= winSize) return false;
	Packet *packet = new Packet();
	packet->acknum = -1; //忽略该字段
	packet->seqnum = expectSequenceNumberSend++;
	packet->checksum = 0;
	memcpy(packet->payload, message.data, sizeof(message.data));
	packet->checksum = pUtils->calculateCheckSum(*packet);
	pUtils->printPacket("发送方发送报文", *packet);
	pns->startTimer(SENDER, Configuration::TIME_OUT,packet->seqnum);			//启动发送方定时器
	packetAccepted[packet->seqnum] = false;
	numToPacket[packet->seqnum] = packet;
	pUtils->printPacket("Win push", *packet);
	packetWin.push(packet);
	pns->sendToNetworkLayer(RECEIVER, *packet);								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方																					//进入等待状态
	return true;
}

void SRRdtSender::receive(const Packet &ackPkt) {
	if (!packetWin.empty()) {//如果发送方处于等待ack的状态，作如下处理；否则什么都不做
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//如果校验和正确
		if (checkSum == ackPkt.checksum) {
			packetAccepted[ackPkt.acknum] = true;
			pns->stopTimer(SENDER, ackPkt.acknum);//关闭定时器
			while (!packetWin.empty() && packetAccepted[packetWin.front()->seqnum] ){
				pUtils->printPacket("Win pop", *packetWin.front());
				numToPacket.erase(packetWin.front()->acknum);
				packetAccepted.erase(packetWin.front()->acknum);
				delete packetWin.front();
				packetWin.pop();
			}
			pUtils->printPacket("发送方正确收到确认", ackPkt);
					
		}
		else {
			if(checkSum != ackPkt.checksum)
				pUtils->printPacket("发送方损坏", ackPkt);
			if(ackPkt.acknum < packetWin.front()->seqnum)
				pUtils->printPacket("发送方acknum错误", ackPkt);			
			// do nothing

		}
	}	
}

void SRRdtSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	pns->stopTimer(SENDER,seqNum);										//首先关闭定时器
	pUtils->printPacket("发送方定时器时间到，重发未确认报文", *numToPacket[seqNum]);
	pns->sendToNetworkLayer(RECEIVER, *numToPacket[seqNum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT,seqNum);			//重新启动发送方定时器

	


}
