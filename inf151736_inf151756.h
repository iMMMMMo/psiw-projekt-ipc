// headery
struct msg {
    long int msg_type; // 1 - serwer, 2 - klient
    int sub_type;

    int sender;
    char shortText[64];
    char longText[1024];
};