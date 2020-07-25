cd tp-2020-1c-Qbits-Racers/Game-Boy/
mkdir Debug
cd Debug/
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"consola.d" -MT"consola.d" -o "consola.o" "../consola.c"
gcc -L"../../usoGeneralQbits/Debug" -o "../gameboy" "consola.o" -lcommons -lpthread -lreadline -lm -lusoGeneralQbits
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/gameboy/tp-2020-1c-Qbits-Racers/usoGeneralQbits/Debug/
cd ..

./gameboy BROKER NEW_POKEMON Onix 2 8 1
./gameboy BROKER NEW_POKEMON Gengar 6 6 1
./gameboy BROKER NEW_POKEMON Squirtle 5 5 1

./gameboy BROKER NEW_POKEMON Umbreon 10 6 1
./gameboy BROKER NEW_POKEMON Espeon 7 1 1
./gameboy BROKER NEW_POKEMON Vaporeon 4 10 1
