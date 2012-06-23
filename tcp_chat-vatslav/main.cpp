#include <string>
#include <stdio.h>
#include <winsock2.h>
#include <map>
#include <sstream> ///string stream
#include <iostream> /// основной ввод-вывод
//signs #
//libwsock32
DWORD WINAPI ServClient(LPVOID data);

int nclients = 0;
//инициализируем поток как файл
using std::istringstream; /// работа со строками как с потоком
using std::string;
using std::cout;
using std::endl;


void ign(int& my_sock, bool& isConn,struct client* cl,istringstream& sstr,char* buff,string ip);
void online(int& my_sock, bool& isConn,struct client* cl);
void bye(int& my_sock,bool& isConn,struct client* cl,char* buff,string ip);
void hello(int& my_sock, bool& isConn,struct client* cl,istringstream& sstr,char* buff,string ip);
void msg(int& my_sock, bool& isConn,struct client* cl,istringstream& sstr,char* buff,string ip);
void echoes(int& my_sock, char* buff); ///эхо сообщение для клиента
void goodspace(char* message);
void SetTextColour(int colour); /// прототипы функций
int maskpointer (string clientTo);
char* myTime();
/// структура клиенты
struct client
{
    string name; //имя
    int socket; /// хранит дескриптор
    string addr; ///Строка с IP адресом
};
///кого игнорим
struct ignList
{
    string from;  /// кто
    string to; /// кого игнорирует
};

struct sortIgn //cтруктура необходимая для карты
{
    //возвращает 1 если значение меньше
    bool operator()(ignList const &a,ignList const &b)const
    {
        return (a.from + a.to)<(b.from + b.to);
    }
};
///стандартны контейнер map с использованием hand-maid`овских структур
std::map <string, int> arrClients;
std::map <ignList, int, sortIgn> arrIgnClients;


/*Функция WSAStartup инициализирует WinSock.
Эта функция всегда самая первая при начале работы с WinSock.
Вот как она описана:

int WSAStartup (
    WORD wVersionRequested,
    LPWSADATA lpWSAData  );
Первый параметр - это версия, которая будет использоваться.
    Младший байт основная версия, старший байт расширение версии.
    Если инициализация состоялась, то вернется нулевое значение.
    Вобщем-то инициализация заключается в сопоставлении номера версии и реально существуюшей DLL в системе.
Второй параметр - это указатель на структуру WSADATA, в которую возвратятся параметры инициализации.

typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN+1];
    char szSystemStatus[WSASYS_STATUS_LEN+1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char FAR * lpVendorInfo;
} WSADATA, FAR * LPWSADATA;

Использованные выше поля szDescription и szSystemStatus после вызова функции WSAStartup содержат,
соответственно, описание конкретной реализации интерфейса Windows Socket
и текущее состояние этого интерфейса в виде текстовых строк.
В полях wVersion и wHighVersion записаны, соответственно,
версия спецификации Windows Socket, которую будет использовать приложение,
и версия спецификации, которой соответствует конкретная реализация интерфейса Windows Socket.
Приложение может одновременно создавать несколько сокетов, например,
для использования в разных подзадачах одного процесса.
В поле iMaxSockets хранится максимальное количество сокетов,
которое можно получить для одного процесса.
В поле iMaxUdpDg записан максимальный размер пакета данных,
который можно переслать с использованием датаграммного протокола UDP .
И, наконец, поле lpVendorInfo содержит указатель на дополнительную информацию,
формат которой зависит от фирмы-изготовителя конкретной реализации системы Windows Sockets.

WSACleanup завершает использование данного DLL и прерывает обращение к функциям WinSock.
    При удачном выполнении вернется нуль.
*/


