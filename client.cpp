#include <windows.h>
#include <iostream>
#include <conio.h>

void igralnoPolje(int m,int razmakx,int razmaky)
{
    int ax;
    int ay;
    const std::string zacetek = "\x1b[";
    const char kvadrat = 254;
    const char abeceda[28] = "0ABCDEFGHIJKLMNOPQRSTUVWKYZ";

    std::string izhod;

    //nariši polje
    //najprej nastavi barvo
    std::cout << "\x1b[38;2;100;100;100m";//nastavi barvo na rgb
    for (int x = 1;x < m+1;x++)
    {
        for(int y = 1;y < m+1;y++)
        {
            ax = 2*x+razmakx;
            ay = y+razmaky;

            izhod = zacetek + std::to_string(ay) + ';' + std::to_string(ax) + 'H' + kvadrat;//prikaži kvadrat na ax ay koordinati
            std::cout << izhod;
            izhod = "";

        }   
    }

    //pisanje y stolpca
    izhod = "";
    std::cout << "\x1b[38;2;240;100;100m";//nastavi barvo na rgb --rdečkasta
    ax = razmakx-1;//x koordinata je konstantna saj pišemo samo y

    for(int y = 1;y < m+1;y++)
    {
        ay = y+razmaky;

        izhod = zacetek + std::to_string(ay) + ';' + std::to_string(ax) + 'H' + abeceda[m-y+1];
        std::cout << izhod;
        izhod = "";
    }

    //pisanje x stolpca
    izhod = "";
    std::cout << "\x1b[38;2;100;100;240m";//nastavi na barvo rgb --modrikasta
    ay = razmaky+1+m;//y kooridnata je konstantna saj pišemo samo x

    for(int x = 1;x < m+1;x++)
    {
        ax = 2*x+razmakx;

        izhod = zacetek + std::to_string(ay) + ';' + std::to_string(ax) + 'H' + std::to_string(x-1);
        std::cout << izhod;
        izhod = "";
    }

    //ponastavljanje barve 
    std::cout << "\x1b[0m";//ponastavi barve
}


void ladja(int velikost, int x, int y,int smer)
{
    const char kvadrat = 254;   
    const std::string zacetek= "\x1b[";
    std::string izhod;
    
    std::cout << "\x1b[38;2;200;30;170m";//nastavi na barvo rgb --vijolična

    if(smer == 1)
    {
        for(int i = 0;i < velikost;i++)
        {
            izhod = zacetek + std::to_string(y+i) + ';' + std::to_string(x) + 'H' + kvadrat;
            std::cout << izhod;
        }
    }
    if(smer == 0)
    {
        for(int i = 0;i < velikost;i++)
        {
            izhod = zacetek + std::to_string(y) + ';' + std::to_string(x-2*i) + 'H' + kvadrat;
            std::cout << izhod;
        }
    }

    std::cout << "\x1b[0m";//ponastavi barve
}


bool EnableVTMode()
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
    EnableVTMode();
    std::cout << "\x1b[?12l";
    std::cout << "\x1b[?25l";

    int m = 10;
    int razmaky = 4;
    int razmakx = 2*razmaky;
    int x;
    int y;
    const char abeceda[27] = "ABCDEFGHIJKLMNOPQRSTUVWKYZ";
    char crka;
    char stevilka;
    char smer;
    char velikost;
    const std::string zacetek ="\x1b[";
    std::string izhod = "";


    igralnoPolje(m,razmakx,razmaky);

    //risanje ladjic
    int i = 0;
    while(i < 3)
    {
        izhod = "";
        izhod = zacetek + std::to_string(m+razmaky+4) + ';' + std::to_string(razmakx) + 'H';
        std::cout << izhod;



        velikost:;//zajemanje velikosti za ladjo
        while(!kbhit)
        {}
        velikost = getch();
        if(velikost > 48 && velikost < 58)
        {
            std::cout << velikost;
        }
        else
        {
            goto velikost;
        }


        stevilka:;//zajemanje stevilke za lokacijo
        while(!kbhit)
        {}
        stevilka = getch();
        if(stevilka > 47 && stevilka < 58)
        {
            std::cout << stevilka;
        }
        else
        {
            goto stevilka;
        }


        crka:;//zajemanje crke za lokacijo              
        while(!kbhit())
        {}
        crka = getch();
        if(crka < 91 && crka > 64)
        {
            std::cout << crka;
        }
        else
        {
            goto crka;
        }


        smer:;//zajemanje smer ladje 1 navpicno 0 vodoravno
        while(!kbhit())
        {}
        smer = getch();
        if(smer > 47 && smer < 50)
        {
            std::cout << smer;
        }
        else
        {
            goto smer;
        }




        //racunanje koncnih vrednosti
        velikost = velikost-48;
        x = 2*(stevilka-47)+razmakx;
        y = m+1+razmaky-crka+64;
        smer = smer-48; 

        ladja(velikost,x,y,smer);

        izhod = "";
        izhod = zacetek + std::to_string(m+razmaky+4) + ';' + std::to_string(razmakx) + 'H';
        std::cout << izhod;
        std::cout << "    ";

        i++;
    }

    izhod = zacetek + std::to_string(m+razmaky+5) + ';' + '1' + 'H';
    std::cout << izhod;
    system("pause");
    return 0;
}