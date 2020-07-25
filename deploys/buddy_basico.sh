cd tp-2020-1c-Qbits-Racers/Game-Boy/
mkdir Debug
cd Debug/
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"consola.d" -MT"consola.d" -o "consola.o" "../consola.c"
gcc -L"../../usoGeneralQbits/Debug" -o "../gameboy" "consola.o" -lcommons -lpthread -lreadline -lm -lusoGeneralQbits
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/gameboy/tp-2020-1c-Qbits-Racers/usoGeneralQbits/Debug/
cd ..
./gameboy BROKER CAUGHT_POKEMON 1 OK
./gameboy BROKER CAUGHT_POKEMON 2 FAIL

./gameboy BROKER NEW_POKEMON Pikachu 2 3 1

./gameboy BROKER CATCH_POKEMON Onyx 4 5

./gameboy SUSCRIPTOR NEW_POKEMON 10

./gameboy BROKER CATCH_POKEMON Charmander 4 5
