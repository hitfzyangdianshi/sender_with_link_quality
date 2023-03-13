#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <chrono>


#include<sys/types.h>
#include <WinSock2.h>

#include <windows.h>

#pragma warning(disable : 4996)

using namespace std;
#pragma comment(lib,"WS2_32.lib")



using namespace std;
using namespace chrono;


#define __uint16_t uint16_t 
#define __uint32_t uint32_t 
#define __uint64_t uint64_t
#define __int32_t int32_t 
#define __int64_t int64_t 


void usleep(__int64 usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}


struct data_packet {
    __uint16_t senderID;
    __uint32_t packet_num;

    __uint64_t time_since_epoch_micro;

    __int32_t payload[1000];
};
typedef struct data_packet PACKET;


int main(int argc, char** argv)
{
    if (argc < 6) {
        cout << "the format of parameters should be:[the_filename_of_this.run] destination_IP_address destination_IP_port sender_ID frequency numbers_of_packets_you_want_to_send(0=infinite) " << endl;
        exit(EXIT_FAILURE);
    }
    cout << endl;

    char* dest_ip = argv[1];
    int port = atoi(argv[2]);
    int sender_ID = atoi(argv[3]);
    long preset_frequency = atoi(argv[4]); // 10 000 for 10ms, 0.01s
    long numbers_of_packets_sent = atoi(argv[5]);

    PACKET packet;
    packet.senderID = sender_ID;
    for (int i = 0; i < 10; i++)    packet.payload[i] = 9000 + i;


    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);


    int brdcFd;
    while ((brdcFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        fprintf(stderr, "socket fail, errno=%d, %s\n", errno, strerror(errno));
        continue;
    }
    int optval = 1;
    // setsockopt(brdcFd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int));
    struct sockaddr_in theirAddr;
    memset(&theirAddr, 0, sizeof(struct sockaddr_in));
    theirAddr.sin_family = AF_INET;
    theirAddr.sin_addr.s_addr = inet_addr(dest_ip);
    theirAddr.sin_port = htons(port);


    // __uint64_t microseconds_since_epoch = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();


    if (numbers_of_packets_sent == 0) {
        for (packet.packet_num = 0;; packet.packet_num++) {
            int sendBytes;

            packet.time_since_epoch_micro = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

            if ((sendBytes = sendto(brdcFd, (char*)&packet, sizeof(packet), 0, (struct sockaddr*)&theirAddr, sizeof(struct sockaddr))) == -1) {
                fprintf(stderr, "sendto fail, errno=%d, %s\n", errno, strerror(errno));
                continue;
            }

            cout << "send " << packet.packet_num << "\t" << sendBytes << "\t" << packet.time_since_epoch_micro << endl;
            //   printf("send %d\t%d\t%lu\n", packet.packet_num, sendBytes, packet.time_since_epoch_micro);

               //usleep(1e4);  // 10ms
            usleep(preset_frequency);
        }
    }
    else {
        for (packet.packet_num = 0; packet.packet_num < numbers_of_packets_sent; packet.packet_num++) {
            int sendBytes;

            packet.time_since_epoch_micro = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

            if ((sendBytes = sendto(brdcFd, (char*)&packet, sizeof(packet), 0, (struct sockaddr*)&theirAddr, sizeof(struct sockaddr))) == -1) {
                fprintf(stderr, "sendto fail, errno=%d, %s\n", errno, strerror(errno));
                continue;
            }

            cout << "send " << packet.packet_num << "\t" << sendBytes << "\t" << packet.time_since_epoch_micro << endl;
            //   printf("send %d\t%d\t%lu\n", packet.packet_num, sendBytes, packet.time_since_epoch_micro);

               //usleep(1e4);  // 10ms
            usleep(preset_frequency);
        }
    }


    closesocket(brdcFd);
    WSACleanup();

    return 0;
}