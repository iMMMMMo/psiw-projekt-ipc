// klient
#include "inf151736_inf151756.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void sig_handler(int signum) {

}

int login(int id) {
    struct msg queue;
    char buffer[64];
    fgets(buffer, sizeof(buffer), stdin);
    queue.msg_type = 1;
    queue.sub_type = 1; // komunikat 1 - proba zalogowania
    queue.sender = getpid();
    strcpy(queue.shortText, buffer);
    msgsnd(id, &queue, sizeof(queue), 0);
    pause();

    struct msg mess;
    msgrcv(id, &mess, sizeof(mess), 0, 0);
    printf("%s\n", mess.shortText);
    if (mess.sub_type == 0) {
        return 0; // udana proba zalogowania
    }
    return 1; // nieudana proba zalogowania
}

int main() {
    int msg_id = msgget(76, 0666|IPC_CREAT);

    if (msg_id == -1)
    {
        printf("Error in creating queue\n");
        exit(0);
    }

    printf("Dolaczyles do serwera!\n");
    do {
        signal(SIGALRM, sig_handler);
        printf("Wpisz login, aby sie zalogowac:\n");
    }
    while (login(msg_id));
    // while(1) {}
}