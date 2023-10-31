
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
	packet->acknum = -1; //���Ը��ֶ�
	packet->seqnum = expectSequenceNumberSend++;
	packet->checksum = 0;
	memcpy(packet->payload, message.data, sizeof(message.data));
	packet->checksum = pUtils->calculateCheckSum(*packet);
	pUtils->printPacket("���ͷ����ͱ���", *packet);
	pns->startTimer(SENDER, Configuration::TIME_OUT,packet->seqnum);			//�������ͷ���ʱ��
	packetAccepted[packet->seqnum] = false;
	numToPacket[packet->seqnum] = packet;
	pUtils->printPacket("Win push", *packet);
	packetWin.push(packet);
	pns->sendToNetworkLayer(RECEIVER, *packet);								//����ģ�����绷����sendToNetworkLayer��ͨ������㷢�͵��Է�																					//����ȴ�״̬
	return true;
}

void SRRdtSender::receive(const Packet &ackPkt) {
	if (!packetWin.empty()) {//������ͷ����ڵȴ�ack��״̬�������´�������ʲô������
		//���У����Ƿ���ȷ
		int checkSum = pUtils->calculateCheckSum(ackPkt);

		//���У�����ȷ
		if (checkSum == ackPkt.checksum) {
			packetAccepted[ackPkt.acknum] = true;
			pns->stopTimer(SENDER, ackPkt.acknum);//�رն�ʱ��
			while (!packetWin.empty() && packetAccepted[packetWin.front()->seqnum] ){
				pUtils->printPacket("Win pop", *packetWin.front());
				numToPacket.erase(packetWin.front()->acknum);
				packetAccepted.erase(packetWin.front()->acknum);
				delete packetWin.front();
				packetWin.pop();
			}
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

void SRRdtSender::timeoutHandler(int seqNum) {
	//Ψһһ����ʱ��,���迼��seqNum
	pns->stopTimer(SENDER,seqNum);										//���ȹرն�ʱ��
	pUtils->printPacket("���ͷ���ʱ��ʱ�䵽���ط�δȷ�ϱ���", *numToPacket[seqNum]);
	pns->sendToNetworkLayer(RECEIVER, *numToPacket[seqNum]);
	pns->startTimer(SENDER, Configuration::TIME_OUT,seqNum);			//�����������ͷ���ʱ��

	


}
