#myvpn: simplevpn myvpn_nat myvpn_aes

#simplevpn: myvpn.c
#	gcc -Wall -O2 -pthread -o myvpn myvpn.c
#myvpn_nat: myvpn_nat.c
#	gcc -Wall -O2 -pthread -o myvpn_nat myvpn_nat.c
#myvpn_aes: evp_gcm.o myvpn_aes.o
#	gcc -Wall -c -O2 -o evp_gcm.o evp_gcm.c
#	gcc -Wall -c -O2 -o myvpn_aes.o myvpn_aes.c
#	gcc -o myvpn_aes myvpn_aes.o evp_gcm.o -lcrypto -pthread 
myvpn_xor: xor.o myvpn_xor.o
	gcc -Wall -c -O3 -o xor.o xor.c
	gcc -Wall -c -O3 -o myvpn_xor.o myvpn_xor.c
	gcc -o myvpn_xor myvpn_xor.o xor.o -pthread -lcrypto
