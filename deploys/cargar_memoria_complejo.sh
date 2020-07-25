cd tp-2020-1c-Qbits-Racers/Game-Boy/
mkdir Debug
cd Debug/
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"consola.d" -MT"consola.d" -o "consola.o" "../consola.c"
gcc -L"../../usoGeneralQbits/Debug" -o "../gameboy" "consola.o" -lcommons -lpthread -lreadline -lm -lusoGeneralQbits
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/gameboy/tp-2020-1c-Qbits-Racers/usoGeneralQbits/Debug/
cd ..
./gameboy BROKER CATCH_POKEMON Pikachu 9 3 #19b
./gameboy BROKER CATCH_POKEMON Squirtle 9 3 #20b

./gameboy BROKER CAUGHT_POKEMON 10 OK
./gameboy BROKER CAUGHT_POKEMON 11 FAIL

./gameboy BROKER CATCH_POKEMON Bulbasaur 1 7 #21
./gameboy BROKER CATCH_POKEMON Charmander 1 7 #22

./gameboy BROKER GET_POKEMON Pichu #9
./gameboy BROKER GET_POKEMON Raichu #10
