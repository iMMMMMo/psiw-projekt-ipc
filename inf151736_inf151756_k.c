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
    if (signum == SIGUSR1)
    {
        struct msg queue;
        msgrcv(msgget(76, 0666), &queue, sizeof(queue), getpid(), 0);
        if (queue.sender == 14)
        {
            printf("Otrzymałeś wiadomość od: %s\n", queue.shortText);
            printf("Treść: %s", queue.longText);
        }
    }
}

int handle_server_answer(int id)
{
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
        printf("Pomyślnie wylogowano!\n");
        return 0;
    }
    else if (queue.sender == 4)
    {
        printf("Lista zalogowanych użytkowników:\n");
        if (strlen(queue.longText) > 0)
            printf("%s", queue.longText);
        else
            printf("Brak!\n");
        return 0;
    }
    else if (queue.sender == 5)
    {
        printf("Lista użytkowników zapisanych do grupy: \n");
        if (strlen(queue.longText) > 0)
            printf("%s", queue.longText);
        else
            printf("Brak!\n");
        return 0;
    }
    else if (queue.sender == 6)
    {
        printf("Grupa o podanej nazwie nie istnieje.\n");
        return 1;
    }
    else if (queue.sender == 7)
    {
        printf("Juz jesteś w tej grupie.\n");
        return 0;
    }
    else if (queue.sender == 8)
    {
        printf("Pomyślnie dołączono do grupy.\n");
        return 0;
    }
    else if (queue.sender == 9)
    {
        printf("Nie jesteś w tej grupie.\n");
        return 1;
    }
    else if (queue.sender == 10)
    {
        printf("Opuszczono grupę.\n");
        return 0;
    }
    else if (queue.sender == 11)
    {
        printf("Lista dostępnych grup:\n");
        if (strlen(queue.longText) > 0)
            printf("%s", queue.longText);
        else
            printf("Brak!\n");
        return 0;
    }
    else if (queue.sender == 12)
    {
        printf("Wysłano wiadomość do grupy.\n");
        return 0;
    }
    else if (queue.sender == 13)
    {
        printf("Wysłano wiadomość do użytkownika.\n");
        return 0;
    }
    return -1;
}

int login(int id)
{
    struct msg queue;
    char buffer[SHORT_TEXT] = "";
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\r\n" )] = 0;
    queue.msg_type = 1;
    queue.sub_type = 1; // komunikat 1 - próba zalogowania
    queue.sender = getpid();
    strcpy(queue.shortText, buffer);
    msgsnd(id, &queue, sizeof(queue), 0);

    return handle_server_answer(id);

}

int logout(int id)
{
    struct msg queue;
    queue.msg_type = 1;
    queue.sub_type = 2; // komunikat 2 - próba wylogowania
    queue.sender = getpid();
    msgsnd(id, &queue, sizeof(queue), 0);
    return handle_server_answer(id);
}

int print_list_of_users(int id)
{
    struct msg queue;
    queue.msg_type = 1;
    queue.sub_type = 3; // komunikat 3 - lista użytkowników
    queue.sender = getpid();
    msgsnd(id, &queue, sizeof(queue), 0);
    return handle_server_answer(id);
}

int print_users_of_group(int id, char* group_name)
{
    struct msg queue;
    queue.msg_type = 1;
    queue.sub_type = 4; // komunikat 4 - lista zapisanych do danej grupy
    queue.sender = getpid();
    strcpy(queue.shortText, group_name);
    msgsnd(id, &queue, sizeof(queue), 0);
    return handle_server_answer(id);
}

int join_group(int id, char* group_name)
{
    struct msg queue;
    queue.msg_type = 1;
    queue.sub_type = 5; // komunikat 5 - dołącz do grupy
    queue.sender = getpid();
    strcpy(queue.shortText, group_name);
    msgsnd(id, &queue, sizeof(queue), 0);
    return handle_server_answer(id);
}

int leave_group(int id, char* group_name)
{
    struct msg queue;
    queue.msg_type = 1;
    queue.sub_type = 6; // komunikat 6 - opuść grupę
    queue.sender = getpid();
    strcpy(queue.shortText, group_name);
    msgsnd(id, &queue, sizeof(queue), 0);
    return handle_server_answer(id);
}

