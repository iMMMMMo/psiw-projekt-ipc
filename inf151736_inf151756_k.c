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

void sig_handler(int signum)
{

}

int handle_server_answer(int id) {
    struct msg queue;
    pause();
    msgrcv(id, &queue, sizeof(queue), getpid(), 0);
    if (queue.sender == 0)
    {
        printf("Zalogowano.\n");
        return 0;
    }
    else if (queue.sender == 1)
    {
        printf("Użytkownik o podanym nicku nie istnieje!\n");
        return 1;
    }
    else if (queue.sender == 2)
    {
        printf("Użytkownik o podanym nicku jest już zalogowany!\n");
        return 1;
    }
    else if (queue.sender == 3)
    {
        printf("Pomyslnie wylogowano!\n");
        return 0;
    }
}

int login(int id)
{
    struct msg queue;
    char buffer[SHORT_TEXT] = "";
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\r\n" )] = 0;
    queue.msg_type = 1;
    queue.sub_type = 1; // komunikat 1 - proba zalogowania
    queue.sender = getpid();
    strcpy(queue.shortText, buffer);
    msgsnd(id, &queue, sizeof(queue), 0);

    // turn handle_server_answer(id);
    // pause();
    // struct msg mess;
    // printf("Czekam na odpowiedz serwera!\n");
    // msgrcv(id, &mess, sizeof(mess), getpid(), 0);
    // printf("Otrymalem:%d\n", mess.sender);
    // if (mess.sender == 0) {
    //     printf("Zalogowano.\n");
    //     return 0; // udana proba zalogowania
    // }
    // else if(mess.sender == 1)
    //     printf("Użytkownik o podanym nicku nie istnieje!\n");
    // else if(mess.sender == 2)
    //     printf("Użytkownik o podanym nicku jest już zalogowany!\n");
    // return 1; // nieudana proba zalogowania
}

int logout(int id)
{
    struct msg queue = {0};
    queue.msg_type = 1;
    queue.sub_type = 2; // komunikat 2 - proba wylogowania
    queue.sender = getpid();
    msgsnd(id, &queue, sizeof(queue), 0);
    return 0;
    // return handle_server_answer(id);
}

int main()
{
    signal(SIGALRM, sig_handler);
    int msg_id = msgget(76, 0666);
    int logged = 1;
    int choice;
    if (msg_id == -1)
    {
        printf("Wystapil problem z serwerem! Sprobuj ponownie pozniej!\n");
        exit(0);
    }

    printf("Dolaczyles do serwera!\n");
    printf("Aby skorzytac z dodatkowych opcji najpierw musisz sie zalogowac.\n");

    do
    {
        printf("Wpisz login, aby sie zalogowac:\n");
    }
    while(login(msg_id));

    // printf("Dostepne opcje:\n");
    // printf("0 -- wyloguj");
    // while(logged)
    // {
    //     scanf("%d", &choice);
    //     switch(choice) {
    //         case 0:
    //             logout(msg_id);
    //             logged = 0;
    //             break;
    //         default:
    //             printf("Wybrano nieistniejaca opcje!\n");
    //     }
    // }
}