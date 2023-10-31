#include "Global.h"
#include "TCPRdtSender.h"


TCPRdtSender::TCPRdtSender():expectSequenceNumberSend(1),winSize(8),nackCount(0)
{
}


TCPRdtSender::~TCPRdtSender()
{
}

bool TCPRdtSender::getWaitingState(){
	return packetWin.size() >= winSize;
}


bool TCPRdtSender::send(const Message &message) {
	if(packetWin.size() >= winSize) return false;
	Packet *packet = new Packet();
	packet->acknum = -1; //忽略该字段
	packet->seqnum = expectSequenceNumberSend++;
	packet->checksum = 0;
	memcpy(packet->payload, message.data, sizeof(message.data));
	packet->checksum = pUtils->calculateCheckSum(*packet);
	pUtils->printPacket("发送方发送报文", *packet);
	if(packetWin.empty()) pns->startTimer(SENDER, Configuration::TIME_OUT,packet->seqnum);			//启动发送方定时器
	pUtils->printPacket("Win push", *packet);
	packetWin.push(packet);
	pns->sendToNetworkLayer(RECEIVER, *packet);								//调用模拟网络环境的sendToNetworkLayer，通过网络层发送到对方																					//进入等待状态
	return true;
}

void TCPRdtSender::receive(const Packet &ackPkt) {
	if (!packetWin.empty()) {//如果发送方处于等待ack的状态，作如下处理；否则什么都不做
		//检查校验和是否正确
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//如果校验和正确
		if (checkSum == ackPkt.checksum && ackPkt.acknum >= packetWin.front()->seqnum) {
			nackCount = 0;
			pns->stopTimer(SENDER, packetWin.front()->seqnum);//关闭定时器
			while (!packetWin.empty() && ackPkt.acknum >= packetWin.front()->seqnum){
				pUtils->printPacket("Win pop", *packetWin.front());
				delete packetWin.front();
				packetWin.pop();
			}
			if(!packetWin.empty()) pns->startTimer(SENDER, Configuration::TIME_OUT,packetWin.front()->seqnum);//开启定时器
			pUtils->printPacket("发送方正确收到确认", ackPkt);
					
		}
		else {
			
			if(checkSum != ackPkt.checksum)
				pUtils->printPacket("发送方损坏", ackPkt);
			if(ackPkt.acknum < packetWin.front()->seqnum)
			{
				pUtils->printPacket("发送方acknum错误", ackPkt);	
				nackCount ++;
				if(nackCount == 3){
					printf("快速重传\n");
					nackCount = 0;
					pUtils->printPacket("发送方发送报文", *packetWin.front());
					pns->stopTimer(SENDER, packetWin.front()->seqnum);
					pns->startTimer(SENDER, Configuration::TIME_OUT,packetWin.front()->seqnum);			//启动发送方定时器
					pns->sendToNetworkLayer(RECEIVER, *packetWin.front());						
				}
			}		
			// do nothing

		}
	}	
}

void TCPRdtSender::timeoutHandler(int seqNum) {
	//唯一一个定时器,无需考虑seqNum
	pns->stopTimer(SENDER,seqNum);										//首先关闭定时器
	for(int num = 0; num < packetWin.size(); packetWin.pop()){
		pUtils->printPacket("发送方定时器时间到，重发未确认报文", *packetWin.front());
		pns->sendToNetworkLayer(RECEIVER, *packetWin.front());
		packetWin.push(packetWin.front());	
		num++;
	}
	pns->startTimer(SENDER, Configuration::TIME_OUT,seqNum);			//重新启动发送方定时器

	


}
