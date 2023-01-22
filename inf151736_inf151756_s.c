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

char* users[USERS_LIMIT] = {"mateusz", "adam", "rafal"};
char* groups[GROUPS_LIMIT] = {"labowa", "cwiczeniowa"};
char* active_users[USERS_LIMIT] = {"rafal"};

int in_list(char* str, char* list) // 1 jeśli jest 0 jeśli nie ma
{
    for (int i=0; i<sizeof(list)/sizeof(char*); i++)
    {
        if (strcmp(list[i], str)==0)
            return 1;
    }
    return 0;
}

void handle_login(int pid, char* username, char* accounts, char* active)
{
    struct msg tmp; // usunać
    tmp.msg_type = pid;
    if (in_list(username, active))
        tmp.sender = 2;
    else if (in_list(username, accounts))
        tmp.sender = 0;
    else
        tmp.sender = 1;
    msgsnd(pid, &tmp, sizeof(tmp), 0);
}

// void success_message(int id, int pid, char* content) {
//     struct msg message;
//     message.msg_type = pid;
//     message.sub_type = 0; // nr komunikatu 0 - sukces 1 - nie istnieje 2 - juz zalogowany
//     msgsnd(id, &message, sizeof(message), 0);
// }

// void error_message(int id, int pid, char* content) {
//     struct msg message;
//     message.msg_type = pid;
//     message.sub_type = -1; // nr komunikatu -1 - error
//     msgsnd(id, &message, sizeof(message), 0);
// }

int main() {

    struct msg public_queue;
    int msg_id = msgget(76, 0666|IPC_CREAT);

    while(1) {
        msgrcv(msg_id, &public_queue, sizeof(public_queue), 1, 0);
        if(public_queue.sub_type == 1)
        {
            handle_login(public_queue.sender, public_queue.shortText, users, active_users);
        // printf("\nOdebrano: %s", public_queue.shortText);
        // if (public_queue.sub_type == 1) {
        //     if (check_user==1)
        //         success_message(msg_id, public_queue.sender, "Pomyslnie zalogowano");
        //     else
        //         error_message(msg_id, public_queue.sender, "Podany uzytkownik nie istnieje");

            kill(public_queue.sender, SIGALRM);
        }
    }

    msgctl(msg_id, IPC_RMID, 0);
}