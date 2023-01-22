// headery
#define SHORT_TEXT 64
#define LONG_TEXT 1024
#define USERS_LIMIT 16
#define GROUPS_LIMIT 8
struct msg
{
    long int msg_type; // 1 - serwer, PID - klient
    int sub_type;

    int sender;
    char shortText[SHORT_TEXT];
    char longText[LONG_TEXT];
};