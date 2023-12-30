#include <windows.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

void igralnoPolje(int m, int razmakx, int razmaky)
{
    int ax;    
    int ay;
    const std::string zacetek = "\x1b[";
    const char kvadrat = 254;
    const char abeceda[28] = "0ABCDEFGHIJKLMNOPQRSTUVWKYZ";

    std::string izhod;

    //nariši polje
    std::cout << "\x1b[38;2;100;100;100m";//nastavi barvo na rgb
    for (int x = 1; x < m + 1; x++)
    {
        for (int y = 1; y < m + 1; y++)
        {
            ax = 2 * x + razmakx;
            ay = y + razmaky;

            izhod = zacetek + std::to_string(ay) + ';' + std::to_string(ax) + 'H' + kvadrat;//prikaži kvadrat na ax ay koordinati
            std::cout << izhod;
            izhod = "";

        }
    }

    //risanje y stolpca
    izhod = "";
    std::cout << "\x1b[38;2;240;100;100m";//nastavi barvo na rgb --rdeèkasta
    ax = razmakx - 1;//x koordinata je konstantna saj pišemo samo y

    for (int y = 1; y < m + 1; y++)
    {
        ay = y + razmaky;

        izhod = zacetek + std::to_string(ay) + ';' + std::to_string(ax) + 'H' + abeceda[m - y + 1];
        std::cout << izhod;
        izhod = "";
    }

    //risanje x stolpca
    izhod = "";
    std::cout << "\x1b[38;2;100;100;240m";//nastavi na barvo rgb --modrikasta
    ay = razmaky + 1 + m;//y kooridnata je konstantna saj pišemo samo x

    for (int x = 1; x < m + 1; x++)
    {
        ax = 2 * x + razmakx;

        izhod = zacetek + std::to_string(ay) + ';' + std::to_string(ax) + 'H' + std::to_string(x - 1);
        std::cout << izhod;
        izhod = "";
    }

    //ponastavljanje barve 
    std::cout << "\x1b[0m";//ponastavi barve
}


void ladja(int velikost, int x, int y, int smer)
{
    const char kvadrat = 254;
    const std::string zacetek = "\x1b[";
    std::string izhod;

    std::cout << "\x1b[38;2;200;30;170m";//nastavi na barvo rgb --vijolièna

    if (smer == 1)//navpicno
    {
        for (int i = 0; i < velikost; i++)
        {
            izhod = zacetek + std::to_string(y + i) + ';' + std::to_string(x) + 'H' + kvadrat;
            std::cout << izhod;
        }
    }
    if (smer == 0)//vodoravno
    {
        for (int i = 0; i < velikost; i++)
        {
            izhod = zacetek + std::to_string(y) + ';' + std::to_string(x - 2 * i) + 'H' + kvadrat;
            std::cout << izhod;
        }
    }

    std::cout << "\x1b[0m";//ponastavi barve
}


bool vklopiVTS()//Virtual Terminal Sequences
{
    //nastavi izhod da uporablja virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
    }
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
    }
    return true;
}


