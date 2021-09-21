#include <stdio.h>
#include <stdlib.h>

int stun_get_addr(char *stun_server_ip, short stun_server_port, short local_port, char *return_ip, short *return_port);

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("STUN(RFC5389) client demo by Chris<nodexy@gmail> and Alexey Kasyanchuk <degitx@gmail.com>\n");
		printf("Usage: %s<server_ip><server_port><local_port>\n\n", argv[0]);
		exit(1);
	}

	printf("Main start ... \n");

	int n = 0;
	char return_ip[50];
    	short return_port;
	n = stun_get_addr(argv[1], atoi(argv[2]), atoi(argv[3]), return_ip, &return_port);
	if (n != 0)
		printf("STUN req error : %d\n", n);
	else
		printf("ip:port = %s:%d\n", return_ip, return_port);

	printf("Main over.\n");

}
