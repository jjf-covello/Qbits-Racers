
#ifndef FUNCIONES_AUXILIARES_H_
#define FUNCIONES_AUXILIARES_H_

#include "globales.h"

void comportamiento_entrenador(entrenador_t* );
char* obtener_pokemon(coordenadas_t* );
bool es_mi_id(uint32_t);
void ordenar_alfabeticamente(t_list* );
bool son_iguales(t_list* , t_list* );
char* obtener_pokemon(coordenadas_t* );
bool cumpli_todos_mis_objetivos(entrenador_t*);
coordenadas_t* leer_coordenada_string(char*);
void liberar_char_doble_puntero(char**);
t_list* obtener_lista_de_pokemons(char** );
t_list*leer_lista_string(char*);
t_list* ubicar_entrenadores(char** );
void pasar_de_cola_a(entrenador_t* , int );
t_queue*obtener_cola(int );
double modulo(int ,int );
entrenador_t* entrenador_mas_cercano(entrenador_t*,entrenador_t*,uint32_t ,uint32_t);
entrenador_t*entrenador_mas_cercano_en_new(uint32_t,uint32_t);
entrenador_t* entrenador_mas_cercano_en_block(uint32_t,uint32_t);
bool esta_mas_cerca(entrenador_t*,entrenador_t* , uint32_t ,uint32_t);
entrenador_t*entrenador_mas_cercano_en_a(t_list* ,uint32_t,uint32_t );
bool tiene_capacidad_maxima_pokemon(entrenador_t*);
void realizar_intercambio(entrenador_t*);
int desplazar_en_mapa_al(entrenador_t*);
t_list* pokemones_que_no_me_sirven(entrenador_t*);
t_list* entrenadores_para_intercambiar(entrenador_t*,char*);
void ordenar_lista_de_tradeo(entrenador_t*,entrenador_t*);
bool quiero_ese_pokemon(entrenador_t*, char*);
bool cumple_con_algun_objetivo_personal(entrenador_t* ,char*);
void agregar_pokemon_que_no_me_sirve(entrenador_t* ,char*);
void mover_de_cola(entrenador_t*);
bool puedo_atrapar_algun_pokemon_del_mapa(entrenador_t*);
coordenadas_t* asignar_coordenadas_del_pokemon(entrenador_t*);
sem_t* obetener_semaforo_log();
t_log*obtener_log_obligatorio();
void actualizar_mapa(char* , entrenador_t*);
t_list* pokemones_que_me_faltan(entrenador_t*);
void decrementar_pokemones_faltantes(char*);
void incrementar_pokemones_faltantes(char*);
bool le_sirve_al_team(char*);
void calcular_ciclos_globales();

#endif /* FUNCIONES_AUXILIARES_H_ */
