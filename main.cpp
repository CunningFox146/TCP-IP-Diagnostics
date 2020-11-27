#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#pragma warning(disable : 4996) // inet_addr устарело

//#define ADDR "8.8.8.8"  //IP - адрес

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus");

	HANDLE hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE)
	{
		printf("Ошибка IcmpCreatefile: %ld\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	unsigned long ipaddr = inet_addr(argv[1]); //преобразование IP-адреса
	char SendData[32] = "Data Buffer"; //данные эхо-запроса
	LPVOID ReplyBuffer = NULL; //данные эхо-ответа
	DWORD ReplySize = 0; //размер данных эхо-ответа
	
	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);

	//Выделяем память
	ReplyBuffer = (VOID*)malloc(ReplySize);
	if (ReplyBuffer == NULL)
	{
		printf("\tНевозможно выделить память\n");
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
		printf("Посылка icmp сообщения на %s\n",
			argv[1]);
		if (dwRetVal > 1) {
			printf("Получен %ldicmp ответ\n",
				dwRetVal);
			printf("Информация:\n");
		}
		else {
			printf("Получен %ld icmp ответ\n",
				dwRetVal);
			printf("Информация:\n");
		}
		printf("Получено от %s\n",
			inet_ntoa(ReplyAddr));
		printf("\tСтатус = %ld\n",
			pEchoReply->Status);
		printf("Время отклика  = %ld миллисекунд \n",
			pEchoReply->RoundTripTime);
	}
	else
	{
		printf("Вызов IcmpSendEcho завершился с ошибкой.\n");
		printf("IcmpSendEcho ошибка: %ld\n",
			WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	BOOL bRetVal; //освобождаем дескриптор
	bRetVal = IcmpCloseHandle(hIcmpFile);

	if (bRetVal)
		printf("Handle was closed\n");
	else
		printf("IcmpCloseHandle failed with error: %ld\n",
			WSAGetLastError());

	return EXIT_SUCCESS;
}