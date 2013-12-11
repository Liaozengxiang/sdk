#include "cstringext.h"
#include "sys/sock.h"
#include "sys/system.h"
#include "time64.h"
#include <stdio.h>

static char reply[128];

int main(int argc, char* argv[])
{
	socket_t tcp;
	const char *echo = "hello server";
	const char *host = "127.0.0.1";
	int port = 2012;
	int work = 10000;
	int i, r, n;
	time64_t lt, lt1;

	for(i = 1; i < argc; i++)
	{
		if(0 == strcmp("-h", argv[i]))
		{
			if(i + 1 >= argc) exit(1);
			host = argv[++i];
		}
		else if(0 == strcmp("-p", argv[i]))
		{

			if(i + 1 >= argc) exit(1);
			port = atoi(argv[++i]);
		}
		else if(0 == strcmp("-w", argv[i]))
		{
			if(i+1 >= argc) exit(1);
			work = atoi(argv[++i]);
		}
	}

	socket_init();
	tcp = socket_tcp();
	r = socket_connect_ipv4(tcp, host, (unsigned short)port);
	if(r < 0)
	{
		printf("socket connect %s:%d error: %d/%d\n", host, port, r, socket_geterror());
		return 1;
	}

	n = strlen(echo);
	lt = time64_now();
	for(i = 0; i < work; i++)
	{
		r = socket_send(tcp, echo, n, 0);
		if(r < 0)
		{
			printf("socket send[%d] error: %d/%d\n", i, r, socket_geterror());
			return 1;
		}

		r = socket_recv(tcp, reply, sizeof(reply), 0);
		if(r < 0)
		{
			printf("socket receive[%d] error: %d/%d\n", i, r, socket_geterror());
			return 1;
		}

		system_sleep(2000);
	}
	
	lt1 = time64_now();
	printf("tcp[%s:%d] send/recv %d time: %ll\n", host, port, work, lt1-lt);

	socket_close(tcp);
	socket_cleanup();
	return 0;
}
