 gcc -c -D_GNU_SOURCE -Wall -Wreturn-type -g udp-broadcast-client.c
 gcc -c -D_GNU_SOURCE -Wall -Wreturn-type -g udp-broadcast-server.c
 gcc -c -D_GNU_SOURCE -Wall -Wreturn-type -g mkaddr.c
 gcc -g udp-broadcast-client.o mkaddr.o -o udp-broadcast-client
 gcc -g udp-broadcast-server.o mkaddr.o -o udp-broadcast-server

 rm *.o

