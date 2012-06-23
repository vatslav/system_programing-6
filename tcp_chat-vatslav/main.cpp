#include <string>
#include <stdio.h>
#include <winsock2.h>
#include <map>
#include <sstream> ///string stream
#include <iostream> /// �������� ����-�����
//signs #
//libwsock32
DWORD WINAPI ServClient(LPVOID data);

int nclients = 0;
//�������������� ����� ��� ����
using std::istringstream; /// ������ �� �������� ��� � �������
using std::string;
using std::cout;
using std::endl;


void ign(int& my_sock, bool& isConn,struct client* cl,istringstream& sstr,char* buff,string ip);
void online(int& my_sock, bool& isConn,struct client* cl);
void bye(int& my_sock,bool& isConn,struct client* cl,char* buff,string ip);
void hello(int& my_sock, bool& isConn,struct client* cl,istringstream& sstr,char* buff,string ip);
void msg(int& my_sock, bool& isConn,struct client* cl,istringstream& sstr,char* buff,string ip);
void echoes(int& my_sock, char* buff); ///��� ��������� ��� �������
void goodspace(char* message);
void SetTextColour(int colour); /// ��������� �������
int maskpointer (string clientTo);
char* myTime();
/// ��������� �������
struct client
{
    string name; //���
    int socket; /// ������ ����������
    string addr; ///������ � IP �������
};
///���� �������
struct ignList
{
    string from;  /// ���
    string to; /// ���� ����������
};

struct sortIgn //c�������� ����������� ��� �����
{
    //���������� 1 ���� �������� ������
    bool operator()(ignList const &a,ignList const &b)const
    {
        return (a.from + a.to)<(b.from + b.to);
    }
};
///���������� ��������� map � �������������� hand-maid`������ ��������
std::map <string, int> arrClients;
std::map <ignList, int, sortIgn> arrIgnClients;


/*������� WSAStartup �������������� WinSock.
��� ������� ������ ����� ������ ��� ������ ������ � WinSock.
��� ��� ��� �������:

int WSAStartup (
    WORD wVersionRequested,
    LPWSADATA lpWSAData  );
������ �������� - ��� ������, ������� ����� ��������������.
    ������� ���� �������� ������, ������� ���� ���������� ������.
    ���� ������������� ����������, �� �������� ������� ��������.
    ������-�� ������������� ����������� � ������������� ������ ������ � ������� ������������ DLL � �������.
������ �������� - ��� ��������� �� ��������� WSADATA, � ������� ����������� ��������� �������������.

typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN+1];
    char szSystemStatus[WSASYS_STATUS_LEN+1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char FAR * lpVendorInfo;
} WSADATA, FAR * LPWSADATA;

�������������� ���� ���� szDescription � szSystemStatus ����� ������ ������� WSAStartup ��������,
��������������, �������� ���������� ���������� ���������� Windows Socket
� ������� ��������� ����� ���������� � ���� ��������� �����.
� ����� wVersion � wHighVersion ��������, ��������������,
������ ������������ Windows Socket, ������� ����� ������������ ����������,
� ������ ������������, ������� ������������� ���������� ���������� ���������� Windows Socket.
���������� ����� ������������ ��������� ��������� �������, ��������,
��� ������������� � ������ ���������� ������ ��������.
� ���� iMaxSockets �������� ������������ ���������� �������,
������� ����� �������� ��� ������ ��������.
� ���� iMaxUdpDg ������� ������������ ������ ������ ������,
������� ����� ��������� � �������������� ������������� ��������� UDP .
�, �������, ���� lpVendorInfo �������� ��������� �� �������������� ����������,
������ ������� ������� �� �����-������������ ���������� ���������� ������� Windows Sockets.

WSACleanup ��������� ������������� ������� DLL � ��������� ��������� � �������� WinSock.
    ��� ������� ���������� �������� ����.
*/


