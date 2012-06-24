#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
using namespace std;

HANDLE servMutex;
HANDLE keybMutex;

DWORD WINAPI getMsg(LPVOID dSocket);
DWORD WINAPI getKeyb(LPVOID dKeyb);

string streamTickCount();

void SetTextColor(int colour);


int quit;
int original;

int main(int argc, char* argv[])
{
    srand (time(NULL));

    setlocale(LC_ALL,"rus");
    SetTextColor(3);
    system("cls");
    original=GetConsoleCP();
    SetConsoleCP(1251);
    quit = 0;
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
        printf("Socket() error %d\n", WSAGetLastError());
        return -1;
    }

    char SERVERADDR[16]="";
    int PORT=0;

    printf("Input IP adress of the server: ");
    cin>>SERVERADDR;
    //SERVERADDR="127.0.0.1".c_str());
    printf("Input the server PORT: ");
    cin>>PORT;
    //PORT="5000";
    if(PORT==0)
    {
        string addrSrv = "127.0.0.1 4799";
        istringstream sstr( addrSrv );
        sstr >> SERVERADDR >> PORT;
    }

    sockaddr_in dest_addr; ///ПОЧЕМУ ПЕРЕМЕННАЯ НАЗВАНА ИМЕННО ТАК?

    /**struct sockaddr_in
{
    short sin_family; // семейство протоколов (как правило, AF_INET)
    unsigned short sin_port; // порт
    struct in_addr sin_addr; // IP-адрес
=!= на самом деле есть: sin_addr.s_addr, почему sin_addr хранит подструктуру?
    char sin_zero[8]; // не используется, должно быть заполнено нулями
};
    **/
    dest_addr.sin_family = AF_INET; //мы точно работаем по TCP
    dest_addr.sin_port = htons(PORT); //порт преобразововать не надо, сразу пишем в структуру
    HOSTENT *hst; ///структура в которой хранится DNS сервера(имя, основной адр, побочные,aux)
    /**struct hostent
{
    char* h_name;            	// официальное имя узла
    char* FAR * h_aliases;   	// альтернативные имена узла (массив строк)
    short h_addrtype;             	// тип адреса
    short h_length;               	// длина адреса (как правило AF_INET)
    char** h_addr_list; 	// список указателей на IP-адреса. ноль - конец списка
}; */


    ///если нам дали ip, то ок пишем его в структуру
    if (inet_addr(SERVERADDR) != INADDR_NONE)
        dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    else///если не ip,пытаемся получить ip SERVERADDR`а
    {
        if (hst = gethostbyname(SERVERADDR))///если получилось получить host addr by name -> true
        {
            struct in_addr* ad = (struct in_addr*)hst->h_addr_list[ 0 ];
            dest_addr.sin_addr.s_addr = ad->s_addr;
        }
        else ///нет такого адреса/неправильный адрес
        {
            printf("Invalid address %s\n", SERVERADDR);
            closesocket(my_sock);
            WSACleanup();
            return -1;
        }
    }
        ///если нам дали ip не существ. сервера
    if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
    {
        printf("Connect error %d\n", WSAGetLastError());
        return -1;
    }

        ///streamTickCount () - время от старта системы в милисекундах
    cout<<"Connection with " << SERVERADDR << " was established successfully\n";
    //string t="random"+rand() % 10 + 1;
    servMutex = CreateMutex(
                    NULL,                       // default security attributes
                    FALSE,                      // initially not owned!!
                    ( string("ServMutex")+streamTickCount() ).c_str());               // name mutex     "random");

    keybMutex = CreateMutex(
                    NULL,                       // default security attributes
                    FALSE,                      // initially not owned!!
                    //"key");
                    ( string("KeybMutex")+streamTickCount() ).c_str());               // name mutex
    DWORD servThID;
    CreateThread(NULL, 0, getMsg, &my_sock, 0, &servThID);
    /**HANDLE CreateThread(

LPSECURITY_ATTRIBUTES lpThreadAttributes, // дескриптор защиты
SIZE_T dwStackSize,                       // начальный размер стека
LPTHREAD_START_ROUTINE lpStartAddress,    // функция потока
LPVOID lpParameter,                       // параметр потока
DWORD dwCreationFlags,                    // опции создания
LPDWORD lpThreadId                        // идентификатор потока

);*/
    DWORD keybThID;        ///getKeyb - функция, с 1 обяз параметром
    CreateThread(NULL, 0, getKeyb,  &my_sock, 0, &keybThID);
    //cout<<getKeyb<<endl;


    HANDLE h[2]= {servMutex, keybMutex};
    Sleep(2000);

    while (!quit )
    {
        DWORD dw = WaitForMultipleObjects(2, h, FALSE, INFINITE);
        /**DWORD WaitForMultipleObjects(
DWORD nCount,               // число объектов в массиве lpHandles
CONST HANDLE *lpHandles,    // указатель на массив описателей объектов ядра
BOOL bWaitAll,              // флаг, означающей надо ли дожидаться всех объектов или достаточно одного
DWORD dwMilliseconds        // таймаут
);*/
        switch (dw)
        {
        case WAIT_FAILED:
            // неправильный вызов функции (неверный описатель?)
            break;
        case WAIT_TIMEOUT:
            // ни один из объектов не освободился в течение таймаута (если устанволен)
            //у нас INFINITE, так что думаю это ткусок кода не когда выполниться
            std::cout << "Error: Time OUT!\n";
            break;
        case WAIT_OBJECT_0 + 0:
            // освободился мьютекс от сервера
            //         std::cout << "Get message from server\n";
            ReleaseMutex( servMutex );
            break;
        case WAIT_OBJECT_0 + 1:
            // освободился мьютекс от клавиатуры
            //            std::cout << "Get message from keyboard\n";
            ReleaseMutex( keybMutex );
            break;
        }
    }
    CloseHandle( servMutex );
    CloseHandle( keybMutex );

    WSACleanup();
    return 0;
}

