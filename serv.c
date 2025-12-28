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
//#include <strings.h> jakby krzyczaolo na bzero

#include <signal.h> //obsuga sigchild
#include <sys/wait.h> //obsuga sigchild


#define LISTEN 10 //ile klientw ma czekac w kolejce

void sigchild(int s){
        (void)s; /*zawsze kernel przekazuje numer sygnau jako argument alr my go nie 
        uzywamy*/ 
        while(waitpid(-1, NULL, WNOHANG)>0){} 
        /*
        -1-zabierz dowolne dziecko
        NULL- nie interesuje nas kod zakoczenia dziecka
        WHOHANG-jak nie ma dzieci do zabrania wr od razu
        */

}

int main(int argc, char **argv){
        int desc1, desc2; //desktyptory
        socklen_t len; //rozmiar adresu klienta
        struct sockaddr_in6 servaddr, cliaddr; //do struktury adresowej
        char buf[4096]; //bufor na dane
        ssize_t n;

        //socket
        if((desc1=socket(AF_INET6, SOCK_STREAM, 0))<0){
                perror("socket: ");//komunikat o bledzie
                return 1;
        }
         
        //struktura adresowa
        bzero(&servaddr, sizeof(servaddr)); //czyszczenie struktury adresowej
        servaddr.sin6_family =AF_INET6;
        servaddr.sin6_addr = in6addr_any;
        servaddr.sin6_port= htons(13);

        //bind
        if(bind(desc1, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
                perror("bind: ");
                return 1;
        }
         
        //listen
        if(listen(desc1, LISTEN)<0){
                perror("listen: ");
                return 1;
        }

        //sigaction 
        struct sigaction sigact;
        bzero(&sigact, sizeof(sigact));
        sigact.sa_handler=sigchild; //odwolanie do voida na gorze do obslugi waitpid
        sigemptyset(&sigact.sa_mask); //przy wykonywaniu hendlera nie blokuje dod. syg.
        sigact.sa_flags=SA_RESTART; //jesli sygnal przerwie accept to system ma je wzmowic
        sigaction(SIGCHLD, &sigact, NULL); //sigaction

        while(1){
                len=sizeof(cliaddr); //rozmiar bufora do ktrego wpiszemy adres klienta

                if((desc2=accept(desc1, (struct sockaddr*)&cliaddr, &len))<0){
                        if (errno == EINTR) continue; //Jakby nastapil Sigchild w trakcie
                        perror("accept: ");
                        continue; //jeli bd to idz na pocztek ptli
                }

                if(fork()==0){
                        close(desc1);
                        /*Na razie robie echo*/
                        while((n=read(desc2, buf, sizeof(buf)))>0){
                                write(desc2, buf, n); //odeslij to co jest w buforze
                        }
 
                        if(n<0){
                                perror("read: ");
                                return 1;
                        }

                        /*Koniec echo*/
                        close(desc2);
                        exit(0);
                }
                close(desc2);
        }

}
