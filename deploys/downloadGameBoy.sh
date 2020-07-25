cp runSharedLibrary.sh ~
cp runGameBoy.sh ~
cp buddy_basico.sh ~
cp cargar_memoria_complejo.sh ~
cp cargar_memoria_simple.sh ~
cp catch_charmander.sh ~
cp catch_pokemon_escenario_1_s1.sh ~
cp catch_pokemon_escenario_1_s2.sh ~
cp compactacion_basico.sh ~
cp consolidacion_basico.sh ~
cp espera_circular_basico.sh ~
cp new_pikachu.sh ~
cp new_pokemon_antes_team.sh ~
cp new_pokemon_post_team.sh ~
cp new_pokemons_varios.sh ~
cp prueba_final_team.sh ~
cd ~
mkdir gameboy
mv runSharedLibrary.sh gameboy/
mv runGameBoy.sh gameboy/
mv buddy_basico.sh gameboy/
mv cargar_memoria_complejo.sh gameboy/
mv cargar_memoria_simple.sh gameboy/
mv catch_charmander.sh gameboy/
mv catch_pokemon_escenario_1_s1.sh gameboy/
mv catch_pokemon_escenario_1_s2.sh gameboy/
mv compactacion_basico.sh gameboy/
mv consolidacion_basico.sh gameboy/
mv espera_circular_basico.sh gameboy/
mv new_pikachu.sh gameboy/
mv new_pokemon_antes_team.sh gameboy/
mv new_pokemon_post_team.sh gameboy/
mv new_pokemons_varios.sh gameboy/
mv prueba_final_team.sh gameboy/
cd gameboy/
git clone https://github.com/sisoputnfrba/tp-2020-1c-Qbits-Racers.git
cd tp-2020-1c-Qbits-Racers/
git checkout Game-Boy
git pull
cd ..
