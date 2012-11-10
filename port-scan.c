/*
* Copyright (C) 2012, William H. Welna All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY William H. Welna ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL William H. Welna BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Example code for a very basic Win32 async port scanner */

#define Socket SOCKET
#define _WIN32_WINNT 0x0501

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Winbase.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void print_peer(Socket sock, char *port) {
	struct sockaddr_in p;
	int p_len = sizeof(struct sockaddr_in);
	struct in_addr addr;
	struct hostent *info;
	if(getpeername(sock, &p, &p_len)!=SOCKET_ERROR) {
		if((info=gethostbyaddr((char *)&p.sin_addr, 4, AF_INET))!=NULL) {
			fprintf(stdout, "OPEN: %s (%s) on port %s\n", inet_ntoa(p.sin_addr), info->h_name, port);
		}
	}
}

int ipv4_scanner(char **ips, char *port) {
	struct addrinfo hints, *servinfo=NULL, *p=NULL;
	Socket sock, *k;
	fd_set fd_master, fd_read, fd_write, fd_errors;
	int r=0, x, y=1, global=0, left=0;
	struct timeval t;
	char b[8];
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	FD_ZERO(&fd_master); FD_ZERO(&fd_write); FD_ZERO(&fd_read); FD_ZERO(&fd_errors);
	for(x=0; ips[x] != NULL; ) // Count how many ips passed
		x++;
	k=calloc(x, sizeof(Socket));
	if(!k)
		return 0;
	for(x=0; ips[x] != NULL; x++) {
		if((r=getaddrinfo(ips[x], port, &hints, &servinfo))!=0) {
			free(k);
			return 0;
		}
		for(p=servinfo; p != NULL; p=p->ai_next) {
			if((sock = WSASocket(p->ai_family, p->ai_socktype, p->ai_protocol, NULL, 0, 0)) == INVALID_SOCKET) {
			continue;
			}
			ioctlsocket(sock, FIONBIO, &y);
			if(!connect(sock, p->ai_addr, p->ai_addrlen)) {
				print_peer(sock, port);
				//I has Instant Connect
			}
			FD_SET(sock, &fd_master);
			k[global] = sock; ++global;
		}
		if(servinfo)
			freeaddrinfo(servinfo);
	}
	t.tv_sec = 8; // 8 second timeout
	t.tv_usec = 0;
	left=global;
	while(left > 0) {
		fd_write = fd_read = fd_errors = fd_master;
		if((r=select(0, &fd_read, &fd_write, &fd_errors, &t))!=SOCKET_ERROR) {
			// Check for successful conects
			for(x=0; x < global; x++) {
				if(FD_ISSET(k[x], &fd_write)) {
					// Successful Connect
					print_peer(k[x], port);
					closesocket(k[x]);
					FD_CLR(k[x], &fd_master);
					--left;
				}
				if(FD_ISSET(k[x], &fd_read)) {
					if((y=recv(k[x], b, 8, 0))==0) { // Socket Closed
						closesocket(k[x]);
					} else if(y < 0) { // Socket Error
						closesocket(k[x]);
					} else { // Got some data
						print_peer(k[x], port);
						closesocket(k[x]);
					}
					FD_CLR(k[x], &fd_master);
					--left;
				}
				if(FD_ISSET(k[x], &fd_errors)) {
					// Error
					closesocket(k[x]);
					FD_CLR(k[x], &fd_master);
					--left;
				}
			}	
		} else {
			break;
		}
	}
	free(k);
	return 1;
}

char *addip_arry(char *ip) {
	char *r=calloc(18, sizeof(char));
	strncpy(r, ip, strlen(ip));
	return r;
}

void free_arry(char **i, int size) {
	int x;
	for(x=0; x < size; x++)
		free(i[x]);
	free(i);
}

int main(int argc, char **argv) {
	char **p = calloc(7, sizeof(char **));
	int x;
	WSADATA wsainfo;
	WSAStartup(0x22, &wsainfo);
	p[0]=addip_arry("127.0.0.1");
	p[1]=addip_arry("74.125.227.144"); // google ip addresses
	p[2]=addip_arry("74.125.227.145");
	p[3]=addip_arry("74.125.227.146");
	p[4]=addip_arry("74.125.227.147");
	p[5]=addip_arry("74.125.227.148");
	p[6]=NULL;
	if(ipv4_scanner(p, "80")) {
		fprintf(stdout, "Finished...\n");
	} else {
		fprintf(stderr, "Something Just Went Wrong...");
	}
	free_arry(p, 7);
	WSACleanup();
	return 0;
}
