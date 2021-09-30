Linux:
```bash
gcc stun.c stun_get_addr.c
```

Windows (mingw):
```bash
gcc stun.c stun_get_addr.c -l ws2_32
```


then
```
./a 173.194.222.127 19302 4233
```

implementation (include stun_get_addr.c in your project):
```c
int stun_get_addr(char *stun_server_ip, short stun_server_port, short local_port, char *return_ip, short *return_port);
```

example:
```c
char return_ip[50];
short return_port;
if (stun_get_addr("173.194.222.127", 19302, 4233, return_ip, &return_port) == 0) {
	printf("%s\n", return_ip);
} else {
	printf("some error\n");
}
```
