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

char groups[GROUPS_LIMIT][SHORT_TEXT];
struct user users[USERS_LIMIT] = {0};
int CURRENT_USERS = 0;
int CURRENT_GROUPS = 0;

void add_user(char* name)
{
    strcpy(users[CURRENT_USERS].username, name);
    users[CURRENT_USERS].pid = 0;
    users[CURRENT_USERS].number_of_groups = 0;
    CURRENT_USERS++;
}

void add_group(char* name)
{
    strcpy(groups[CURRENT_GROUPS], name);
    CURRENT_GROUPS++;
}

void add_active_user(int _pid, char* _username)
{
    for(int i=0; i<CURRENT_USERS; i++)
    {
        if(strcmp(users[i].username, _username)==0)
        {
            users[i].pid = _pid;;
        }
    }
}

void delete_active_user(int _pid)
{
    for (int i=0; i<CURRENT_USERS; i++)
    {
        if (users[i].pid == _pid)
        {
            users[i].pid = 0;
            printf("Wylogowano użytkownika: %s.\n", users[i].username);
        }
    }
}

char* get_username(int _pid)
{
    for (int i=0; i<CURRENT_USERS; i++)
    {
        if (users[i].pid == _pid)
            return users[i].username;
    }
    return "";
}

int get_pid (char* _username)
{
    for (int i=0; i<CURRENT_USERS; i++)
    {
        if (strcmp(users[i].username, _username) == 0)
            return users[i].pid;
    }
    return 0;
}

int in_list(char* name) // 1 jeśli jest 0 jeśli nie ma
{
    for (int i=0; i<CURRENT_USERS; i++)
    {
        if (strcmp(users[i].username, name)==0)
        {
            if (users[i].pid == 0)
                return 0;
            else
                return 1;
        }
    }
    return 2;
}

