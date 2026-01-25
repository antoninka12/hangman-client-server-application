#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "new_clients.h"
void handle_client_input(int desc2)
{
    char buf[4096];
    ssize_t n;
    char username[32]; //LOGIN
    char letter; //litera do GUESS
    char extra; //do sprawdzania czy jest wiecej niz jedna litera w GUESS

    n = read(desc2, buf, sizeof(buf) - 1); //odczyt
   if (n <= 0) { //zamiast while
    if (n < 0)
        perror("read");
    close(desc2);
    client_remove(desc2);
    return;
}

    buf[n] = '\0'; //zerowanie
    char *newline = strchr(buf, '\n');//szukamy znaku nowej linii
    if (newline) *newline = '\0'; //jesli znajdziemy to zmieniamy na null

    struct client *c = client_get(desc2); //tworzymy strukture, ktora ma zapisanych aktualnych kleintow
    if (!c) return; //jesli by bylo puste to wroc

    //logika logowania:
    if (strncmp(buf, "LOGIN", 5) == 0) {
        if (c->logged_in){
            write(desc2, "ERROR already logged in\n", 24);
            //continue;
        } //sprawdzamy czy w strukturze juz jest zalogowwany
            
        else if (sscanf(buf, "LOGIN %31s", username) != 1){ //sprawdza czy pasuje do formatu, jak nie to blad
             write(desc2, "ERROR invalid login\n", 21);
             //continue;
        }
           
        else if (username_taken(username)) //sprawdzamy czy wolny login, funkcja z new_clients
            {write(desc2, "ERROR login taken\n", 19);
                //continue;
            }
        else if (client_login(desc2, username) < 0){ //funkcja z new_client
            write(desc2, "ERROR login failed\n", 19);
            //continue;
        }
            
        else
            write(desc2, "OK LOGIN\n", 9); //ok
    } //tu tylko sprawdzanie, add w main

    else if (strcmp(buf, "JOIN") == 0) {
        if (!c->logged_in)
            {write(desc2, "ERROR: not logged in\n", 22);
                //continue;
            }
        else if (client_set_ready(desc2) < 0){
            write(desc2, "ERROR: tu bedzie czy w grze czy nie \n", 35);
            //continue;
        }
            
        else
            write(desc2, "WAIT: joining...\n", 18);
    }

    else if (strncmp(buf, "GUESS ", 6) == 0) {
        if (!c->logged_in){
            write(desc2, "ERROR: not logged in\n", 22);
            //continue;
        } 
       /*else if (!c->in_game){
            write(desc2, "ERROR: not in game\n", 20);
            //continue; //TO TRZEBA BEDZIE ALE TO W GRZE
        }*/ 
        else if (sscanf(buf, "GUESS %c %c", &letter, &extra) > 1){
            write(desc2, "TOO MANY LETTERS\n", 17);
            //continue;
        }  
        else if (sscanf(buf, "GUESS %c", &letter) == 1) {
            if (!isalpha((unsigned char)letter)){
                write(desc2, "ERROR: must be a letter\n", 25);
            }
                
            else {
                letter = tolower((unsigned char)letter);
                write(desc2, "ERROR: game not implemented\n", 29);
            }
        } else
            write(desc2, "ERROR invalid guess\n", 21);
    }

    else {
        write(desc2, "ERROR unknown command\n", 22);
    }
}