DWORD WINAPI getMsg(LPVOID dSocket)
{
    int my_sock = *(reinterpret_cast<int*> (dSocket));
    char buff[100];
    int bytes_recv;
    int color=0;
    WaitForSingleObject(servMutex, INFINITE);

    //  string msgAns = "SEND\n";
    // send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);

    while ( (bytes_recv = recv(my_sock, buff, sizeof(buff) - 1, 0 )) )
    {
        if ( bytes_recv == SOCKET_ERROR )
            break;
        buff[ bytes_recv ] = 0;

        char* tmpc;
        tmpc = new char [1];
        if(buff[0]=='#')
        {
            tmpc[0]= buff[1];
            color = atoi(tmpc);
            for(int i=0; i<bytes_recv; i++)
            {
                buff[i]=buff[i+2];
            }
        }


        SetConsoleCP(original);
        SetTextColor(color);
        printf("%s",buff);
        SetTextColor(1);
        SetConsoleCP(1251);
        ReleaseMutex( servMutex );
        WaitForSingleObject(servMutex, INFINITE);

    }
    closesocket(my_sock);

    return 0;
}

DWORD WINAPI getKeyb(LPVOID dKeyb)
{
    int my_sock = *(reinterpret_cast<int*> (dKeyb));
    string msgOut;
    WaitForSingleObject(keybMutex, INFINITE);

    while ( msgOut != "quit" )
    {
        SetTextColor(1);

        getline( cin, msgOut );
        send(my_sock, msgOut.c_str(), strlen( msgOut.c_str() ), 0);

        ReleaseMutex( keybMutex );
        WaitForSingleObject(keybMutex, INFINITE);
    }
    quit = 1;
}



void SetTextColor(int colour)
{
    int text;
    int background;
    switch (colour)
    {
    case 1: //Все отправляемые команды + повтор отправленного сообщения
        text=2;
        //text=1;
        background=15;
        break;
    case 2: //Сообщения получаемые
        text=5;
        //text=1;
        background=15;
        break;
    case 3: //Ответы на рабочие команды
        text=19;
        //text=1;
        background=15;
        break;
    case 4: //Ответы на технические команды
        text=1;
        background=15;
        break;
    case 5: //Сообщения о различных ошибках ( авторизации, формата команд и т.д.)
        text=4;
        background=15;
        break;
    }
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

string streamTickCount()
{
  stringstream stream;
  stream<<GetTickCount();
  return stream.str();
}


