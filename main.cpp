#include <bits/stdc++.h>
#include <sys/time.h>

using namespace std;

struct data_packet {
    __uint16_t senderID;
    __uint32_t packet_num;

    __uint64_t tv_sec;		/* Seconds.  */
    __uint64_t tv_usec;	/* Microseconds.  */ // __SLONGWORD_TYPE	long int

    __int32_t payload[10];
}; // sizeof(PACKET) should be 64

typedef struct data_packet PACKET;

struct timeval *current_time;

int main(int argc, char** argv)
{
    if (argc < 4) {
        cout << "the format of parameters should be:[the_filename_of_this.run] destination_IP_address destination_IP_port sender_ID " << endl;
        exit(EXIT_FAILURE);
    }

    char* dest_ip = argv[1];
    int port = atoi(argv[2]);
    int sender_ID = atoi(argv[3]);


    PACKET packet;
    packet.senderID = sender_ID;
    for (int i = 0; i < 10; i++)    packet.payload[i] = INT_MAX;


    gettimeofday(current_time,NULL);




    return 0;
}