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
	packet->acknum = -1; //���Ը��ֶ�
	packet->seqnum = expectSequenceNumberSend++;
	packet->checksum = 0;
	memcpy(packet->payload, message.data, sizeof(message.data));
	packet->checksum = pUtils->calculateCheckSum(*packet);
	pUtils->printPacket("���ͷ����ͱ���", *packet);
	if(packetWin.empty()) pns->startTimer(SENDER, Configuration::TIME_OUT,packet->seqnum);			//�������ͷ���ʱ��
	pUtils->printPacket("Win push", *packet);
	packetWin.push(packet);
	pns->sendToNetworkLayer(RECEIVER, *packet);								//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�																					//����ȴ�״̬
	return true;
}

void TCPRdtSender::receive(const Packet &ackPkt) {
	if (!packetWin.empty()) {//������ͷ����ڵȴ�ack��״̬�������´�������ʲô������
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//���У�����ȷ
		if (checkSum == ackPkt.checksum && ackPkt.acknum >= packetWin.front()->seqnum) {
			nackCount = 0;
			pns->stopTimer(SENDER, packetWin.front()->seqnum);//�رն�ʱ��
			while (!packetWin.empty() && ackPkt.acknum >= packetWin.front()->seqnum){
				pUtils->printPacket("Win pop", *packetWin.front());
				delete packetWin.front();
				packetWin.pop();
			}
			if(!packetWin.empty()) pns->startTimer(SENDER, Configuration::TIME_OUT,packetWin.front()->seqnum);//������ʱ��
			pUtils->printPacket("���ͷ���ȷ�յ�ȷ��", ackPkt);
					
		}
		else {
			
			if(checkSum != ackPkt.checksum)
				pUtils->printPacket("���ͷ���", ackPkt);
			if(ackPkt.acknum < packetWin.front()->seqnum)
			{
				pUtils->printPacket("���ͷ�acknum����", ackPkt);	
				nackCount ++;
				if(nackCount == 3){
					printf("�����ش�\n");
					nackCount = 0;
					pUtils->printPacket("���ͷ����ͱ���", *packetWin.front());
					pns->stopTimer(SENDER, packetWin.front()->seqnum);
					pns->startTimer(SENDER, Configuration::TIME_OUT,packetWin.front()->seqnum);			//�������ͷ���ʱ��
					pns->sendToNetworkLayer(RECEIVER, *packetWin.front());						
				}
			}		
			// do nothing

		}
	}	
}

void TCPRdtSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	pns->stopTimer(SENDER,seqNum);										//���ȹرն�ʱ��
	for(int num = 0; num < packetWin.size(); packetWin.pop()){
		pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط�δȷ�ϱ���", *packetWin.front());
		pns->sendToNetworkLayer(RECEIVER, *packetWin.front());
		packetWin.push(packetWin.front());	
		num++;
	}
	pns->startTimer(SENDER, Configuration::TIME_OUT,seqNum);			//�����������ͷ���ʱ��

	


}
