#include "new_clients.h"
#include <string.h>

/*struct client {
    int fd;                         // deskryptor gniazda
    int used;                       
    int logged_in;                  
    int ready;                      
    char username[USERNAME_LEN];    
};*/
static struct client clients[MAX_CLIENTS];

void clients_init(void){
   memset(clients, 0, sizeof(clients));
}
int client_add(int fd){
     for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].used) {
            clients[i].used = 1;
            clients[i].fd = fd;
            clients[i].logged_in = 0;
            clients[i].ready = 0;
            clients[i].username[0] = '\0';
            return 0;   // OK
        }
    }
    return -1;
}

void client_remove(int fd){
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].used && clients[i].fd==fd) {
            clients[i].used = 0;
            clients[i].fd = -1;
            clients[i].logged_in = 0;
            clients[i].ready = 0;
            clients[i].username[0] = '\0';
        }
    }
}
struct client *client_get(int fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].used && clients[i].fd == fd) {
            return &clients[i];
        }
    }
    return 0;
}
int username_taken(const char *username){
       for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].used &&
            clients[i].logged_in &&
            strcmp(clients[i].username, username) == 0) { //porownuje czy sa takie same
            return 1;   // zajęty
        }
    }
    return 0; //wolny
}

int client_login(int fd, const char *username)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].used && clients[i].fd == fd) {

            if (strlen(username) >= USERNAME_LEN){
                return -1; // za długi login
            }
            strncpy(clients[i].username, username, USERNAME_LEN - 1);
            clients[i].username[USERNAME_LEN - 1] = '\0';
            clients[i].logged_in = 1;
            clients[i].ready = 0;
            return 0;
        }
    }
    return -1;
}
void client_logout(int fd){
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].used&&clients[i].fd==fd){
            memset(clients[i].username, 0, sizeof(clients[i].username));
            clients[i].logged_in = 0;
            clients[i].ready = 0;
            return;
        }
    }
}

int client_set_ready(int fd)
{
    for (int i = 0; i< MAX_CLIENTS; i++) {
        if (clients[i].used&&
            clients[i].fd==fd&&
            clients[i].logged_in) {
            clients[i].ready=1;
            return 0;   
        }
    }
    return -1;          
}