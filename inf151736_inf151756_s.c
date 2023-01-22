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

int in_list(char* str, char** list) // 1 jeśli jest 0 jeśli nie ma
{
    int i=0;
    while (list[i])
    {
        if (strcmp(list[i], str)==0)
            return 1;
        i++;
    }
    return 0;
}

void handle_login(int id, int pid, char* username, char* accounts[], char* active[])
{
    struct msg queue; // usunać
    queue.msg_type = pid;
    if (in_list(username, active))
        queue.sender = 2; // juz zalogowany
    else if (in_list(username, accounts))
        queue.sender = 0; // sukces
        add_active_user()
    else
        queue.sender = 1; // nie istnieje
    printf("Wysylam: %d\n.", queue.sender);
    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_logout(int id, int pid, char* accounts[], char* active[])
{
    struct msg queue;
    queue.msg_type = pid;
    queue.sender = 3;

    printf("Wysylam: %d\n.", queue.sender);
    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_list_of_users(int id, int pid, char* accounts[])
{
    struct msg queue;
    queue.msg_type = pid;
    queue.sender = 4;

    int i=0;
    // int position = 0;
    while (accounts[i])
    {
        // strncpy(queue.longText + position, accounts[i], sizeof(char)*64);
        // position += 64;
        // dodac accounts[i]+\n do queue.longText
        i++;
    }
    printf("Wysylam: %d\n.", queue.sender);
    msgsnd(id, &queue, sizeof(queue), 0);
}

int main() {

    struct msg public_queue;
    int msg_id = msgget(76, 0666|IPC_CREAT);

    while(1) {
        msgrcv(msg_id, &public_queue, sizeof(public_queue), 1, 0);

        if (public_queue.sub_type == 1)
        {
            printf("Odebralem: %s.\n", public_queue.shortText);
            handle_login(msg_id, public_queue.sender, public_queue.shortText, users, active_users);
        }
        else if (public_queue.sub_type == 2)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_logout(msg_id, public_queue.sender, users, active_users);
        }
        else if (public_queue.sub_type == 3)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_list_of_users(msg_id, public_queue.sender, users);
        }
        kill(public_queue.sender, SIGALRM);
    }
    msgctl(msg_id, IPC_RMID, 0);
}