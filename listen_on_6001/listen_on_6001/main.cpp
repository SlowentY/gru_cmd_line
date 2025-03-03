
//udp echo (+rx +tx)

#include <iostream>

using namespace std;

//!!! #include "winsock2.h" //D:\Program Files (2)\CodeBlocks\MinGW\x86_64-w64-mingw32\lib\libws2_32.a

#include "../../src/PassiveSocket.h" // Include header for active socket object definition

//#define MAX_PACKET 4096
//constexpr uint32 MAX_PACKET{4096};
constexpr size_t MAX_PACKET{4096};

int main(int argc, char **argv)
{

  uint8 buf[MAX_PACKET] ;

    CPassiveSocket socket(CSimpleSocket::CSocketType::SocketTypeUdp);
    //CActiveSocket Client(CSimpleSocket::CSocketType::SocketTypeUdp) ;

    //CSimpleSocket socket(CSimpleSocket::CSocketType::SocketTypeUdp); //error: 'class CSimpleSocket' has no member named 'Listen'|

    cout << "starting" << endl;

    // Initialize our socket object
    //socket.Initialize();
   cout << " socket.Initialize() = " << socket.Initialize() << endl;

   //cout << " = " << << endl;

    if (!socket.Listen("192.168.100.3", 6001))
    {
        std::cerr << socket.DescribeError() << std::endl;
        return 1;
    } //if

    cout << "listening" << endl;

    while (socket.IsSocketValid())
    {
        //if ((pClient = socket.Accept()) != NULL)
       // if ((Client.O) != NULL)

       cout << "socket.Receive() = " << socket.Receive(MAX_PACKET, buf) << endl;
       std::cout << socket.DescribeError() << std::endl;

       cout << "socket.GetBytesReceived() = " << socket.GetBytesReceived() << endl;

//       for(int32 ii=0; ii<socket.GetBytesReceived(); ii++)
//       {
//           cout << " buf[" << ii << "] = " << buf[ii] << " " << endl;
//       } //for

       cout << "socket.Send() = " << socket.Send(buf, socket.GetBytesReceived()) << endl;
       std::cout << socket.DescribeError() << std::endl;
    } //while

    socket.Close();

    cout << "closed" << endl;

    return 1;
} //main
