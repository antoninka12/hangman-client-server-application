/*
struct game {
    int active;                 // czy gra trwa
    int player1_fd;             // deskryptor 1. gracza
    int player2_fd;             // deskryptor 2. gracza

    char word[MAX_WORD_LEN];    // hasło
    char guessed[MAX_WORD_LEN]; // aktualny stan (_ a _ _)
    int wrong_guesse;          // liczba błędów
};*/

#include "game.h"
#include "tlv.h"
#include "protocol.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


static struct game games; //pojedyncza gra 

static const char *word_list[] = {
    "gra",
    "programowanie", 
    "sieci",
    "myszka",
    "drzewo",
    "papier",
    "klawisz",
    "kursor",
    "kubek",
    "lustro",
    "telefon"
};

void game_init(void) {
    memset(&games, 0, sizeof(games));
    srand(time(NULL)); //inicjalizacja rand
}

int start_game(int desc2){

    if(games.active){ //jesli trwa inna gra
        sendtlv(desc2, TLV_MSG, "Wait: game already in progress\n", 32);
        
    }

    if(games.player1_fd==0){
        games.player1_fd=desc2;
        sendtlv(desc2, TLV_MSG, "Wait: waiting for another player...\n", 36);
        return 0;
    }
    else if(games.player2_fd==0 && desc2 != games.player1_fd){
        games.player2_fd=desc2;
       
        //wybór słowa
          int index = rand() % (sizeof(word_list) / sizeof(word_list[0]));
        const char *selected_word = word_list[index];
        strncpy(games.word, selected_word,MAX_WORD_LEN- 1);  //kopiowanie slowa MAX_WORD_LEN ????????
        games.word[MAX_WORD_LEN-1]='\0'; //dodanie na koncu null

        for(size_t i=0; i<strlen(games.word); i++){
            games.guessed[i]='_'; //wstawienie wszedzie pustyc miejsc
        }
        games.guessed[strlen(games.word)]='\0'; //dodanie na koncu null
        games.wrong_guesse=0;
        games.active=1; //ustawienie ze gra trwa

        sendtlv(games.player1_fd, TLV_MSG, "Starting game!!!\n", 18);
        sendtlv(games.player2_fd, TLV_MSG, "Starting game!!!\n", 18);
        
        return 0;
    }
    else{
        sendtlv(desc2, TLV_MSG, "ERROR: cannot join game\n", 24);
        return -1;
    }



}

void guess(int desc2, char letter){
    if(!games.active){
        sendtlv(desc2, TLV_MSG, "ERROR: not in game?\n", 20);
        return;
    }
    if(desc2 != games.player1_fd && desc2 != games.player2_fd){
        sendtlv(desc2, TLV_MSG, "ERROR: you didnt start the game\n", 32);
        return;
    }
    letter=tolower(letter); //mala litera0
    int correct=0;

    for(size_t i=0; i<strlen(games.word); i++){
        if(games.word[i]==letter && games.guessed[i]=='_'){
            games.guessed[i]=letter;
            correct=1; //ustawianie na 1, bo pozniej boolem sprawdzamy
        }
    }
    if(!correct){
        games.wrong_guesse++;
    }
    //wyslanie stanu gry
    send_game();
    if(all_guessed()){
        sendtlv(games.player1_fd, TLV_MSG, "GAME OVER!! winner\n", 21);
        sendtlv(games.player2_fd, TLV_MSG, "GAME OVER!! winner\n", 21);
        game_reset();
        return;
    }
    if(games.wrong_guesse >= MAX_WRONG_GUESSES){
        sendtlv(games.player1_fd, TLV_MSG, "GAME OVER!! too many wrong guesses\n", 36);
        sendtlv(games.player2_fd, TLV_MSG, "GAME OVER!! too many wrong guesses\n", 36);
        game_reset();
        return;
    }
}

int all_guessed(void){
    for(size_t i=0; i<strlen(games.word); i++){
        if(games.guessed[i]=='_'){
            return 0; //jesli jest gdzies _ to nie wszysrtkie zgadlismy
        }
    }
    return 1; 
}

void game_reset(void){
    memset(&games, 0, sizeof(games)); //czyszcenie tablicy
}

void send_game(){
    char buff[256];
    snprintf(buff, sizeof(buff), "Word: %s Wrong guesses: %d/%d\n",
             games.guessed, games.wrong_guesse, MAX_WRONG_GUESSES);
    sendtlv(games.player1_fd,TLV_MSG, buff,strlen(buff));
    sendtlv(games.player2_fd,TLV_MSG, buff,strlen(buff));
}