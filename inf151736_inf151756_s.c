// serwer
#include "inf151736_inf151756.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

char* users[] = {"mateusz", "adam", "rafal"};
char* groups[] = {"labowa", "cwiczeniowa"};

void success_message(int id, int user, char* content) {
    struct msg message;
    message.msg_type = user;
    message.sub_type = 0; // nr komunikatu 0 - sukces
    strcpy(message.shortText, content);

    msgsnd(id, &message, sizeof(message), 0);
}

void error_message(int id, int user, char* content) {
    struct msg message;
    message.msg_type = user;
    message.sub_type = -1; // nr komunikatu -1 - error
    strcpy(message.shortText, content);

    msgsnd(id, &message, sizeof(message), 0);
}

int main() {
    struct msg public_queue;
    int msg_id = msgget(76, 0666|IPC_CREAT);
    if (msg_id == -1)
    {
        printf("Error in creating queue\n");
        exit(0);
    }
    int check_user = 0;
    msgrcv(msg_id, &public_queue, sizeof(public_queue), 0, 0);
    // nr komunikatu 1 - probwa logowania
    while(1) {
        if (public_queue.sub_type == 1) {
            for (int i=0; i<sizeof(users)/sizeof(char*); i++) {
                if (strcmp(users[i], public_queue.shortText)==0) {
                    check_user = 1;
                    printf("YES");
                    break;
                }
            }
            if (check_user) {
                success_message(msg_id, public_queue.sender, "Pomyslnie zalogowano");

            }
            else {
                error_message(msg_id, public_queue.sender, "Podany uzytkownik nie istnieje");
            }
            kill(public_queue.sender, SIGALRM);
        }
    }

    msgctl(msg_id, IPC_RMID, 0);
}