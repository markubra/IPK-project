/*
* Author: Marko Kubrachenko (xkubra00)
* Date: 02.22
* Project: IPK http server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>

void get_hostname(FILE *p, char *answer)
{
	p = popen("cat /proc/sys/kernel/hostname", "r");
	char ch;
	int i = 0;
	while((ch = fgetc(p)) != EOF)
	{
		answer[i] = ch;
		i++;				
	}
	pclose(p);
}

void get_cpuname(FILE *p, char *buffer)
{
	p = popen("cat /proc/cpuinfo | head -n 5 | tail -n 1 | cut -c 14-", "r");
	char ch;
	int i = 0;
	while((ch = fgetc(p)) != EOF)
	{
		buffer[i] = ch;
		i++;
	}
	pclose(p);
}

void get_cpu_load(FILE *p, char *buffer)
{
	p = popen("cat /proc/stat | head -n 1 | cut -c 6-", "r");
	unsigned long long int stat1[10];
	for (int j = 0; j < 10; j++)
	{
		fscanf(p, "%llu", &stat1[j]);
	}
	pclose(p);
	
	sleep(1);
	
	p = popen("cat /proc/stat | head -n 1 | cut -c 6-", "r");
	unsigned long long int stat2[10];
	for (int j = 0; j < 10; j++)
	{
		fscanf(p, "%llu", &stat2[j]);
	}
	pclose(p);
	
	unsigned long long int prev_idle = stat1[3] + stat1[4];
	unsigned long long int idle = stat2[3] + stat2[4];
	
	unsigned long long int prev_non_idle = stat1[0]	+ stat1[1] + stat1[2] + stat1[5] + stat1[6] + stat1[7];
	unsigned long long int non_idle = stat2[0] + stat2[1] + stat2[2] + stat2[5] + stat2[6] + stat2[7];
	
	unsigned long long int prev_total = prev_idle + prev_non_idle;
	unsigned long long int total = idle + non_idle;

	long long int total_diff = total - prev_total;
	long long int idle_diff = idle - prev_idle;	
	
	int CPU_load = round((((double)(total_diff - idle_diff)) / ((double)total_diff)) * 100);
	sprintf(buffer, "%d%%\n", CPU_load);
}

int main(int argc, char *argv[])
{
	FILE *p;
  	char buffer[1024] = {0};

	int server, new_socket, port;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}
	else
	{
		fprintf(stderr, "%s", "You need to specify a port.\n");
		return -1;
	}

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == -1)
	{
		fprintf(stderr, "Unable to create a socket.\n");
		return -1;
	}

	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
		
		fprintf(stderr, "Unable to set socket's options.\n");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(server, (struct sockaddr *) &address, sizeof(address)) != 0) 
	{
		fprintf(stderr, "Unable to bind a socket.\n");
		return -1;	
	}

	if (listen(server, 10) == -1)
	{
		fprintf(stderr, "Unable to start listening to.\n");
		return -1;
	}
	
	while (1)
	{
		printf("Connection...\n\n");

		new_socket = accept(server, (struct sockaddr *) &address, (socklen_t *) &addrlen);
		read(new_socket, buffer, 1024);
		printf("%s\n", buffer);
		
		char header[1024] = {0};
		char answer[1024] = {0};
		
		strcpy(header, "HTTP/1.1 200 OK\r\nContent-Type:text/plain;\r\n\r\n");
		
		if (strncmp(buffer, "GET /hostname ", 14) == 0)
		{
		  	get_hostname(p, answer);
		}
		else if (strncmp(buffer, "GET /cpu-name ", 14) == 0)
		{
			get_cpuname(p, answer);
		}
		else if (strncmp(buffer, "GET /load ", 10) == 0)
		{
			get_cpu_load(p, answer);
		}
		else
		{
			strcpy(header, "HTTP/1.1 404 Not Found\r\nContent-Type:text/plain;\r\n\r\n");
			strcpy(answer, "404 Not Found\n");
		}
		
		strcat(header, answer);
		
		write(new_socket, header, strlen(header));
		printf("Sent.\n\n");
		close(new_socket);				
	}
	close(server);
	return 0;
}
