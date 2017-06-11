char* reference[5] = {"","192.168.2.1", "192.168.2.2"};
char self = '3';

void broadcast_hello_packet(int sock) {

    //invoke system call to update ip address
    struct packet = make_packet('1', self, 0, NULL);

    int i = 0;

    for(i=1; i < count + 1; i++) {

        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(9001);
        client_addr.sin_addr.s_addr= inet_addr(reference[i]);


        sendto(sock, &packet, sizeof(packet) + 1, 0,
               (struct sockaddr *) &client_addr, sizeof(client_addr));
    }

}

void braodcast_dead_packet(int sock) {

    //invoke system call to update ip address

    struct packet = make_packet('2', self, 0, NULL);

    int i = 0;

    for(i=1; i < count + 1; i++) {

        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(9001);
        client_addr.sin_addr.s_addr= inet_addr(reference[i]);


        sendto(sock, &packet, sizeof(packet) + 1, 0,
               (struct sockaddr *) &client_addr, sizeof(client_addr));
    }
}