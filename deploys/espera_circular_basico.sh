cd tp-2020-1c-Qbits-Racers/Game-Boy/
mkdir Debug
cd Debug/
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"consola.d" -MT"consola.d" -o "consola.o" "../consola.c"
gcc -L"../../usoGeneralQbits/Debug" -o "../gameboy" "consola.o" -lcommons -lpthread -lreadline -lm -lusoGeneralQbits
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/gameboy/tp-2020-1c-Qbits-Racers/usoGeneralQbits/Debug/
cd ..

./gameboy TEAM APPEARED_POKEMON Jolteon 2 2 57
./gameboy TEAM APPEARED_POKEMON Flareon 4 6 57
./gameboy TEAM APPEARED_POKEMON Umbreon 10 6 57
./gameboy TEAM APPEARED_POKEMON Espeon 7 1 57
./gameboy TEAM APPEARED_POKEMON Vaporeon 4 10 57