int main(int argc, char* argv[])
{
    SetTextColour(4);
    system("cls");
    setlocale(LC_ALL,"rus");
    string my_port_s; /// ���� ������� � ������
    int my_port; ///� ���� �����
    WSADATA g_wsadata;
    SetTextColour(4);
    printf("TCP SERVER DEMO\n");
    ///2,2 - ������ ������� �������
    ///WSAStartup - ������������� ����������  Windows Sockets
    if (WSAStartup( MAKEWORD( 2, 2 ), &g_wsadata )) /// 0 - �����
    {
        SetTextColour(4);
        ///WSAGetLastError - ��������� ��� ������
        printf("ERROR: Error WSAStartup %d\n", WSAGetLastError());
        return -1;
    }

    int mysocket;
    ///������� ������� ����� (����� ��� �������)
    /*�������� socket'�:

    int socket (domain, type, protocol)
        int domain;
        int type;
        int protocol;

    �������� domain ������ ������������ ��� �������������� ����� ����������
    (��� ���������������� �������), ��� ����� ���������� TCP/IP
    �� ������ ����� ���������� �������� AF_INET.

    �������� type ������ ����� ��������������:

    SOCK_STREAM - � ������������� ����������;
    SOCK_DGRAM - ��� ������������ ����������.
    �������� protocol ������ ���������� �������� ������������� ������
    (�� ���������� ��������� � ����� ����������). ���� ���� �������� ����� ������ 0,
    �� ����� ����������� �������� "�� ���������"
    (TCP ��� SOCK_STREAM � UDP ��� SOCK_DGRAM ��� ������������� ��������� ���������� TCP/IP).

    ��� ������� ���������� ����� ������ ������ ������� ����������
    ���������� socket'� - ����� ��������������� �����, ���������� ��� ����������������.

    ��� ����������� ������ � ���� ����� ������ ������� ���������� ����� "-1".*/

/// ���� � ��� ��� ���� ��� �������� �������, ������ ���)
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
    ///.c_str() ���������� ��������� �� ������ �������� + ����������� ����
    while(my_port>65535 or my_port<=0) ///������ ���� ���� �� ������ "�������"
    {
        SetTextColour(4);
        printf("ERROR: Input PORT number (0<PORT number<65535): ");
        std::cin>>my_port_s;
        my_port=atoi(my_port_s.c_str());
    }
    /*
    ���� sin_family ���������� ������������ ������ ������ (����� ����������),
    � ����� ������ (��� TCP/IP) ��� ������ ����� �������� AF_INET.
    ���� sin_addr �������� ����� (�����) ���� ����.
    ���� sin_port �������� ����� ����� �� ���� ����.
    ���� ���� sin_addr.s_addr ����� �������� INADDR_ANY,
    �� ��������� ����� ����� ����������� � socket'� ����� (�����) ���������� ���� ����.
    */
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(my_port);
    /*������� htons ������������ ������� ������ ��������� ����� �� ������� ����,
    ��������� �� ����������, � ������� ������� ����.*/
    local_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    /*������� htonl ������������ ������� ������ �������� ����� �� ������� ����,
    ��������� �� ����������, � ������� ������� ����.*/
    /*����� � ��� �� ����������, ���������� ���������, ��� �� ������ "n" ���������� ����������
    "network", �� "h" - "host" (��������������� ���������), "s" � "l" �������������� ��������
    (short) � ������� (long) ����������� �����, � "to" ���������� ��������������.
    ��������, "htons" ���������������� ���: "Host ( Network (short )"
    �.�. ������������� �������� ����� �� ������� ���������� ����� � ������� ������.
    */
    if (                            ///���������� ������ � ��������� ������
        bind(mysocket,              /// (in) ���������� ������
             (sockaddr *)&local_addr,    /// (in) ���������, ���������� ������ ����� � ����
             sizeof(local_addr))         /// (in) ������ ��������� local_addr
    )      ///� ������ ������ bind ���������� 0, � ��������� ������ - "-1".
    {
        SetTextColour(4);
        printf("ERROR: Error bind %d\n", WSAGetLastError());
        closesocket(mysocket);
        WSACleanup();
        return -1;
    }

    if (listen(
                mysocket,///���������� socket'� ����� ������� �������
                256)    ///������������ ����� ������� �������� �������� �� ������������ �����
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

    int client_socket; ///��������� ���������� ��� "����" �������
    sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
///accept() - ����� �������� �� �������� �� ������������ �����
    /*��������� ����� accept ��������� �� �������, �������������� ��������� ������� listen,
    ������ ������ �� ���������� � ���������� ���������� ������
    (������������� ����������) socket'� � ���� �� ����������, ��� � socket,
    ���������� ������ ����������. ���� ����� ���������� ���������� ������������
    �� ���� ����������� ��������� ������ ������� � ��������.

    ����� �������� ���������� accept ������ � ������ �������� ����� ��������� ���������������� �������
    ���� ������� �������� �� ������ ���������� accept �����, �� ��������� ���������
    � ��������� �������� ����������� �������� �� �������� �� �������������� �����.

    ��������� ���������� ���������� accept ������ ������������� ������������ ��������
    (���������� socket'� ������������� ���� �� �����).*/
///���������� ��� "�������" accept ->
    while (client_socket = accept(
                               mysocket, ///���������� socket'�, ����� ������� ���������-������ �������� ������
                               //�� ���������� (����������� ���������� ������� listen ).
                               (sockaddr *)&client_addr, ///��������� �� ������� ������, ������
                               //������� �������� �� ���������� � ��� ��������� ������,
                               //���������� ����� socket'� �������, ���������� ������ �� ����������.
                               //������� ������������� ���� ������� �� ���������.
                               &client_addr_size)///��������� �� ������� ������ � ���� ������ �����,
                           //��������� ������ (� ������) ������� ������, ����������� ���������� addr.
          )
    {
        nclients++;

        HOSTENT *hst; ///��������� hostent ������������ ���������, ����� ������� ���������� � ������: ��� ���, ���, IP �����, � �.�.
        hst = gethostbyaddr(///������� gethostbyaddr ���������� ���������� � ������� �� ��� IP ������
                  (char *)&client_addr.sin_addr.s_addr, /// ��������� �� ����� ���������� � ������� �������
                  4,  ///����� ������ � ������
                  AF_INET///��� ������ (������� �� ���������)
              );

        SetTextColour(2);
        printf("%sNew connect! => %s [%s] \n", //������� �� ����� �������
               myTime(),
               ( hst != NULL ) ? hst->h_name : "", ///��� �����
               inet_ntoa(client_addr.sin_addr) ///�� ��������� � ��������� �����
              );

        SetTextColour(4);
        if (nclients)
            printf("%d user on-line\n", nclients);
        else
            printf("No User on line\n");


        DWORD thID; ///Double word ������� �����
        struct client* cl = new client;
        cl->name = "";
        cl->socket = client_socket; ///"���������� ��� ���� �������" �� ��������� ������� accept
        cl->addr = inet_ntoa(client_addr.sin_addr); ///���� ���� IP �����
        ///������� ����� (����) ��� ���������� � ����������� �������� ������������ ��������
        CreateThread(NULL,  // ���������� ������ (����� �� ���������� ���� �������������)
                     0,              // ��������� ������ �����
                     ServClient,     // ������� ������
                     cl,             // �������� ������
                     0,              // ����� ��������
                     &thID           // ID ������ (�� ���� ��� �������, ��� ��������� ������ � ��������� ���)
                    );
    }

    WSACleanup(); ///������ �� ���� ���� accept ����������
    return 0; ///�������� main
}

DWORD WINAPI ServClient(LPVOID data) ///��������� ���������� ������
{
    ///�������� ���������� ��� � client ������ data � ���������� ��� � cl
    struct client* cl = reinterpret_cast<struct client*>( data );
    int my_sock = cl->socket; ///����������� ����� � ����� ��� � my_sock
    char buff[100]; ///������ ������
    int bytes_recv;
    bool isConn = false;
    string ip="["+cl->addr+"]";
    while ( (bytes_recv = recv( ///������ ������ �� ������.
                              ///������� ���������� ����� ��������� ������ ��� -1 � ������ ������.
                              ///������� �������� ������������� �� ���������� ���������� � ����� ���������-����������� ������.
                              my_sock,        ///�����-����������, �� �������� �������� ������
                              buff,           /// ����� ������ ��� ������ �������� ������
                              sizeof(buff) - 1,/// ����� ������ ��� ������ �������� ������
                              0)/* ���������� ������� ������, ����������� �������� ������.
        ���� �������� flags ����� ����, �� ��������� ������ ��������� �� ������.
        ���� �������� flags ���� MSG_PEEK, �� ������ �� ��������� � ����� ����
        ������� ���������� ������� ( ��� �������� ) recv
        */)
          )
    {
        if ( bytes_recv == SOCKET_ERROR )break;///���� ���� ������ �������� ���������

        buff[ bytes_recv ] = 0;
        istringstream sstr( buff );
        string command;
        sstr >> command;

        if(!isConn and !(command == "HELLO" or command == "HELP"))
        {

            echoes(my_sock,buff);///������ ������������ �������
            SetTextColour(4);
            ///%������� ����� ���� ��� ������� ����� ������� ��� ip �����: ���.c_str
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
        else if(name=="ALL" or name=="���" or name == "SERVER" or name == "������" or name == "server" or name == "������" )
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
            string msgAns = "#1"; ///���� ������ ������� ������� �� Hello
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
    char message[ 100 ]; ///������ ������
    string key="";
    string clientTo="";
    string tmp_str="";
    ignList ign_tmp;
    int sockTo=0;
    sstr >>clientTo; ///��������� �� ������� (��� �������)
///��� ���
   // cout<<clientTo<<" name cl \n";
    sstr.getline( message, sizeof( message ) - 1 );
///������ ���������� ����� ������
    if (  ( message[ strlen( message) - 1 ] == '\r' ) and ( strlen( message) != 0 ) )
        message[ strlen( message) - 1 ] = '\0';
///�������� � message ��� c �������
    istringstream mes_str( message );
    mes_str>>key;
///���� ���� � ������ ��� ���� �� - ������
    if(clientTo=="ALL")
    {
        if(key=="-p")
        {
            echoes(my_sock,buff); ///���������� ������� �� ��� ��������, ������� � ����������� � ����������
            SetTextColour(4); ///������ ���� ���������
            ///%����� %������ (���� ����� �� ���, ���� ���, �� ip)
            printf("%s ERROR: %s: %s\n",myTime(), cl->name=="" ? ip.c_str() : cl->name.c_str(), buff);
            string msgAns = "#5ERROR: You can't send private messages to all\n";
            send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
        }
        else
            key="-a"; ///������ ���� �������� ���������
    }
    else if (key=="-p") ///���� ���������
        {
            int len=strlen( message );
            int pointer=mes_str.tellg(); ///����. ��������� �� ������
            for (int i = 0; i<= len - pointer; i++ )
            {
                message[ i ] = message[ i+pointer ]; ///�������� ���������, ����� �� ���� ����
            }
        }
        else
        {
            key="-u"; ///�� ��������� (�������) - �� ��������� ����� ����
        }

    goodspace(message); ///������� �������

///���� ��� �����
    if ( clientTo.empty() )
    {
        echoes(my_sock,buff);
        SetTextColour(4);
        printf("%s ERROR: %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
        string msgAns = "#5ERROR: NameTo is empty\n";
        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
    }
    ///���� ��� ����������� � �������� ���������
    else if(clientTo==cl->name)
    {
        echoes(my_sock,buff);
        SetTextColour(4);
        printf("%s %s: %s\n",myTime(),cl->name==""?ip.c_str():cl->name.c_str(),buff);
        string msgAns = "#5ERROR: You can't send messages to yourself\n";
        send(my_sock, msgAns.c_str(), strlen( msgAns.c_str() ), 0);
    }
///���� ��������� ������
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
        ///����� ������ ����������
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
            if (maskpointer(clientTo))   ///���� � ����� ���� ����� (���������)
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
    case 1: //���� ��� ������������ ���������
        text=1;

        background=15;
        break;
    case 2: //���� ��� ������� ������ +connect
        text=2;
        //text=1;
        background=15;
        break;
    case 3: //���� ��� ����������� ����������
        text=10;
        //text=1;
        background=15;
        break;
    case 4: //���� ��� ����������� ������
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
    //������ ������������ �������
    goodspace(buff); ///������� ������� �� buff
    ///���������� ��� �� 1)����� 2) ����� 3)��������� 4)����� ������
    string qmsgAns = "#1";
    qmsgAns += myTime();
    qmsgAns += buff;
    qmsgAns += "\n";
    send(my_sock, qmsgAns.c_str(), strlen( qmsgAns.c_str() ), 0);
}

