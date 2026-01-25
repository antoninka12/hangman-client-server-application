#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include                <unistd.h>
#include        <time.h>                /* old system? */
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include "discovery_server.h"
#include "new_clients.h"
#include "handle_client.h"
//#include <strings.h> jakby krzyczaolo na bzero

#include <signal.h> //obsuga sigchild - juz nie potrzebna, chyba ze do multicastu ale to id
#include <sys/wait.h> //obsuga sigchild

#include <ctype.h> //do liter i gry, zmiana wielkosci np
#include <poll.h>

#define LISTEN 10
#define MAXEVENTS 2000 

//dotad ok

int main(int argc, char **argv)
{
        int desc1, desc2;
        socklen_t len;
        struct sockaddr_in6 servaddr, cliaddr;

        //do poll
        struct pollfd client[MAXEVENTS];
        int i, maxi, nready;


        clients_init();

        /* socket */
        if ((desc1 = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
                perror("socket");
                return 1;
        }

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin6_family = AF_INET6;
        servaddr.sin6_addr   = in6addr_any;
        servaddr.sin6_port   = htons(1234);

        if (bind(desc1, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
                perror("bind");
                return 1;
        }

        if (listen(desc1, LISTEN) < 0) {
                perror("listen");
                return 1;
        }

        /* multicast discovery – BEZ ZMIAN */
        if (fork() == 0) {
                multicast_discovery_server();
                exit(0);
        }

        /* poll init */
        client[0].fd = desc1;
        client[0].events = POLLIN;

        for (i = 1; i < MAXEVENTS; i++)
                {client[i].fd = -1;}

        maxi = 0;

        while (1) {
                nready = poll(client, maxi + 1, -1);
                if (nready < 0) {
                        if (errno == EINTR) continue;
                        perror("poll");
                        exit(1);
                }

                /* nowe połączenie */
                if (client[0].revents & POLLIN) {
                        len = sizeof(cliaddr);
                        desc2 = accept(desc1, (struct sockaddr*)&cliaddr, &len);

                        if (desc2 >= 0) {
                                for (i = 1; i < MAXEVENTS; i++) {
                                        if (client[i].fd < 0) {
                                                client[i].fd = desc2;
                                                client[i].events = POLLIN;
                                                client_add(desc2);
                                                if (i > maxi) maxi = i;
                                                break;
                                        }
                                }
                        }
                        if (--nready <= 0)
                                continue;
                }

                /* obsługa klientów */
                for (i = 1; i <= maxi; i++) {
                        if (client[i].fd < 0)
                                continue;

                        if (client[i].revents & POLLIN) {
                                handle_client_input(client[i].fd);
                                if (--nready <= 0)
                                break;
                        }
                        }
        }
}
