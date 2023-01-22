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

char users[USERS_LIMIT][SHORT_TEXT] = {"mateusz", "adam", "rafal"};
char groups[GROUPS_LIMIT][SHORT_TEXT] = {"labowa", "cwiczeniowa"};
char active_users[USERS_LIMIT][SHORT_TEXT];
int CURRENT_USERS = 3;
int CURRENT_ACTIVE_USERS = 0;

int add_active_user(char* username) 
{
    strcpy(active_users[CURRENT_ACTIVE_USERS], username);
    printf("Dodano aktywnego uzytkownika: %s\n", active_users[CURRENT_ACTIVE_USERS]);
    CURRENT_ACTIVE_USERS++;
}

int delete_active_user(char* username)
{
    int pos=0;
    while(strcmp(active_users[pos], username))
        pos++;

    for(int i=pos-1; i<CURRENT_ACTIVE_USERS-1; i++)
        strcpy(active_users[i], active_users[i+1]);
}

int in_list(char* str, char list[][SHORT_TEXT], int len) // 1 jeśli jest 0 jeśli nie ma
{
    for (int i=0; i<len; i++)
    {
        if (strcmp(list[i], str)==0)
            return 1;
    }
    return 0;
}

// void handle_login(int id, int pid, char* username, char* accounts[], char* active[])
void handle_login(int id, int pid, char* username)
{
    struct msg queue; // usunać
    queue.msg_type = pid;
    if (in_list(username, active_users, CURRENT_ACTIVE_USERS))
        queue.sender = 2; // juz zalogowany
    else if (in_list(username, users, CURRENT_USERS))
    {
        queue.sender = 0; // sukces
        add_active_user(username);
    }
    else
        queue.sender = 1; // nie istnieje
    printf("Wysylam: %d\n.", queue.sender);
    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_logout(int id, int pid, char accounts[][SHORT_TEXT], char active[][SHORT_TEXT])
{
    struct msg queue;
    queue.msg_type = pid;
    queue.sender = 3;

    // delete_active_user();
    printf("Wysylam: %d\n.", queue.sender);
    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_list_of_users(int id, int pid, char active[][SHORT_TEXT])
{
    struct msg queue;
    queue.msg_type = pid;
    queue.sender = 4;

    char tmp[LONG_TEXT] = "";
    for(int i=0; i<CURRENT_ACTIVE_USERS; i++)
    {
        strcat(tmp, active_users[i]);
        strcat(tmp, "\n");
    }

    strcpy(queue.longText, tmp);
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
            handle_login(msg_id, public_queue.sender, public_queue.shortText);
        }
        else if (public_queue.sub_type == 2)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_logout(msg_id, public_queue.sender, users, active_users);
        }
        else if (public_queue.sub_type == 3)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_list_of_users(msg_id, public_queue.sender, active_users);
        }
        kill(public_queue.sender, SIGALRM);
    }
    msgctl(msg_id, IPC_RMID, 0);
}