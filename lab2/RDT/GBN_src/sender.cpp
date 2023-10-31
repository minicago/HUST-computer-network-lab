
#include "Global.h"
#include "GBNRdtSender.h"


GBNRdtSender::GBNRdtSender():expectSequenceNumberSend(1),winSize(4)
{
}


GBNRdtSender::~GBNRdtSender()
{
}

bool GBNRdtSender::getWaitingState(){
	return packetWin.size() >= winSize;
}


bool GBNRdtSender::send(const Message &message) {
	if(packetWin.size() >= winSize) return false;
	Packet *packet = new Packet();
	packet->acknum = -1; //���Ը��ֶ�
	packet->seqnum = expectSequenceNumberSend++;
	packet->checksum = 0;
	memcpy(packet->payload, message.data, sizeof(message.data));
	packet->checksum = pUtils->calculateCheckSum(*packet);
	pUtils->printPacket("���ͷ����ͱ���", *packet);
	if(packetWin.empty()) pns->startTimer(SENDER, Configuration::TIME_OUT,packet->seqnum);			//�������ͷ���ʱ��
	packetWin.push(packet);
	pns->sendToNetworkLayer(RECEIVER, *packet);								//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�																					//����ȴ�״̬
	return true;
}

void GBNRdtSender::receive(const Packet &ackPkt) {
	if (!packetWin.empty()) {//������ͷ����ڵȴ�ack��״̬�������´�������ʲô������
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//���У�����ȷ
		if (checkSum == ackPkt.checksum && ackPkt.acknum >= packetWin.front()->seqnum) {
			pns->stopTimer(SENDER, packetWin.front()->seqnum);//�رն�ʱ��
			while (!packetWin.empty() && ackPkt.acknum >= packetWin.front()->seqnum){
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
				pUtils->printPacket("���ͷ�acknum����", ackPkt);			
			// do nothing

		}
	}	
}

void GBNRdtSender::timeoutHandler(int seqNum) {
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
