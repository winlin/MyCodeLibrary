/*
Broadcast Sockets (datagrams only)
The following piece of code sets up a broadcast socket, 
which is able to pass data to every machine of the local network. 
Only one physical copy of the data is sent down the wire, 
but being retransmitted by all hardware network components (repeaters, hubs and switches) 
it will propagate onto the entire network (LAN).
*/
int sock;
int on = 1;
struct sockaddr_in sin;

if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
  perror("socket");
  exit(1);
}
if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
  &on, sizeof(on)) < 0) {
  perror("setsockopt");
  exit(1);
}
sin.sin_family = PF_INET;
sin.sin_addr.s_addr = INADDR_BROADCAST;
sin.sin_addr.s_port = htons(5555);
if (bind(sock, (struct sockaddr *)&sin,
  sizeof(sin)) < 0) {
  perror("bind");
  exit(1);
}
/* This sends the message to every machine
   (port 5555) on the local subnet! */
write(sock,"Hello world!", 12);
close(sock);
