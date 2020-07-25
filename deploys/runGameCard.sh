cd tp-2020-1c-Qbits-Racers/GameCard/
mkdir Debug
cd Debug/
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"main.d" -MT"main.d" -o "main.o" "../main.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"funcionalidades_pokemon.d" -MT"funcionalidades_pokemon.d" -o "funcionalidades_pokemon.o" "../funcionalidades_pokemon.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"funciones_auxiliares.d" -MT"funciones_auxiliares.d" -o "funciones_auxiliares.o" "../funciones_auxiliares.c"
gcc -L"../../usoGeneralQbits/Debug" -o "../gamecard" "funcionalidades_pokemon.o" "funciones_auxiliares.o" "main.o" -lcommons -lpthread -lreadline -lm -lusoGeneralQbits
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/gamecard/tp-2020-1c-Qbits-Racers/usoGeneralQbits/Debug/
cd ..
./gamecard
cd ..