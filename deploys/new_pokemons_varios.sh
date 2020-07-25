cd tp-2020-1c-Qbits-Racers/Game-Boy/
mkdir Debug
cd Debug/
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"consola.d" -MT"consola.d" -o "consola.o" "../consola.c"
gcc -L"../../usoGeneralQbits/Debug" -o "../gameboy" "consola.o" -lcommons -lpthread -lreadline -lm -lusoGeneralQbits
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/gameboy/tp-2020-1c-Qbits-Racers/usoGeneralQbits/Debug/
cd ..

./gameboy GAMECARD NEW_POKEMON Pikachu 1 9 3 1
./gameboy GAMECARD NEW_POKEMON Charmander 4 1 101 2
./gameboy GAMECARD NEW_POKEMON Charmander 517 2046 15 3
./gameboy GAMECARD NEW_POKEMON Charmander 413 17 1 4
./gameboy GAMECARD NEW_POKEMON Charmander 723 97 29 5
./gameboy GAMECARD NEW_POKEMON Charmander 65 93 3001 6
./gameboy GAMECARD NEW_POKEMON Charmander 12 17 34 7
./gameboy GAMECARD NEW_POKEMON Charmander 129 547 11 8
