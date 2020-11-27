#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable : 4996) // inet_addr ��������

//#define ADDR "8.8.8.8"  //IP - �����

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus");

	HANDLE hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE)
	{
		printf("������ IcmpCreatefile: %ld\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	unsigned long ipaddr = inet_addr(argv[1]); //�������������� IP-������
	char SendData[32] = "Data Buffer"; //������ ���-�������
	LPVOID ReplyBuffer = NULL; //������ ���-������
	DWORD ReplySize = 0; //������ ������ ���-������
	
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);

	//�������� ������
	ReplyBuffer = (VOID*)malloc(ReplySize);
	if (ReplyBuffer == NULL)
	{
		printf("\t���������� �������� ������\n");
		exit(EXIT_FAILURE);
	}

	DWORD dwRetVal =
		IcmpSendEcho(hIcmpFile, ipaddr, SendData,
			sizeof(SendData), NULL,
			ReplyBuffer, ReplySize, 1000);

	if (dwRetVal != 0)
	{
		PICMP_ECHO_REPLY pEchoReply =
			(PICMP_ECHO_REPLY)ReplyBuffer;
		struct in_addr ReplyAddr;
		ReplyAddr.S_un.S_addr = pEchoReply->Address;
		printf("������� icmp ��������� �� %s\n",
			argv[1]);
		if (dwRetVal > 1) {
			printf("������� %ldicmp �����\n",
				dwRetVal);
			printf("����������:\n");
		}
		else {
			printf("������� %ld icmp �����\n",
				dwRetVal);
			printf("����������:\n");
		}
		printf("�������� �� %s\n",
			inet_ntoa(ReplyAddr));
		printf("\t������ = %ld\n",
			pEchoReply->Status);
		printf("����� �������  = %ld ����������� \n",
			pEchoReply->RoundTripTime);
	}
	else
	{
		printf("����� IcmpSendEcho ���������� � �������.\n");
		printf("IcmpSendEcho ������: %ld\n",
			WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	BOOL bRetVal; //����������� ����������
	bRetVal = IcmpCloseHandle(hIcmpFile);

	if (bRetVal)
		printf("Handle was closed\n");
	else
		printf("IcmpCloseHandle failed with error: %ld\n",
			WSAGetLastError());

	return EXIT_SUCCESS;
}