int print_list_of_groups(int id)
{
    struct msg queue;
    queue.msg_type = 1;
    queue.sub_type = 7; // komunikat 7 - lista grup
    queue.sender = getpid();
    msgsnd(id, &queue, sizeof(queue), 0);
    return handle_server_answer(id);
}

int send_mess_to_group(int id, char* group_name, char* content)
{
    struct msg queue;
    queue.msg_type = 1;
    queue.sub_type = 8; // komunikat 8 - wiadomość do grupy
    queue.sender = getpid();
    strcpy(queue.shortText, group_name);
    strcpy(queue.longText, content);
    msgsnd(id, &queue, sizeof(queue), 0);
    return handle_server_answer(id);
}

int send_mess_to_user(int id, char* username, char* content)
{
    struct msg queue;
    queue.msg_type = 1;
    queue.sub_type = 9; // komunikat 9 - wiadomość do użytkownika
    queue.sender = getpid();
    strcpy(queue.shortText, username);
    strcpy(queue.longText, content);
    msgsnd(id, &queue, sizeof(queue), 0);
    return handle_server_answer(id);
}

int main()
{
    int msg_id = msgget(76, 0666);
    signal(SIGALRM, sig_handler);
    signal(SIGUSR1, sig_handler);
    int logged = 1;
    int choice;
    if (msg_id == -1)
    {
        printf("Wystąpił problem z serwerem! Spróbuj ponownie później!\n");
        exit(0);
    }

    printf("Dołączyłeś do serwera!\n");
    printf("Aby skorzytać z dodatkowych opcji najpierw musisz siś zalogować.\n");

    do
    {
        printf("Wpisz login, aby się zalogować:\n");
    }
    while(login(msg_id));

    printf("Dostępne opcje:\n");
    printf("0 -- wyloguj\n");
    printf("1 -- wyświetl listę zalogowanych użytkowników\n");
    printf("2 -- wyświetl listę użytkowników zapisanych do grupy tematycznej\n");
    printf("3 -- zapisz się do grupy\n");
    printf("4 -- wypisz się z grupy\n");
    printf("5 -- wyświetl listę dostepnych grup\n");
    printf("6 -- wyślij wiadomość do grupy\n");
    printf("7 -- wyślij wiadomość do użytkownika\n");
    while(logged)
    {
        char group_name[SHORT_TEXT] = "";
        char user_name[SHORT_TEXT]= "";
        char mess_content[LONG_TEXT]= "";
        scanf("%d", &choice);
        switch(choice) {
            case 0:
                logout(msg_id);
                logged = 0;
                break;
            case 1:
                print_list_of_users(msg_id);
                break;
            case 2:
                printf("Podaj nazwę grupy: \n");
                fflush(stdin);
                scanf("%s", group_name);
                group_name[strcspn(group_name, "\r\n" )] = 0;
                print_users_of_group(msg_id, group_name);
                break;
            case 3:
                printf("Podaj nazwę grupy: \n");
                fflush(stdin);
                scanf("%s", group_name);
                group_name[strcspn(group_name, "\r\n" )] = 0;
                join_group(msg_id, group_name);
                break;
            case 4:
                printf("Podaj nazwę grupy: \n");
                fflush(stdin);
                scanf("%s", group_name);
                group_name[strcspn(group_name, "\r\n" )] = 0;
                leave_group(msg_id, group_name);
                break;
            case 5:
                print_list_of_groups(msg_id);
                break;
            case 6:
                printf("Podaj nazwę grupy: \n");
                fflush(stdin);
                scanf("%s", group_name);
                printf("Wpisz treść wiadomości: \n");
                fflush(stdin);
                scanf(" %[^\n]", mess_content);
                group_name[strcspn(group_name, "\r\n" )] = 0;
                send_mess_to_group(msg_id, group_name, mess_content);
                break;
            case 7:
                printf("Podaj nazwę użytkownika: \n");
                fflush(stdin);
                scanf("%s", user_name);
                printf("Wpisz treść wiadomości: \n");
                fflush(stdin);
                scanf(" %[^\n]", mess_content);
                user_name[strcspn(user_name, "\r\n" )] = 0;
                send_mess_to_user(msg_id, user_name, mess_content);
                break;
            default:
                printf("Wybrano nieistniejacą opcję!\n");
        }
    }
}