cd tp-2020-1c-Qbits-Racers/broker/
mkdir Debug
cd Debug/
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"main.d" -MT"main.d" -o "main.o" "../main.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"buddy_system.d" -MT"buddy_system.d" -o "buddy_system.o" "../buddy_system.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"dump_cache.d" -MT"dump_cache.d" -o "dump_cache.o" "../dump_cache.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"funciones_compartidas.d" -MT"funciones_compartidas.d" -o "funciones_compartidas.o" "../funciones_compartidas.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"particiones_dinamicas.d" -MT"particiones_dinamicas.d" -o "particiones_dinamicas.o" "../particiones_dinamicas.c"
gcc -L"../../usoGeneralQbits/Debug" -o "../broker" "buddy_system.o" "dump_cache.o" "funciones_compartidas.o" "main.o" "particiones_dinamicas.o" -lcommons -lpthread -lusoGeneralQbits
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/broker/tp-2020-1c-Qbits-Racers/usoGeneralQbits/Debug/
cd ..
./broker
cd ..