void handle_login(int id, int pid, char* username)
{
    struct msg queue; // usunać
    queue.msg_type = pid;

    if (in_list(username) == 2)
    {
        queue.sender = 1; // nie istnieje
        printf("Kod %d: Użytkownik %s nie istnieje.\n", queue.sender, username);
    }
    else if (in_list(username) == 1)
    {
        queue.sender = 2; // juz zalogowany
        printf("Kod %d: Użytkownik %s jest już zalogowany.\n", queue.sender, username);
    }
    else
    {
        queue.sender = 0; // sukces
        add_active_user(pid, username);
        printf("Kod %d: Zalogowano użytkownika %s.\n", queue.sender, username);
    }

    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_logout(int id, int pid)
{
    struct msg queue;
    queue.msg_type = pid;
    queue.sender = 3;

    delete_active_user(pid);
    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_list_of_users(int id, int pid)
{
    struct msg queue;
    queue.msg_type = pid;
    queue.sender = 4;

    char tmp[LONG_TEXT] = "";
    for (int i=0; i<CURRENT_USERS; i++)
    {
        if (users[i].pid != 0)
        {
            strcat(tmp, users[i].username);
            strcat(tmp, "\n");
        }
    }

    strcpy(queue.longText, tmp);
    printf("Kod %d: Wysłano listę użytkowników.\n", queue.sender);
    msgsnd(id, &queue, sizeof(queue), 0);
}

int check_group(char* group_name)
{
    for (int i=0; i<CURRENT_GROUPS; i++)
    {
        if (strcmp(groups[i], group_name)==0)
            return 1;
    }
    return 0;
}

int check_if_user_in_group(int pid, char* group_name)
{
    for (int i=0; i<CURRENT_USERS; i++)
    {
        if (users[i].pid == pid)
        {
            for (int j=0; j<users[i].number_of_groups; j++)
            {
                if (strcmp(users[i].group[j], group_name)==0)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void handle_users_of_group(int id, int pid, char* group_name)
{
    struct msg queue;
    queue.msg_type = pid;

    if (!check_group(group_name))
    {
        queue.sender = 6;
        printf("Kod %d: Grupa %s nie istnieje.\n", queue.sender, group_name);
        msgsnd(id, &queue, sizeof(queue), 0);
    }
    else
    {
        char tmp[LONG_TEXT] = "";
        for(int i=0; i<CURRENT_USERS; i++)
        {
            for (int j=0; j<users[i].number_of_groups; j++)
            {
                if (strcmp(users[i].group[j], group_name) == 0)
                {
                    strcat(tmp, users[i].username);
                    strcat(tmp, "\n");
                }
            }
        }
        strcpy(queue.longText, tmp);
        queue.sender = 5;
        printf("Kod %d: Wysłano listę użytkowników grupy %s\n", queue.sender, group_name);
        msgsnd(id, &queue, sizeof(queue), 0);
    }
}



void handle_joining_group(int id, int pid, char* group_name)
{
    struct msg queue;
    queue.msg_type = pid;

    if (check_group(group_name))
    {
        for (int i=0; i<CURRENT_USERS; i++)
        {
            if (users[i].pid == pid)
            {
                if (!check_if_user_in_group(pid, group_name))
                {
                    strcpy(users[i].group[users[i].number_of_groups], group_name);
                    users[i].number_of_groups++;
                    queue.sender = 8;
                    printf("Kod %d: Użytkownik %s dołączył do grupy %s.\n", queue.sender, get_username(pid), group_name);
                }
                else
                {
                    queue.sender = 7;
                    printf("Kod %d: Użytkownik %s próbował dołączyć do grupy %s, ale już w nie jest.\n", queue.sender, get_username(pid), group_name);
                }
            }
        }
    }
    else
    {
        queue.sender = 6;
        printf("Kod %d: Użytkownik %s próbował dołączyć do grupy %s, która nie istnieje.\n", queue.sender, get_username(pid), group_name);
    }
    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_leaving_group(int id, int pid, char* group_name)
{
    struct msg queue;
    queue.msg_type = pid;

    if (!check_group(group_name))
    {
        queue.sender = 6;
        printf("Kod %d: Użytkownik %s próbował opuścić grupę %s, która nie istnieje.\n", queue.sender, get_username(pid), group_name);
    }
    else if (!check_if_user_in_group(pid, group_name))
    {
        queue.sender = 9;
        printf("Kod %d: Użytkownik %s próbował opuścić grupę %s, w której go nie ma .\n", queue.sender, get_username(pid), group_name);
    }
    else
    {
        for (int i=0; i<CURRENT_USERS; i++)
        {
            if (users[i].pid == pid)
            {
                for (int j=0; j<users[i].number_of_groups; j++)
                {
                    if (strcmp(users[i].group[j], group_name)==0)
                    {
                        users[i].number_of_groups--;
                        for (int k=j-1; k<users[i].number_of_groups-1; k++)
                        {
                            strcpy(users[i].group[k], "");
                            strcpy(users[i].group[k], users[i].group[k+1]);
                        }
                    }
                }
            }
        }
        queue.sender = 10;
        printf("Kod %d: Użytkownik %s opuścił grupę %s.\n", queue.sender, get_username(pid), group_name);
    }
    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_list_of_groups(int id, int pid)
{
    struct msg queue;
    queue.msg_type = pid;
    queue.sender = 11;

    char tmp[LONG_TEXT] = "";
    for (int i=0; i<CURRENT_GROUPS; i++)
    {
            strcat(tmp, groups[i]);
            strcat(tmp, "\n");
    }

    strcpy(queue.longText, tmp);
    printf("Kod %d: Wysłano listę dostępnych grup.\n", queue.sender);
    msgsnd(id, &queue, sizeof(queue), 0);
}

void handle_mess_for_group(int id, int pid, char* group_name, char* content)
{
    struct msg queue;
    if (!check_group(group_name))
    {
        queue.msg_type = pid;
        queue.sender = 6;
        printf("Kod %d: Użytkownik %s próbował wysłać wiadomość do %s, która nie istnieje.\n", queue.sender, get_username(pid), group_name);
        msgsnd(id, &queue, sizeof(queue), 0);
        return;
    }
    else if (!check_if_user_in_group(pid, group_name))
    {
        queue.msg_type = pid;
        queue.sender = 9;
        printf("Kod %d: Użytkownik %s próbował wysłać wiadomość do grupy %s, w której go nie ma .\n", queue.sender, get_username(pid), group_name);
        msgsnd(id, &queue, sizeof(queue), 0);
        return;
    }
    else
    {
        strcpy(queue.shortText, get_username(pid));
        char tmp[LONG_TEXT] = "";
        strcat(tmp, content);
        strcat(tmp, "\n");
        strcpy(queue.longText, tmp);
        for (int i=0; i<CURRENT_USERS; i++)
        {
            if (check_if_user_in_group(users[i].pid, group_name))
            {
                queue.msg_type = users[i].pid;
                queue.sender = 14;
                printf("Kod %d: Użytkownik %s wysłał wiadomość do %s.\n", queue.sender, get_username(pid), get_username(users[i].pid));
                kill(users[i].pid, SIGINT);
                msgsnd(id, &queue, sizeof(queue), 0);
            }
        }
    }
    queue.msg_type = pid;
    queue.sender = 12;
    msgsnd(id, &queue, sizeof(queue), 0);
}

int check_user(char* _username)
{
    for (int i=0; i<CURRENT_USERS; i++)
    {
        if (strcmp(users[i].username, _username) == 0)
            return 1;
    }
    return 0;
}

void handle_mess_for_user(int id, int pid, char* username, char* content)
{
    struct msg queue;
    if (!check_user(username))
    {
        queue.sender = 1;
        printf("Kod %d: Użytkownik %s  nie istnieje.\n", queue.sender, username);
        msgsnd(id, &queue, sizeof(queue), 0);
    }
    else
    {
        queue.sender = 14;
        queue.msg_type = get_pid(username);
        strcpy(queue.shortText, get_username(pid));
        char tmp[LONG_TEXT] = "";
        strcat(tmp, content);
        strcat(tmp, "\n");
        strcpy(queue.longText, tmp);
        printf("Kod %d: Użytkownik %s wysłał wiadomość do %s.\n", queue.sender, get_username(pid), username);
        kill(get_pid(username), SIGINT);
        msgsnd(id, &queue, sizeof(queue), 0);
    }
    queue.msg_type = pid;
    queue.sender = 13;
    msgsnd(id, &queue, sizeof(queue), 0);
}
int main() {

    struct msg public_queue;
    int msg_id = msgget(76, 0666|IPC_CREAT);
    if(msg_id == -1)
    {
        if(fork()==0)
            execlp("ipcrm", "ipcrm", "--all=msg", NULL);
        msg_id = msgget(76, 0666|IPC_CREAT);
    }
    int start = 1;

    FILE *conf;
    char *line = NULL;
    size_t len = 0;

    conf = fopen("init.conf", "r");
    if (conf == NULL)
        printf("Nie można odczytać pliku konfiguracyjnego!\n");
    else
    {
        for(int i=0; i<9; i++)
        {
            getline(&line, &len, conf);
            line[strcspn(line, "\r\n" )] = 0;
            add_user(line);
        }
        getline(&line, &len, conf);
        for(int i=0; i<3; i++)
        {
            getline(&line, &len, conf);
            line[strcspn(line, "\r\n" )] = 0;
            add_group(line);
        }
    }

    while(start) {
        msgrcv(msg_id, &public_queue, sizeof(public_queue), 1, 0);
        if (public_queue.sub_type == 1)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_login(msg_id, public_queue.sender, public_queue.shortText);
        }
        else if (public_queue.sub_type == 2)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_logout(msg_id, public_queue.sender);
        }
        else if (public_queue.sub_type == 3)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_list_of_users(msg_id, public_queue.sender);
        }
        else if (public_queue.sub_type == 4)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_users_of_group(msg_id, public_queue.sender, public_queue.shortText);
        }
        else if (public_queue.sub_type == 5)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_joining_group(msg_id, public_queue.sender, public_queue.shortText);
        }
        else if (public_queue.sub_type == 6)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_leaving_group(msg_id, public_queue.sender, public_queue.shortText);
        }
        else if (public_queue.sub_type == 7)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_list_of_groups(msg_id, public_queue.sender);
        }
        else if (public_queue.sub_type == 8)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_mess_for_group(msg_id, public_queue.sender, public_queue.shortText, public_queue.longText);
        }
        else if (public_queue.sub_type == 9)
        {
            printf("Odebralem: %d.\n", public_queue.sub_type);
            handle_mess_for_user(msg_id, public_queue.sender, public_queue.shortText, public_queue.longText);
        }
        kill(public_queue.sender, SIGALRM);
    }
    msgctl(msg_id, IPC_RMID, 0);
}