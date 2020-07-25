cd tp-2020-1c-Qbits-Racers/usoGeneralQbits/
mkdir Debug
cd Debug/
gcc -O0 -g3 -c -w -fmessage-length=0 -fPIC -MMD -MP -MF"armador_de_paquetes.d" -MT"armador_de_paquetes.d" -o "armador_de_paquetes.o" "../armador_de_paquetes.c"
gcc -O0 -g3 -c -w -fmessage-length=0 -fPIC -MMD -MP -MF"cliente.d" -MT"cliente.d" -o "cliente.o" "../cliente.c"
gcc -O0 -g3 -c -w -fmessage-length=0 -fPIC -MMD -MP -MF"desarmador_de_paquetes.d" -MT"desarmador_de_paquetes.d" -o "desarmador_de_paquetes.o" "../desarmador_de_paquetes.c"
gcc -O0 -g3 -c -w -fmessage-length=0 -fPIC -MMD -MP -MF"servidor.d" -MT"servidor.d" -o "servidor.o" "../servidor.c"
gcc -shared -o "libusoGeneralQbits.so" ./armador_de_paquetes.o ./cliente.o ./desarmador_de_paquetes.o ./servidor.o
cd ..
cd ..
cd ..