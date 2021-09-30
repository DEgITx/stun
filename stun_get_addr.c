#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#ifdef _WIN32
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
/*
 *
 *  STUN(RFC5389)
 */

int stun_get_addr(char *stun_server_ip, short stun_server_port, short local_port, char *return_ip, short *return_port)
{
	struct sockaddr_in servaddr;
	struct sockaddr_in localaddr;
	unsigned char buf[200];
	int sockfd, i;
	unsigned char bindingReq[20];

	int stun_method, msg_length;
	short attr_type;
	short attr_length;
	short port;
	short n;

#ifdef _WIN32
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	//# create socket 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);	// UDP

	// server 
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
#ifdef _WIN32
	servaddr.sin_addr.s_addr = inet_addr(stun_server_ip);
#else
	inet_pton(AF_INET, stun_server_ip, &servaddr.sin_addr);
#endif
	servaddr.sin_port = htons(stun_server_port);

	// local
	bzero(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(local_port);

	n = bind(sockfd, (struct sockaddr *) &localaddr, sizeof(localaddr));
    if (n != 0)
    {
        return -1;
    }

	//## first bind 
	*(short*)(&bindingReq[0]) = htons(0x0001);	// stun_method
	*(short*)(&bindingReq[2]) = htons(0x0000);	// msg_length
	*(int*)(&bindingReq[4]) = htonl(0x2112A442);	// magic cookie

	*(int*)(&bindingReq[8]) = htonl(0x63c7117e);	// transacation ID 
	*(int*)(&bindingReq[12]) = htonl(0x0714278f);
	*(int*)(&bindingReq[16]) = htonl(0x5ded3221);

	n = sendto(sockfd, bindingReq, sizeof(bindingReq), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));	// send UDP
	if (n == -1)
	{
		return -2;
	}

    time_t dur = time(NULL);
    n = -1;
    while (n == -1 && time(NULL) - dur < 2)
    {
        n = recvfrom(sockfd, buf, 200, 0, NULL, 0);	// recv UDP
        //Sleep(50);
        usleep(50 * 1000);
    }
    if (n == -1)
	{
		return -3;
	}

	if (*(short*)(&buf[0]) == htons(0x0101))
	{
		// parse XOR
		n = htons(*(short*)(&buf[2]));
		i = 20;
		while (i < sizeof(buf))
		{
			attr_type = htons(*(short*)(&buf[i]));
			attr_length = htons(*(short*)(&buf[i + 2]));
			if (attr_type == 0x0020)
			{
				// parse : port, IP 
				port = ntohs(*(short*)(&buf[i + 6]));
				port ^= 0x2112;

				sprintf(return_ip, "%d.%d.%d.%d", 
                    buf[i + 8] ^ 0x21, 
                    buf[i + 9] ^ 0x12, 
                    buf[i + 10] ^ 0xA4, 
                    buf[i + 11] ^ 0x42
                );
                *return_port = port;

				break;
			}

			i += (4 + attr_length);
		}
	}

	close(sockfd);

	return 0;
}