
#include "Global.h"
#include "SRRdtReceiver.h"


SRRdtReceiver::SRRdtReceiver():expectSequenceNumberRcvd(1)
{
	lastAckPkt.acknum = -1; //��ʼ״̬�£��ϴη��͵�ȷ�ϰ���ȷ�����Ϊ-1��ʹ�õ���һ�����ܵ����ݰ�����ʱ��ȷ�ϱ��ĵ�ȷ�Ϻ�Ϊ-1
	lastAckPkt.checksum = 0;
	lastAckPkt.seqnum = -1;	//���Ը��ֶ�
	for(int i = 0; i < Configuration::PAYLOAD_SIZE;i++){
		lastAckPkt.payload[i] = '.';
	}
	lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);	
}


SRRdtReceiver::~SRRdtReceiver()
{
}

void SRRdtReceiver::receive(const Packet &packet) {
	//���У����Ƿ���ȷ
	int checkSum = pUtils->calculateCheckSum(packet);

	//���У�����ȷ��ͬʱ�յ����ĵ���ŵ��ڽ��շ��ڴ��յ��ı������һ��
	if (checkSum == packet.checksum ) {
		pUtils->printPacket("���շ���ȷ�յ����ͷ��ı���", packet);
		
		//ȡ��Message�����ϵݽ���Ӧ�ò�
		
		if(!numToMessage.count(packet.seqnum)) {
			numToMessage[packet.seqnum] = new Message();
			memcpy(numToMessage[packet.seqnum]->data, packet.payload, sizeof(packet.payload));
		}
		
		lastAckPkt.acknum = packet.seqnum; //ȷ����ŵ����յ��ı������
		lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
		pUtils->printPacket("���շ�����ȷ�ϱ���", lastAckPkt);
		pns->sendToNetworkLayer(SENDER, lastAckPkt);	//����ģ�����绷����sendToNetworkLayer��ͨ������㷢��ȷ�ϱ��ĵ��Է�
		while(numToMessage.count(expectSequenceNumberRcvd)){
			pns->delivertoAppLayer(RECEIVER, *numToMessage[expectSequenceNumberRcvd]);
			numToMessage.erase(expectSequenceNumberRcvd);
			expectSequenceNumberRcvd ++;
		}
		
	}
	else {
		pUtils->printPacket("���շ�û����ȷ�յ����ͷ��ı���,����У�����", packet);
	}
}