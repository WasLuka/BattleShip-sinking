#include <iostream>
#include <time.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

int main()
{
	//vspostavi WinSock
	WSAData wsData;
	WORD ver MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		std::cerr << "ne morem vspostaviti WinSock-a #" << WSAGetLastError() << std::endl;
	}

	//ustvari socket za poslušanje
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "ne morem ustvariti socket-a #" << WSAGetLastError() << std::endl;
	}

	//poveži ip naslov in port na socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(27000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//povej WinSock-u da je socket za poslušanje
	listen(listening, SOMAXCONN);

	//poèakaj na povezavo za prvi client
	sockaddr_in client1;
	int clientSize1 = sizeof(client1);

	SOCKET clientSocket1 = accept(listening, (sockaddr*)&client1, &clientSize1);

	char host1[NI_MAXHOST];
	char service1[NI_MAXSERV];

	ZeroMemory(host1, NI_MAXHOST);
	ZeroMemory(service1, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client1, sizeof(client1), host1, NI_MAXHOST, service1, NI_MAXSERV, 0) == 0)
	{
		std::cout << host1 << "1 povezan na port-u " << service1 << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client1.sin_addr, host1, NI_MAXHOST);
		std::cout << host1 << "1 povezan na port-u " <<
			ntohs(client1.sin_port) << std::endl;
	}
	//do tukaj

	//poèakaj na povezavo za drugi client
	sockaddr_in client2;
	int clientSize2 = sizeof(client2);

	SOCKET clientSocket2 = accept(listening, (sockaddr*)&client2, &clientSize2);

	char host2[NI_MAXHOST];
	char service2[NI_MAXSERV];

	ZeroMemory(host2, NI_MAXHOST);
	ZeroMemory(service2, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client2, sizeof(client2), host2, NI_MAXHOST, service2, NI_MAXSERV, 0) == 0)
	{
		std::cout << host2 << "2 Povezan na port-u " << service2 << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client2.sin_addr, host2, NI_MAXHOST);
		std::cout << host2 << "2 Povezan na port-u " << service2 <<
			ntohs(client2.sin_port) << std::endl;
	}
	//do tukaj
	
	//zapri port za poslušanje
	closesocket(listening);

	//tukaj mora se biti while loop za nastavitev ladjic

	//while loop kjer se igra
	char buf1[4096];
	char buf2[4096];
	fd_set readSet;

	while (true)//to ostane dosti isto samo ne piše veè nazaj
	{
		ZeroMemory(buf1, 4096);
		ZeroMemory(buf2, 4096);

		FD_ZERO(&readSet);
		FD_SET(clientSocket1, &readSet);
		FD_SET(clientSocket2, &readSet);

		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 10000;

		int activity = select(0, &readSet, nullptr, nullptr, &timeout);

		if (activity == SOCKET_ERROR)
		{
			std::cerr << "napaka v select()" << std::endl;
			return 1;
		}

		if (FD_ISSET(clientSocket1, &readSet))
		{
			int bytesReceived1 = recv(clientSocket1, buf1, 4096, 0);
			if (bytesReceived1 == 0)
			{
				std::cout << "Client 1 se je odjavil" << std::endl;
				return 0;
			}
			else
			{
																		//tukaj dobimo sporocilo od clienta 1
				std::cout << "client 1 je pisal" << std::endl;
			}

		}

		if (FD_ISSET(clientSocket2, &readSet))
		{
			int bytesReceived2 = recv(clientSocket2, buf2, 4096, 0);
			if (bytesReceived2 == SOCKET_ERROR)
			{
				std::cerr << "napaka v recv()" << std::endl;
				return 1;
			}

			if (bytesReceived2 == 0)
			{
				std::cout << "client 2 se je odjavil" << std::endl;
				return 0;
			}
			else
			{
																		//tukaj dobimo sporocilo od clienta 2
				std::cout << "client 2 pisal" << std::endl;
			}
		}





	}
	//zapri socket-a
	closesocket(clientSocket1);
	closesocket(clientSocket2);

	//poèisti winsock
	WSACleanup();

	system("pause");
	return 0;
}