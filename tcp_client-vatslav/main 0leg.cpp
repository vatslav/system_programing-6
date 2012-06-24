// Пример простого TCP-клиента
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <sstream>

DWORD WINAPI getMsg(LPVOID dSocket);

using namespace std;

enum ConsoleColor
{
        Black         = 0,
        Blue          = 1,
        Green         = 2,
        Cyan          = 3,
        Red           = 4,
        Magenta       = 5,
        Brown         = 6,
        LightGray     = 7,
        DarkGray      = 8,
        LightBlue     = 9,
        LightGreen    = 10,
        LightCyan     = 11,
        LightRed      = 12,
        LightMagenta  = 13,
        Yellow        = 14,
        White         = 15
};

void SetColor(ConsoleColor text, ConsoleColor background)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
};

void Non_probels (char (&msg) [500], size_t ind)
{
    int count = 0;
    size_t j = 0;

    while (msg [ind] == ' ')
    {
        count++;
        ind++;
    }
    j = count;

    for (ind = 0; ind < strlen (msg) - count; ind++)
    {
        msg [ind] = msg [j];
        j++;
    }
    msg [ind+1] = '\0';
    msg [ind] = '\n';
    //cout<<"\nCOUNT = "<<count;
}

bool order = false;

int main(int argc, char* argv[])
{
    SetColor (LightGreen,Black);
    setlocale (LC_ALL,".1251");
    //setlocale (LC_ALL,"Rus");
    printf("TCP DEMO CLIENT\n");
    WSADATA g_wsadata;

    if (WSAStartup( MAKEWORD( 2, 2 ), &g_wsadata ))
    {
        printf("WSAStart error %d\n", WSAGetLastError());
        return -1;
    }

    int my_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (my_sock < 0)
    {
        printf("Ошибка сокета %d\n", WSAGetLastError());
        return -1;
    }

    char SERVERADDR[16];
    int PORT;

    //string addrSrv = "127.0.0.1 21";
    //string addrSrv = "10.0.9.20 5000";
    string addrSrv;
    sockaddr_in dest_addr;
    HOSTENT *hst;
    while (order == false)
    {
        cout << "Пожалуйста введите сокет сервера в формате ***.***.***.*** *****: \n";
        getline( cin, addrSrv);

        int cnt = 0;
        for (size_t i = 0; i < strlen (addrSrv.c_str ()); i++)
            if (isalpha (addrSrv [i]))
            {
                 cout << "ОШИБКА: Неверный формат сокета\n\n";
                 cnt++;
                 break;
                 order = false;
            }

        if (cnt == 0)
        {
             istringstream sstr( addrSrv );
             sstr >> SERVERADDR >> PORT;
             dest_addr.sin_family = AF_INET;
             dest_addr.sin_port = htons(PORT);

             if (inet_addr(SERVERADDR) != INADDR_NONE)
                 dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
             else
             {
                 if (hst = gethostbyname(SERVERADDR))
                 {
                    struct in_addr* ad = (struct in_addr*)hst->h_addr_list[ 0 ];
                    dest_addr.sin_addr.s_addr = ad->s_addr;
                 }
                 else
                 {
                    printf("Неверный адрес %s\n\n", SERVERADDR);
                    closesocket(my_sock);
                    WSACleanup();
                    break;
                    order = false;
                    //return -1;
                 }
             }
             if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
             {
                 printf("Ошибка соединения %d\n\n", WSAGetLastError());
                 break;
                 order = false;
                //return -1;
             }
             //else
             //{
                // break;
                 order = true;
             //}
        }
    }
    //cout << "addrSrv = " << addrSrv << " SERVERADDR = " << SERVERADDR << " PORT = " << PORT << endl;

        if (order == true)
        {

            cout<<"Успешное подключение к серверу " << SERVERADDR << "\n Используйте команду 'QUIT' для выхода\n\n";
            SetColor (LightCyan,Black);

            DWORD thID;
            CreateThread(NULL, 0, getMsg, &my_sock, 0, &thID); // поток для получения сообщений

            string msgOut;
            //cout<<"Ввод: ";
            while ( msgOut != "QUIT" )
            {
                getline( cin, msgOut );
                //if (msgOut [0] == 'M' and msgOut [1] == 'S' and msgOut [2] == 'G')
                 //   SetColor (LightCyan,Black);
                send(my_sock, msgOut.c_str(), strlen( msgOut.c_str() ), 0);
            }
        //    printf("Recv error %d\n", WSAGetLastError());
            WSACleanup();
        }

        return 0;
}

DWORD WINAPI getMsg(LPVOID dSocket)
{
    int my_sock = *(reinterpret_cast<int*> (dSocket));
    char buff[500];
    bool ALL = false;
    //int help = 0;

    int bytes_recv;
    while ( (bytes_recv = recv(my_sock, buff, sizeof(buff) - 1, 0)) )
    {
        if ( bytes_recv == SOCKET_ERROR )
            break;

        if ( bytes_recv != 0)
        {
            buff[ bytes_recv ] = 0;
            istringstream sstr( buff );
            int color;
            sstr>>color;
            char msgAnsw[ 500 ];
            sstr.getline( msgAnsw, sizeof( msgAnsw ) - 1 ); // пробелы в начале
            Non_probels (msgAnsw, 0);
            if ( msgAnsw[ strlen(msgAnsw) - 1 ] == '\r')
              msgAnsw[ strlen( msgAnsw) - 1 ] = '\0';

            switch ( color )
            {
              case 0://Все отправляемые команды + повтор отправленного сообщения
                SetColor (LightCyan, Black);
                break;
              case 1://Сообщения получаемые
                SetColor (White, Black);
                break;
              case 2://Ответы на рабочие команды (HELLO, BYE, IGN)
                SetColor (Yellow, Black);
                break;
              case 3://Ответы на технические команды (ONLINE, HELP)
                SetColor (LightGreen, Black);
                break;
              case 4://Сообщения о различных ошибках ( авторизации, формата команд и т.д.)
                SetColor (LightRed, Black);
                break;
              default:
                if (!isalpha (color))
                    SetColor (LightGreen, Black);
                break;
            }
            cout<<msgAnsw;
            SetColor (LightCyan, Black);
        }
        //else
           //cout << "\n";
    }
    closesocket(my_sock);

    return 0;
}

       /** if ( bytes_recv != 0)
        {
            int msgincnt, msgoutcnt = 0;

            //if (buff [0] == 'H')
               //help++;
            for (size_t i = 8; i < strlen (buff); i++)
                if (buff [i] == '[')
                       {
                            SetColor (LightRed,Black);
                            //msgincnt++;
                            ALL = true;
                       }
            for (size_t i = 0; i < sizeof (buff)-1; i++)
               {
                   if (buff [i] == '<' and buff [i+1] == '=')
                   {
                        //cout<<"\nIND = "<<i<<"\n";
                        SetColor (LightRed,Black);
                        msgincnt++;
                   }
                   if (buff [i] == '=' and buff [i+1] == '>')
                   {
                        SetColor (LightCyan,Black);
                        msgoutcnt++;
                   }
               }

            if (msgincnt == 0 and msgoutcnt == 0 and ALL == false)
                SetColor (Yellow,Black);
            cout << buff;
            SetColor (LightGreen,Black);
            //cout << "\nВВОД: ";
        }

        else
        {
            cout << "\n";
            //help = 0;
          //  SetColor (Yellow,Black);
        }
    }
    closesocket(my_sock);

    return 0;
}
*/