int main(int argc, char* argv[])
{
    SetTextColour(4);
    system("cls");
    setlocale(LC_ALL,"rus");
    string my_port_s; /// порт сервака в строку
    int my_port; ///в одно число
    WSADATA g_wsadata;
    SetTextColour(4);
    printf("TCP SERVER DEMO\n");
    ///2,2 - версия системы сокетов
    ///WSAStartup - инициализация интерфейса  Windows Sockets
    if (WSAStartup( MAKEWORD( 2, 2 ), &g_wsadata )) /// 0 - успех
    {
        SetTextColour(4);
        ///WSAGetLastError - возврщает код ошибки
        printf("ERROR: Error WSAStartup %d\n", WSAGetLastError());
        return -1;
    }

    int mysocket;
    ///попытка создать сокет (потом его изменим)
    /*Создание socket'а:

    int socket (domain, type, protocol)
        int domain;
        int type;
        int protocol;

    Аргумент domain задает используемый для взаимодействия набор протоколов
    (вид коммуникационной области), для стека протоколов TCP/IP
    он должен иметь символьное значение AF_INET.

    Аргумент type задает режим взаимодействия:

    SOCK_STREAM - с установлением соединения;
    SOCK_DGRAM - без установления соединения.
    Аргумент protocol задает конкретный протокол транспортного уровня
    (из нескольких возможных в стеке протоколов). Если этот аргумент задан равным 0,
    то будет использован протокол "по умолчанию"
    (TCP для SOCK_STREAM и UDP для SOCK_DGRAM при использовании комплекта протоколов TCP/IP).

    При удачном завершении своей работы данная функция возвращает
    дескриптор socket'а - целое неотрицательное число, однозначно его идентифицирующее.

    При обнаружении ошибки в ходе своей работы функция возвращает число "-1".*/

/// если у нас все есть для создания сокетов, делаем его)
    if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        SetTextColour(4);
        printf("ERROR: Error socket %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    printf("Input PORT number: ");
    //std::cin>>my_port_s;
    //my_port=atoi(my_port_s.c_str());
    cout<<"5000 \n";my_port=4799;
    ///.c_str() Возвращает указатель на массив символов + завершающий ноль
    while(my_port>65535 or my_port<=0) ///читаем порт пока не введут "хороший"
    {
        SetTextColour(4);
        printf("ERROR: Input PORT number (0<PORT number<65535): ");
        std::cin>>my_port_s;
        my_port=atoi(my_port_s.c_str());
    }
    /*
    Поле sin_family определяет используемый формат адреса (набор протоколов),
    в нашем случае (для TCP/IP) оно должно иметь значение AF_INET.
    Поле sin_addr содержит адрес (номер) узла сети.
    Поле sin_port содержит номер порта на узле сети.
    Если поле sin_addr.s_addr имеет значение INADDR_ANY,
    то системный вызов будет привязывать к socket'у номер (адрес) локального узла сети.
    */
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(my_port);
    /*Функция htons осуществляет перевод целого короткого числа из порядка байт,
    принятого на компьютере, в сетевой порядок байт.*/
    local_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    /*Функция htonl осуществляет перевод целого длинного числа из порядка байт,
    принятого на компьютере, в сетевой порядок байт.*/
    /*Чтобы в них не запутаться, достаточно запомнить, что за буквой "n" скрывается сокращение
    "network", за "h" - "host" (подразумевается локальный), "s" и "l" соответственно короткое
    (short) и длинное (long) беззнаковые целые, а "to" обозначает преобразование.
    Например, "htons" расшифровывается так: "Host ( Network (short )"
    т.е. преобразовать короткое целое из формата локального хоста в сетевой формат.
    */
    if (                            ///Связывание сокета с локальным портом
        bind(mysocket,              /// (in) дескриптор сокета
             (sockaddr *)&local_addr,    /// (in) структура, содержащая нужный адрес и порт
             sizeof(local_addr))         /// (in) длинна параметра local_addr
    )      ///В случае успеха bind возвращает 0, в противном случае - "-1".
    {
        SetTextColour(4);
        printf("ERROR: Error bind %d\n", WSAGetLastError());
        closesocket(mysocket);
        WSACleanup();
        return -1;
    }

    if (listen(
                mysocket,///дескриптор socket'а через который слушать
                256)    ///максимальная длина очереди входящих запросов на установление связи
       )
    {
        SetTextColour(4);
        printf("ERROR: Error listen %d\n", WSAGetLastError());
        closesocket(mysocket);
        WSACleanup();
        return -1;
    }
    SetTextColour(4);
    printf("Waiting connections...\n");

    int client_socket; ///объявляем переменную для "нужд" клиента
    sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
///accept() - прием запросов от клиентов на установление связи
    /*Системный вызов accept извлекает из очереди, организованной системным вызовом listen,
    первый запрос на соединение и возвращает дескриптор нового
    (автоматически созданного) socket'а с теми же свойствами, что и socket,
    задаваемый первым аргументом. Этот новый дескриптор необходимо использовать
    во всех последующих операциях обмена данными с килентом.

    После удачного завершения accept второй и третий аргумент будут заполнены соответствующими данными
    Если очередь запросов на момент выполнения accept пуста, то программа переходит
    в состояние ожидания поступления запросов от клиентов на неопределенное время.

    Признаком неудачного завершения accept служит отрицательное возвращенное значение
    (дескриптор socket'а отрицательным быть не может).*/
///собственно сам "товарищ" accept ->
    while (client_socket = accept(
                               mysocket, ///дескриптор socket'а, через который программа-сервер получила запрос
                               //на соединение (посредством системного запроса listen ).
                               (sockaddr *)&client_addr, ///указывает на область памяти, размер
                               //которой позволял бы разместить в ней структуру данных,
                               //содержащую адрес socket'а клиента, сделавшего запрос на соединение.
                               //Никакой инициализации этой области не требуется.
                               &client_addr_size)///указывать на область памяти в виде целого числа,
                           //задающего размер (в байтах) области памяти, указываемой аргументом addr.
          )
    {
        nclients++;

        HOSTENT *hst; ///Структура hostent используется функциями, чтобы хранить информацию о клинте: его имя, тип, IP адрес, и т.д.
        hst = gethostbyaddr(///Функция gethostbyaddr возвращает информацию о клиенте по его IP адресу
                  (char *)&client_addr.sin_addr.s_addr, /// указатель на адрес записанный в сетевом порядке
                  4,  ///длина адреса в байтах
                  AF_INET///тип адреса (основан на протоколе)
              );

        SetTextColour(2);
        printf("%sNew connect! => %s [%s] \n", //Выводим на экран коннект
               myTime(),
               ( hst != NULL ) ? hst->h_name : "", ///имя хоста
               inet_ntoa(client_addr.sin_addr) ///из числового в сивольный адрес
              );

        SetTextColour(4);
        if (nclients)
            printf("%d user on-line\n", nclients);
        else
            printf("No User on line\n");


        DWORD thID; ///Double word Двойное слово
        struct client* cl = new client;
        cl->name = "";
        cl->socket = client_socket; ///"Переменная для нужд клиента" ее заполняет функция accept
        cl->addr = inet_ntoa(client_addr.sin_addr); ///Суем туда IP адрес
        ///создаем поток (нить) для выполнения в виртуальном адресном пространстве Родителя
        CreateThread(NULL,  // дескриптор защиты (может ли дискриптор быть Пронаследован)
                     0,              // начальный размер стека
                     ServClient,     // функция потока
                     cl,             // параметр потока
                     0,              // опции создания
                     &thID           // ID потока (мы выше его создали, тут отправили адресс и заполнили его)
                    );
    }

    WSACleanup(); ///дойдем до сюда если accept закончится
    return 0; ///закончим main
}

DWORD WINAPI ServClient(LPVOID data) ///принимаем безтиповые данные
{
    ///приводим безтиповый тип к client объект data и засовываем его в cl
    struct client* cl = reinterpret_cast<struct client*>( data );
    int my_sock = cl->socket; ///вытаскиваем сокет и пишем его в my_sock
    char buff[100]; ///размер буфера
    int bytes_recv;
    bool isConn = false;
    string ip="["+cl->addr+"]";
    while ( (bytes_recv = recv( ///Чтение данных из сокета.
                              ///Функция возвращает число считанных байтов или -1 в случае ошибки.
                              ///Нулевое значение сигнализирует об отсутствии записанных в сокет процессом-поставщиком данных.
                              my_sock,        ///сокет-дескриптор, из которого читаются данные
                              buff,           /// адрес буфера для записи читаемых данных
                              sizeof(buff) - 1,/// длина буфера для записи читаемых данных
                              0)/* комбинация битовых флагов, управляющих режимами чтения.
        Если аргумент flags равен нулю, то считанные данные удаляются из сокета.
        Если значение flags есть MSG_PEEK, то данные не удаляются и могут быть
        считаны последущим вызовом ( или вызовами ) recv
        */)
          )
    {
        if ( bytes_recv == SOCKET_ERROR )break;///Если была ошибка принятия сообщения

        buff[ bytes_recv ] = 0;
        istringstream sstr( buff );
        string command;
        sstr >> command;

        if(!isConn and !(command == "HELLO" or command == "HELP"))
        {

            echoes(my_sock,buff);///Повтор отправленной команды
            SetTextColour(4);
            ///%текущие время если имя клиента пусто вернуть его ip Иначе: имя.c_str
            printf("%s ERROR: %s: %s\n",myTime(), cl->name=="" ? ip.c_str() : cl->name.c_str(), buff);
            string msgAns = "#5ERROR: I dont know you\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else if ( command.empty() )
        {
            echoes(my_sock,buff);
            SetTextColour(4);
            printf("%s ERROR: %s: <No command>\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str());
            string msgAns = "#5ERROR: No command\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else if ( command == "HELLO" )
            hello ( my_sock,isConn,cl,sstr,buff,ip );

        else if ( command == "BYE")
        {
            SetTextColour(2);
            printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            bye(my_sock,isConn,cl,buff,ip);
        }
        else if ( command == "MSG")
            msg(my_sock, isConn, cl, sstr,buff,ip);

        else if ( command == "IGN")
            ign(my_sock, isConn, cl, sstr,buff,ip);

        else if ( command == "ONLINE")
        {
            SetTextColour(3);
            printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            online(my_sock, isConn, cl);
        }
        else if ( command == "HELP")
        {
            SetTextColour(3);
            printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            string msgAns ="#4";
            msgAns+="HELLO <name> - authorise | unauthorise a client\n";
            msgAns+="BYE - disable authorisation\n";
            msgAns+="MSG <nameTo> <message> - send the message 'message' to the client with name 'nameTo' and key \n";
            msgAns+="ONLINE -view online list of connected clients\n";
            msgAns+="IGN <name> - add existed client 'name' to ignore list\n";
            msgAns+="HELP - view the list of commands\n";
            send (my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else
        {
            echoes(my_sock,buff);
            SetTextColour(4);
            printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            string msgAns = "#5ERROR: No such command\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }

    }

    bye(my_sock,isConn,cl,buff,ip);
    SetTextColour(4);
    nclients--;
    printf("Disconnected User %s\n", cl->name==""?ip.c_str():cl->name.c_str());

    if (nclients)
        cout << nclients << " user on-line\n";
    else
        cout << "No User on line\n";
    SetTextColour(1);
    closesocket(my_sock);
    delete cl;
    return 0;
}

void hello(int& my_sock, bool& isConn, struct client* cl, istringstream& sstr,char* buff,string ip)
{
    if ( isConn )
    {
        echoes(my_sock,buff);
        SetTextColour(4);
        printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
        string msgAns = "#5ERROR: you are already authorised\n";
        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
    }
    else
    {
        string name;
        sstr >> name;
        bool signs=0;
        int len = strlen(name.c_str());
        int t=0;
        for (int i = 0 ; i < len ; i++)
        {
            t=(int)name[i];
            if ( !( (t>=65 and t<=90) or (t>=97 and t<=122 ) or (t>=-64 and t<=-1) or ( t==-88 ) or ( t == -72) or (t>=48 and t<=57 ) ) )
            {
                signs=1;
            }
        }
        if ( name.empty() )
        {
            echoes(my_sock,buff);
            SetTextColour(4);
            printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            string msgAns = "#5ERROR: No client name specified\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else if(signs)
        {
            echoes(my_sock,buff);
            SetTextColour(4);
            printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            string msgAns = "#5ERROR: Name must contain only letters and numbers\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else if ( arrClients.count(name) )
        {
            echoes(my_sock,buff);
            SetTextColour(4);
            printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            string msgAns = "#5ERROR: There is already such client\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else if(name=="ALL" or name=="ВСЕ" or name == "SERVER" or name == "СЕРВЕР" or name == "server" or name == "сервер" )
        {
            echoes(my_sock,buff);
            SetTextColour(4);
            printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            string msgAns = "#5ERROR: Name "+name+" can not be used.\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else
        {
            SetTextColour(2);
            printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            isConn = true;
            arrClients[ name ] = my_sock;   // overwrite
            cl->name = name;
            // string msgAns = myTime();
            //msgAns += "<server>: HI, " + name + "\n";
            SetTextColour(4);
            cout << myTime()<<" <New client>: " << cl->name <<"\n";
            string msgAns = "#1"; ///цвет ответа сервера клиенту на Hello
            msgAns+= myTime();
            msgAns += "SERVER: HI, " + name + "\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);


            int sockTo=0;
            string clientTo = "";
            string tmp_str = "";
            ignList ign_tmp;
            std::map <string, int>::iterator start = arrClients.begin();
            std::map <string, int>::iterator end = arrClients.end();
            ign_tmp.from = cl->name;
            for (; start != end; start++)
            {
                clientTo = start->first;
                ign_tmp.to = clientTo;
                if ( !arrIgnClients.count(ign_tmp) and clientTo!=cl->name )
                {
                    sockTo = arrClients[ clientTo ];
                    tmp_str="#2";
                    tmp_str+=myTime();
                    tmp_str+=cl->name;
                    tmp_str+=" has connected.\n";
                    send(sockTo, tmp_str.c_str(), strlen( tmp_str.c_str() ), 0);
                }

            }

        }
    }
}

void msg(int& my_sock, bool& isConn,struct client* cl,istringstream& sstr,char* buff,string ip)
{
    char message[ 100 ]; ///размер буфера
    string key="";
    string clientTo="";
    string tmp_str="";
    ignList ign_tmp;
    int sockTo=0;
    sstr >>clientTo; ///считываем до пробела (имя клиента)
///сам мсг
   // cout<<clientTo<<" name cl \n";
    sstr.getline( message, sizeof( message ) - 1 );
///делаем корректный конец строки
    if (  ( message[ strlen( message) - 1 ] == '\r' ) and ( strlen( message) != 0 ) )
        message[ strlen( message) - 1 ] = '\0';
///работаем с message как c потоком
    istringstream mes_str( message );
    mes_str>>key;
///если всем и указан доп ключ то - ошибка
    if(clientTo=="ALL")
    {
        if(key=="-p")
        {
            echoes(my_sock,buff); ///возвращаем клиенту то что получили, изменив в соответсвии с протоколом
            SetTextColour(4); ///задаем цвет сообщений
            ///%время %адресс (если знаем то имя, если нет, то ip)
            printf("%s ERROR: %s: %s\n",myTime(), cl->name=="" ? ip.c_str() : cl->name.c_str(), buff);
            string msgAns = "#5ERROR: You can't send private messages to all\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else
            key="-a"; ///ставим ключ обычного сообщения
    }
    else if (key=="-p") ///если приватное
        {
            int len=strlen( message );
            int pointer=mes_str.tellg(); ///след. указатель на чтение
            for (int i = 0; i<= len - pointer; i++ )
            {
                message[ i ] = message[ i+pointer ]; ///сдвигаем сообщение, убрав из него ключ
            }
        }
        else
        {
            key="-u"; ///не приватное (обычное) - по протоколу видно всем
        }

    goodspace(message); ///удаляем пробелы

///если имя пусто
    if ( clientTo.empty() )
    {
        echoes(my_sock,buff);
        SetTextColour(4);
        printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
        string msgAns = "#5ERROR: NameTo is empty\n";
        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
    }
    ///если имя отправителя и адресата совпадают
    else if(clientTo==cl->name)
    {
        echoes(my_sock,buff);
        SetTextColour(4);
        printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
        string msgAns = "#5ERROR: You can't send messages to yourself\n";
        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
    }
///если сообщение пустое
    else if ( strlen( message ) == 0 and ( arrClients.count( clientTo ) or clientTo == "ALL" or maskpointer(clientTo)) )
    {
        echoes(my_sock,buff);
        SetTextColour(4);
        printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
        string msgAns = "#5ERROR: You can't send empty messages\n";
        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
    }
    else
    {
        ///конец строки корректный
        message[ strlen( message ) + 1 ] = '\0';
        message[ strlen( message) ] = '\n';

        if (key == "-a")
        {
            std::map <string, int>::iterator start = arrClients.begin();
            std::map <string, int>::iterator end = arrClients.end();
            ign_tmp.from = cl->name;
            for (; start != end; start++)
            {
                clientTo = start->first;
                ign_tmp.to = clientTo;
                if ( !arrIgnClients.count(ign_tmp) )
                {
                    sockTo = arrClients[ clientTo ];
                    tmp_str="#2";
                    if (clientTo == cl->name ) tmp_str="#1";
                    tmp_str+=myTime();
                    tmp_str+=cl->name;
                    tmp_str+=": ";
                    tmp_str+=message;
                    send(sockTo, tmp_str.c_str(), strlen( tmp_str.c_str() ), 0);
                }
                else
                {
                    SetTextColour(4);
                    printf("%s %s->%s (ALL) !IGNORED!: %s",myTime(),cl->name.c_str(),clientTo.c_str(),message);
                }
            }
            SetTextColour(1);
            printf("%s %s: %s",myTime(),cl->name.c_str(),message);
        }
        if (key == "-u")
        {
            if (maskpointer(clientTo))   ///если в имени есть маска (звездочки)
            {
                int found=0;
                std::map <string, int>::iterator start = arrClients.begin();
                std::map <string, int>::iterator end = arrClients.end();
                ign_tmp.from = cl->name;
                for ( int i = 0; start != end; start++)
                {
                    for(i=0; i<maskpointer(clientTo); i++)
                    {
                        if (start->first[i]!=clientTo[i])
                        {
                            i=maskpointer(clientTo)+1;
                        }
                    }
                    if(i==maskpointer(clientTo))
                    {
                        found++;
                        ign_tmp.to = start->first;
                        if ( !arrIgnClients.count(ign_tmp) )
                        {
                            sockTo = arrClients[ start->first];
                            tmp_str="#2";
                            if (clientTo == cl->name ) tmp_str="#1";
                            tmp_str+=myTime();
                            tmp_str+=cl->name;
                            tmp_str+="->";
                            tmp_str+=clientTo;
                            tmp_str+=": ";
                            tmp_str+=message;
                            send(sockTo, tmp_str.c_str(), strlen( tmp_str.c_str() ), 0);
                        }
                        else
                        {
                            SetTextColour(4);
                            printf("%s %s->%s !IGNORED!: %s",myTime(),cl->name.c_str(),start->first.c_str(),message);
                        }
                    }
                }
                if (!found)
                {
                    echoes(my_sock,buff);
                    SetTextColour(4);
                    printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
                    string msgAns = "#5ERROR: clients '" + clientTo + "' not found\n";
                    send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
                }
                SetTextColour(1);
                printf("%s %s->%s: %s",myTime(),cl->name.c_str(),clientTo.c_str(),message);
            }
            else if ( !arrClients.count( clientTo ))
            {
                echoes(my_sock,buff);
                SetTextColour(4);
                printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
                string msgAns = "#5ERROR: client '" + clientTo + "' not found\n";
                send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
            }
            else
            {
                string clTo;
                std::map <string, int>::iterator start = arrClients.begin();
                std::map <string, int>::iterator end = arrClients.end();
                ign_tmp.from = cl->name;
                for (; start != end; start++)
                {
                    clTo = start->first;
                    ign_tmp.to = clTo;
                    if ( !arrIgnClients.count(ign_tmp) )
                    {
                        sockTo = arrClients[ clTo ];
                        tmp_str="#2";
                        if (clTo == cl->name ) tmp_str="#1";
                        tmp_str+=myTime();
                        clTo != cl->name ?  tmp_str+=cl->name: tmp_str+="";
                        tmp_str+="->";
                        tmp_str+=clientTo;
                        tmp_str+=": ";
                        tmp_str+=message;
                        send(sockTo, tmp_str.c_str(), strlen( tmp_str.c_str() ), 0);
                    }
                    else
                    {
                        SetTextColour(4);
                        printf("%s %s->%s !IGNORED!: %s",myTime(),cl->name.c_str(),clTo.c_str(),message);
                    }
                }

                SetTextColour(1);
                printf("%s %s->%s: %s",myTime(),cl->name.c_str(),clientTo.c_str(),message);

            }
        }
        if (key == "-p")
        {
            ign_tmp.from = cl->name;
            ign_tmp.to = clientTo;
            if ( !arrClients.count( clientTo ))
            {
                echoes(my_sock,buff);
                SetTextColour(4);
                printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
                string msgAns = "#5ERROR: client '" + clientTo + "' not found\n";
                send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
            }
            else if ( arrIgnClients.count(ign_tmp) )
            {
                SetTextColour(4);
                printf("%s %s->%s (PRIVATE) !IGNORED!: %s",myTime(),cl->name.c_str(),clientTo.c_str(),message);
            }
            else
            {
                sockTo = arrClients[ clientTo ];
                tmp_str="#2";
                tmp_str+= myTime();
                tmp_str+= cl->name.c_str();
                tmp_str+= " (PRIVATE): ";
                tmp_str+= message;
                send(sockTo, tmp_str.c_str(), strlen( tmp_str.c_str() ), 0);
                SetTextColour(1);
                printf("%s %s->%s !PRIVATE!: %s",myTime(),cl->name.c_str(),clientTo.c_str(),message);

                tmp_str="#2";
                tmp_str+= myTime();
                tmp_str+="->";
                tmp_str+= clientTo.c_str();
                tmp_str+= " (PRIVATE): ";
                tmp_str+= message;
                send(arrClients[cl->name], tmp_str.c_str(), strlen( tmp_str.c_str() ), 0);

            }
        }

    }}
void ign(int& my_sock, bool& isConn,struct client* cl,istringstream& sstr,char* buff,string ip)
{
    echoes(my_sock,buff);
    Sleep(1);
    string ignClient="";
    sstr >> ignClient;
    ignList tmp;
    tmp.from =ignClient;
    tmp.to =cl->name;
    if(maskpointer(ignClient.c_str()))
    {
      int found=0;
      std::map <string, int>::iterator start = arrClients.begin();
      std::map <string, int>::iterator end = arrClients.end();
      for ( int i = 0; start != end; start++)
      {
          for(i=0;i<maskpointer(ignClient);i++)
          {
              if (start->first[i]!=ignClient[i])
              {
                  i=maskpointer(ignClient)+1;
              }
          }
          if(i==maskpointer(ignClient))
          {
              found++;
              tmp.from = start->first;
              if ( !arrIgnClients.count(tmp) and tmp.from!=tmp.to)
              {
                  arrIgnClients[ tmp ] = 1;
                  string msgAns = "#3";
                  msgAns += myTime();
                  msgAns += "SERVER: OK: now you ignore '" + tmp.from + "'\n";
                  send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
                  SetTextColour(2);
                  printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
                  SetTextColour(4);
                  printf("%s Client '%s' added '%s' to ignore list\n",myTime(),cl->name.c_str(),tmp.from.c_str());
              }
              else if (arrIgnClients.count(tmp))
              {
                  SetTextColour(4);
                  printf("%s ERROR: %s is already ignoring %s (command:%s)\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),tmp.from.c_str(),buff);
                  string msgAns = "#3";
                  msgAns += myTime();
                  msgAns += "SERVER: ERROR: you are already ignoring '" + tmp.from + "'\n";
                  send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
              }
          }
      }
      if (!found)
      {
        SetTextColour(4);
        printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
        string msgAns = "#5ERROR: clients '" + ignClient + "' not found\n";
        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
      }


    }
    else
        if ( ignClient.empty() )
        {
          echoes(my_sock,buff);
            SetTextColour(4);
            printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
            string msgAns = "#5ERROR: Ignore name is empty\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else
            if ( arrIgnClients.count(tmp) )
            {
                arrIgnClients.erase(tmp);

                SetTextColour(2);
                printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
                SetTextColour(4);
                printf("%s Client '%s' removed '%s' from ignore list\n",myTime(),cl->name.c_str(),ignClient.c_str());

                string msgAns ="#3";
                msgAns += myTime();
                msgAns += "SERVER: OK: '" + ignClient + "' removed from your ignore list\n";
                send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);

            }
            else
                if(!arrClients.count(ignClient))
                {
                  echoes(my_sock,buff);
                    SetTextColour(4);
                    printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
                    string msgAns = "#5ERROR: '" + ignClient + "' not found online\n";
                    send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
                }
                else
                    if(ignClient==cl->name)
                    {
                      echoes(my_sock,buff);
                        SetTextColour(4);
                        printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
                        string msgAns = "#5ERROR: You can't ignore yourself\n";
                        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
                    }
                    else
                    {
                        arrIgnClients[ tmp ] = 1;
                        string msgAns ="#3";
                        msgAns += myTime();
                        msgAns += "SERVER: OK: now you ignore '" + ignClient + "'\n";
                        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
                        SetTextColour(2);
                        printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
                        SetTextColour(4);
                        printf("%s Client '%s' added '%s' to ignore list\n",myTime(),cl->name.c_str(),ignClient.c_str());

                        //cout << myTime()<<" Now "<<cl->name<<" ignore "<<ignClient;
                    }
}

char* myTime()
{
    SYSTEMTIME st;
    GetSystemTime (&st);
    char* myT = new char[11];
    // st.wHour = 24;
    sprintf(myT,"[%02d:%02d:%02d] ", (st.wHour+4) >= 24 ? (st.wHour+4) % (24) : st.wHour+4 , st.wMinute, st.wSecond);
    return myT;
}

void online(int& my_sock, bool& isConn,struct client* cl)
{
    string msgAns="";
    std::map <string, int>::iterator start = arrClients.begin();
    std::map <string, int>::iterator end = arrClients.end();
    for (; start != end; start++)
    {
        msgAns="#4";
        msgAns +=start->first;
        if(start->first==cl->name)
        {
            msgAns+=" <-YOU";
        }
        msgAns+="\n";
        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        Sleep(1);
    }
}

void bye(int& my_sock,bool& isConn,struct client* cl,char* buff,string ip)
{
    echoes(my_sock,buff);
    SetTextColour(4);
    if (isConn)
      printf("Removed client %s\n", cl->name==""?ip.c_str():cl->name.c_str());
    arrClients.erase(cl->name);
    string msgAns = "#3OK: now you unauthorized\n";
    send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
    isConn=0;

    int sockTo=0;
    string clientTo = "";
    string str_tmp = "";
    ignList tmp_ign;
    std::map <string, int>::iterator start = arrClients.begin();
    std::map <string, int>::iterator end = arrClients.end();
    tmp_ign.from = cl->name;
    for (; start != end; start++)
    {
        clientTo = start->first;
        tmp_ign.to = clientTo;
        if ( !arrIgnClients.count(tmp_ign) )
        {
            sockTo = arrClients[ clientTo ];
            str_tmp = "#2";
            str_tmp+=myTime();
            str_tmp+=cl->name;
            str_tmp+=" has disconnected.\n";
            send(sockTo, str_tmp.c_str(), strlen( str_tmp.c_str() ), 0);
        }
    }
cl->name = "";

}

void goodspace(char* message)
{
    int spaces;

    for (spaces=0; message[spaces]==' ' or message[spaces]=='\t' ; spaces++) {}

    int len=strlen( message );
    for (int i = 0; i<=len - spaces; i++ )
    {
        message[ i ] = message[ i + spaces ];
    }
}

void SetTextColour(int colour)
{
    int text;
    int background;
    switch (colour)
    {
    case 1: //ЦВЕТ ДЛЯ ПЕРЕСЫЛАЕМЫХ СООБЩЕНИЙ
        text=1;

        background=15;
        break;
    case 2: //ЦВЕТ ДЛЯ РАБОЧИХ КОМАНД +connect
        text=2;
        //text=1;
        background=15;
        break;
    case 3: //ЦВЕТ ДЛЯ ТЕХНИЧЕСКОЙ ИНФОРМАЦИИ
        text=10;
        //text=1;
        background=15;
        break;
    case 4: //ЦВЕТ ДЛЯ НЕИЗВЕСТНЫХ КОМАНД
        text=0;
        background=15;
        break;
    }
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
}

int maskpointer (string clientTo)
{
    int pointer=0;
    for (; clientTo[pointer]!='*' and pointer < clientTo.length(); pointer++) {}
    return (pointer < clientTo.length())?pointer:0;
}

void echoes(int& my_sock,char* buff)
{
    //Повтор отправленной команды
    goodspace(buff); ///удаляем пробелы из buff
    ///составлеям эхо из 1)цвета 2) время 3)сообщения 4)конец строки
    string qmsgAns = "#1";
    qmsgAns += myTime();
    qmsgAns += buff;
    qmsgAns += "\n";
    send(my_sock, qmsgAns.c_str(), strlen( qmsgAns.c_str() ), 0);
}