int main()
{
    vklopiVTS();
    std::cout << "\x1b[?12l";//izklopi utripanje miške
    std::cout << "\x1b[?25l";//izklopi kazalec miške

    const int m = 10;//velikost igralnega polja
    int razmaky = 4;//razmak od gornjega roba
    int razmakx = 2 * razmaky;//razmak od stranskega roba
    int stLadjic = 4;//koliko ladjic hocemo imeti
    int x;//koordinata x
    int y;//koordinata y
    int ax;
    int ay;
    const char abeceda[27] = "ABCDEFGHIJKLMNOPQRSTUVWKYZ";
    char crka;//koordinata y
    char stevilka;//koordinata x
    char smer;//kako glelda ladjica vodoravno-navpièno
    char velikost;//koliko kosov je dolga ladjica
    const std::string zacetek = "\x1b[";
    std::string izhod = "";

    int igralnaPlosca[m][m];
    int ladjice[6] = {0,0,1,2,0,1};
    //ladjice[i] i = velikost, vrednost tega je koliko teh ladjic imaš še lahko

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            igralnaPlosca[i][j] = 0;
        }
    }

    std::string ipAddress = "192.168.8.39";
    int port = 27000;

    //vklopi WinSock
    WSAData data;
    WORD ver = MAKEWORD(2, 2);
    int wsResult = WSAStartup(ver, &data);
    if (wsResult != 0)
    {
        std::cerr << "ne morem zagnati WinSocka #" << WSAGetLastError() << std::endl;
        WSACleanup;
        return 1;
    }

    //ustvari socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        std::cerr << "ne morem ustvariti socket-a #" << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    //izpolni hint strukturo
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    //Poveži se na server
    int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connResult == SOCKET_ERROR)
    {
        std::cerr << "ne morem se povezati na server #" << WSAGetLastError() << std::endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    igralnoPolje(m, razmakx, razmaky);


    //pisanje zahtevanih ladij
    izhod = "";
    izhod = zacetek + std::to_string(razmaky + 1) + ';' + std::to_string(razmakx + 2*m + 4) + 'H' + "1x2d";
    std::cout << izhod;
    izhod = zacetek + std::to_string(razmaky + 2) + ';' + std::to_string(razmakx + 2 * m + 4) + 'H' + "2x3d";
    std::cout << izhod;
    izhod = zacetek + std::to_string(razmaky + 3) + ';' + std::to_string(razmakx + 2 * m + 4) + 'H' + "1x5d";
    std::cout << izhod;
    izhod = "";


                                            //risanje ladjic
    while (stLadjic > 0)
    {   
        zacetek:;
        izhod = "";//postavimo kazalec na pravo mesto za vpisovanje naših ukazov
        izhod = zacetek + std::to_string(m + razmaky + 4) + ';' + std::to_string(razmakx) + 'H';
        std::cout << izhod;



    velikost:;//zajemanje velikosti za ladjo
        while (!_kbhit)
        {
        }
        velikost = _getch();
        if (velikost > 48 && velikost < 58)//mora biti ascii številka
        {   
            if (ladjice[velikost - 48] > 0)
            {
                ladjice[velikost - 48]--;
                std::cout << velikost;
            }
            else
            {
                goto velikost;
            }
        }
        else
        {
            goto velikost;
        }


    stevilka:;//zajemanje številke za lokacijo
        while (!_kbhit)
        {
        }
        stevilka = _getch();
        if (stevilka > 47 && stevilka < 58)//mora biti ascii številka
        {
            std::cout << stevilka;
        }
        else if (stevilka == 8)
        {   
            std::cout << "\x1b[1D";
            std::cout << " ";
            std::cout << "\x1b[1D";
            ladjice[velikost - 48]++;
            goto velikost;
        }
        else
        {
            goto stevilka;
        }


    crka:;//zajemanje èrke za lokacijo              
        while (!_kbhit())
        {
        }
        crka = _getch();
        if (crka < 75 && crka > 64)//mora biti ascii èrka
        {
            std::cout << crka;
        }
        else if ( crka == 8)
        {   
            std::cout << "\x1b[1D";
            std::cout << " ";
            std::cout << "\x1b[1D";
            goto stevilka;
        }
        else
        {
            goto crka;
        }


    smer:;//zajemanje smer ladje 1 navpicno 0 vodoravno
        while (!_kbhit())
        {
        }
        smer = _getch();
        if (smer > 47 && smer < 50)//mora biti ascii številka
        {
            std::cout << smer;
        }
        else if (smer == 8)
        {   
            std::cout << "\x1b[1D";
            std::cout << " ";
            std::cout << "\x1b[1D";
            goto crka;
        }
        else
        {
            goto smer;
        }




        //racunanje koncnih vrednosti tako da se znebimo njihovih "ascii vrednosti"
        velikost = velikost - 48;
        x = 2 * (stevilka - 47) + razmakx;
        y = m + 1 + razmaky - crka + 64;
        smer = smer - 48;

        //primerja z vhodnimi vrednostmi saj so x,y ze procesirane in nastavljene za pisanje v terminal
        switch (smer)//preveri, ce je mozno postaviti ladjico v polje
        {
            case 0://vodoarvno
                if ((stevilka - 47) < velikost)
                {
                    izhod = "";//zbriši vse napisano in zaèni znova
                    izhod = zacetek + std::to_string(m + razmaky + 4) + ';' + std::to_string(razmakx) + 'H';
                    std::cout << izhod;
                    std::cout << "    ";
                    ladjice[velikost - 48]++;
                    goto zacetek;
                }
                break;
            case 1://navpicno
                if ((crka-64) < velikost)
                {
                    izhod = "";//zbriši vse napisano in zaèni znova
                    izhod = zacetek + std::to_string(m + razmaky + 4) + ';' + std::to_string(razmakx) + 'H';
                    std::cout << izhod;
                    std::cout << "    ";
                    ladjice[velikost]++;
                    goto zacetek;
                }
                break;
        }

        ax = stevilka - 48;//dodatne koordinate za sam array in za server
        ay = crka - 65;

        if (ax < 0 || ay < 0)
        {   
            ladjice[velikost]++;
            goto zacetek;
        }

        switch (smer)
        {
            case 0://vodoravno
                for (int i = 0; i < velikost; i++)
                {
                    if (igralnaPlosca[ax - i][ay] == 1)
                    {   
                        izhod = "";//zbriši vse napisano in nadaljuj z naslednjo ladjico
                        izhod = zacetek + std::to_string(m + razmaky + 4) + ';' + std::to_string(razmakx) + 'H';
                        std::cout << izhod;
                        std::cout << "    ";
                        ladjice[velikost]++;
                        goto zacetek;
                    }
                }
                //èe preživi jo shranimo
                for (int i = 0; i < velikost; i++)
                {
                    igralnaPlosca[ax - i][ay] = 1;
                }
                break;


            case 1://navpicno
                for (int i = 0; i < velikost; i++)
                {
                    if (igralnaPlosca[ax][ay - i] == 1)
                    {   
                        izhod = "";//zbriši vse napisano in nadaljuj z naslednjo ladjico
                        izhod = zacetek + std::to_string(m + razmaky + 4) + ';' + std::to_string(razmakx) + 'H';
                        std::cout << izhod;
                        std::cout << "    ";
                        ladjice[velikost]++;
                        goto zacetek;
                    }
                }
                //èe preživi jo shranimo
                for (int i = 0; i < velikost; i++)
                {
                    igralnaPlosca[ax][ay - i] = 1;
                }
                break;
        }


        ladja(velikost, x, y, smer);
        //tukaj se pošlje serverju te koordinate da si jih zapiše pošlje ax,ay seveda

        izhod = "";//zbriši vse napisano in nadaljuj z naslednjo ladjico
        izhod = zacetek + std::to_string(m + razmaky + 4) + ';' + std::to_string(razmakx) + 'H';
        std::cout << izhod;
        std::cout << "    ";

        stLadjic--;
    }                       //konec risanje ladij

    //izbriši tekst
    izhod = "";
    izhod = zacetek + std::to_string(razmaky + 1) + ';' + std::to_string(razmakx + 2 * m + 4) + 'H' + "    ";
    std::cout << izhod;
    izhod = zacetek + std::to_string(razmaky + 2) + ';' + std::to_string(razmakx + 2 * m + 4) + 'H' + "    ";
    std::cout << izhod;
    izhod = zacetek + std::to_string(razmaky + 3) + ';' + std::to_string(razmakx + 2 * m + 4) + 'H' + "    ";
    std::cout << izhod;
    izhod = "";



    //do tukaj naredi prvi del Postavitev ladjic
    //tukaj se zaène drugi del Igra
    //kjer se tudi prikaže nasprotnikovo polje
       
    igralnoPolje(m, razmakx + 2 * m + 10, razmaky);//nasprotnikovo polje

    //while loop za igranje

    char buf[4096];
    char in_buf;
    std::string userInput = "";
    fd_set readSet;


                                                //to je vzeto it BareboneClient treba spremeniti
                                                //tukaj je samo za referenco kaj mora biti narejeno
    while (1)
    {        
        if (_kbhit())
        {
            in_buf = _getch();
            if (in_buf == 13)
            {
                if (userInput.size() > 0)
                {
                    int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
                    userInput = "";
                }
                else
                {
                    break;
                }
            }
            else
            {
                userInput = userInput + in_buf;
            }
        }

        FD_ZERO(&readSet);
        FD_SET(sock, &readSet);

        timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;

        if (select(0, &readSet, nullptr, nullptr, &timeout) > 0)
        {
            ZeroMemory(buf, 4096);
            int bytesReceived = recv(sock, buf, 4096, 0);
            if (bytesReceived > 0)
            {
                std::cout << "prihaja> " << std::string(buf, 0, bytesReceived) << std::endl;
            }

            if (bytesReceived == SOCKET_ERROR)
            {
                std::cerr << "napaka v recv()" << std::endl;
                return 1;
            }
        }


    }


    //tukaj je konec igre
    izhod = zacetek + std::to_string(m + razmaky + 5) + ';' + '1' + 'H';
    std::cout << izhod;
    system("pause");
    return 0;
}