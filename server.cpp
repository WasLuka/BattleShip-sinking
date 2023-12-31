#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <time.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

int main()
{	
	const int m = 10;
	int igralnaPlosca1[m][m];
	int igralnaPlosca2[m][m];
	const int potopitve = 13;//glede na št. polj ki ji ladje zavzamejo

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < m; j++)
		{
			igralnaPlosca1[i][j] = 0;
		}
	}
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < m; j++)
		{
			igralnaPlosca2[i][j] = 0;
		}
	}


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
	int pripravljen = 0;//èe igralec napiše da je pripravljlen da vemo kdaj nadaljevati igro
	char buf1[256];
	char buf2[256];
	fd_set readSet;

	int velikost;
	int x;
	int y;
	int smer;
	int test;

	while (true)//prvi while loop za zajemanje ladjic
	{
		ZeroMemory(buf1, 256);
		ZeroMemory(buf2, 256);

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
			int bytesReceived1 = recv(clientSocket1, buf1, 256, 0);
			if (bytesReceived1 == 0)
			{
				std::cout << "Client 1 se je odjavil" << std::endl;
				return 0;
			}
			else
			{
																		//tukaj dobimo sporocilo od clienta 1
				std::cout << "client 1 je pisal" << std::endl;
				test = buf1[0];
				if (test == 100)
				{	
					std::cout << "client 1 pripravljen" << std::endl;
					pripravljen++;
					if (pripravljen == 2)
					{
						break;
					}
				}
				else
				{
					velikost = buf1[0];
					x = buf1[1];
					y = buf1[2];
					smer = buf1[3];
					std::cout << std::to_string(velikost) << ":,:" << std::to_string(x)
						<< ":,:" << std::to_string(y) << ":,:" << std::to_string(smer) << std::endl;

					switch (smer)
					{
						case 0://vodoravno
							for (int i = 0; i < velikost; i++)
							{
								igralnaPlosca1[x - i][y] = 1;
							}
							break;

						case 1://navpièno
							for (int i = 0; i < velikost; i++)
							{
								igralnaPlosca1[x][y - i] = 1;
							}
							break;
					}
				}

			}

		}

		if (FD_ISSET(clientSocket2, &readSet))
		{
			int bytesReceived2 = recv(clientSocket2, buf2, 256, 0);
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
				std::cout << "client 2 je pisal" << std::endl;
				test = buf2[0];
				if (test == 100)
				{	
					std::cout << "client 2 pripravljen" << std::endl;
					pripravljen++;
					if (pripravljen == 2)
					{
						break;
					}
				}
				else
				{
					velikost = buf2[0];
					x = buf2[1];
					y = buf2[2];
					smer = buf2[3];
					std::cout << std::to_string(velikost) << ":,:" << std::to_string(x)
						<< ":,:" << std::to_string(y) << ":,:" << std::to_string(smer) << std::endl;

					switch (smer)
					{
					case 0://vodoravno
						for (int i = 0; i < velikost; i++)
						{
							igralnaPlosca2[x - i][y] = 1;
						}
						break;

					case 1://navpièno
						for (int i = 0; i < velikost; i++)
						{
							igralnaPlosca2[x][y - i] = 1;
						}
						break;
					}
				}

			}
		}
	}


	std::cout << "napredujem naprej" << std::endl;

	int client1Potopitve = 0;
	int client2Potopitve = 0;
	char nazajClientu[4];

	ZeroMemory(nazajClientu, 4);
	send(clientSocket1, nazajClientu, 256, 0);
	nazajClientu[0] = 1;
	send(clientSocket2, nazajClientu, 256, 0);

	while (true)//drugi while loop za igro
	{
		ZeroMemory(buf1, 256);
		ZeroMemory(buf2, 256);
		ZeroMemory(nazajClientu, 4);

		
		int bytesReceived1 = recv(clientSocket1, buf1, 256, 0);
		if (bytesReceived1 == 0)
		{
			std::cout << "Client 1 se je odjavil" << std::endl;
			return 0;
		}
		else
		{
																	//tukaj dobimo sporocilo od clienta 1
			std::cout << "client 1 je pisal" << std::endl;
			x = buf1[0] - 48;
			y = buf1[1] - 65;

			if (igralnaPlosca2[x][y] == 1)//èe zadane
			{
				client1Potopitve++;
			}
			if (client1Potopitve == potopitve)//èe potopi vse ladjice
			{
				ZeroMemory(nazajClientu, 4);
				nazajClientu[1] = 2;
				send(clientSocket1, nazajClientu, 256, 0);
				ZeroMemory(nazajClientu, 4);
				nazajClientu[1] = 3;
				send(clientSocket2, nazajClientu, 256, 0);

			}
			nazajClientu[0] = igralnaPlosca2[x][y] + 1;
			nazajClientu[1] = 1;
			nazajClientu[2] = x;
			nazajClientu[3] = y;
			send(clientSocket1, nazajClientu, 256, 0);
			nazajClientu[0] = 3;
			send(clientSocket2, nazajClientu, 256, 0);
		}


		
		int bytesReceived2 = recv(clientSocket2, buf2, 256, 0);
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
			std::cout << "client 2 je pisal" << std::endl;
			x = buf2[0] - 48;
			y = buf2[1] - 65;

			if (igralnaPlosca1[x][y] == 1)
			{
				client2Potopitve++;
			}
			if (client2Potopitve == potopitve)
			{
				ZeroMemory(nazajClientu, 4);
				nazajClientu[1] = 2;
				send(clientSocket2, nazajClientu, 256, 0);
				ZeroMemory(nazajClientu, 4);
				nazajClientu[1] = 3;
				send(clientSocket1, nazajClientu, 256, 0);

			}
			nazajClientu[0] = igralnaPlosca2[x][y] + 1;
			nazajClientu[1] = 1;
			nazajClientu[2] = x;
			nazajClientu[3] = y;
			send(clientSocket2, nazajClientu, 256, 0);
			nazajClientu[0] = 3;
			send(clientSocket1, nazajClientu, 256, 0);
		}
		


		if (client1Potopitve == potopitve || client2Potopitve == potopitve)
		{
			//pošlji zmagovalcu "win"
			//pošlji poražencu "los"
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
