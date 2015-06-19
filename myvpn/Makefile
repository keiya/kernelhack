#myvpn: simplevpn myvpn_nat myvpn_aes

#simplevpn: myvpn.c
#	gcc -Wall -O2 -pthread -o myvpn myvpn.c
#myvpn_nat: myvpn_nat.c
#	gcc -Wall -O2 -pthread -o myvpn_nat myvpn_nat.c
myvpn_aes: myvpn_aes.c
	gcc -Wall -c -O2 -o evp_gcm.o evp_gcm.c
	gcc -Wall -c -O2 -o myvpn_aes.o myvpn_aes.c
	gcc -o myvpn_aes myvpn_aes.o evp_gcm.o -lcrypto -pthread 
