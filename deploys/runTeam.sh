cd tp-2020-1c-Qbits-Racers/Team/
mkdir Debug
cd Debug/
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"main.d" -MT"main.d" -o "main.o" "../main.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"algoritmos_de_planificacion.d" -MT"algoritmos_de_planificacion.d" -o "algoritmos_de_planificacion.o" "../algoritmos_de_planificacion.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"funcionalidades_principales.d" -MT"funcionalidades_principales.d" -o "funcionalidades_principales.o" "../funcionalidades_principales.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"funciones_auxiliares.d" -MT"funciones_auxiliares.d" -o "funciones_auxiliares.o" "../funciones_auxiliares.c"
gcc -I "../../usoGeneralQbits" -w -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"serializacion.d" -MT"serializacion.d" -o "serializacion.o" "../serializacion.c"
gcc -L"../../usoGeneralQbits/Debug" -o "../team" "algoritmos_de_planificacion.o" "funcionalidades_principales.o" "funciones_auxiliares.o" "main.o" "serializacion.o" -lcommons -lpthread -lreadline -lm -lusoGeneralQbits
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/team/tp-2020-1c-Qbits-Racers/usoGeneralQbits/Debug/
cd ..
./team
cd ..