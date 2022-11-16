#include <bits/stdc++.h>
#include <sys/time.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>

#include<netinet/in.h>
#include<arpa/inet.h>

#include <unistd.h>


using namespace std;
using namespace chrono;


struct data_packet {
    __uint16_t senderID;
    __uint32_t packet_num;

    __uint64_t time_since_epoch_micro;

    __int32_t payload[10];
}; // sizeof(PACKET) should be 56
typedef struct data_packet PACKET;


int main(int argc, char** argv)
{
    if (argc < 4) {
        cout << "the format of parameters should be:[the_filename_of_this.run] destination_IP_address destination_IP_port sender_ID " << endl;
        exit(EXIT_FAILURE);
    }
    cout << endl;

    char* dest_ip = argv[1];
    int port = atoi(argv[2]);
    int sender_ID = atoi(argv[3]);


    PACKET packet;
    packet.senderID = sender_ID;
    for (int i = 0; i < 10; i++)    packet.payload[i] = 9000+i;


    int brdcFd;
    while ((brdcFd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        fprintf(stderr,"socket fail, errno=%d, %s\n", errno, strerror(errno));
        continue;
    }
    int optval = 1; 
    setsockopt(brdcFd, SOL_SOCKET, SO_BROADCAST | SO_REUSEPORT, &optval, sizeof(int));
    struct sockaddr_in theirAddr;
    memset(&theirAddr, 0, sizeof(struct sockaddr_in));
    theirAddr.sin_family = AF_INET;
    theirAddr.sin_addr.s_addr = inet_addr(dest_ip);
    theirAddr.sin_port = htons(port);


    // __uint64_t microseconds_since_epoch = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();



    for (packet.packet_num = 0;; packet.packet_num++) {
        int sendBytes;
        
        packet.time_since_epoch_micro = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

        if ((sendBytes = sendto(brdcFd, (char*)&packet, sizeof(packet), 0, (struct sockaddr*)&theirAddr, sizeof(struct sockaddr))) == -1) {
            fprintf(stderr,"sendto fail, errno=%d, %s\n", errno, strerror(errno));
            continue;
        }

        cout << "send " << packet.packet_num << "\t" << sendBytes << endl; 

        usleep(1e4);  // 10ms
    }


    close(brdcFd);

    return 0;
}