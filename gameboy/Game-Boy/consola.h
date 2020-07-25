/*
 * consola.h
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdint.h>
#include <stdbool.h>
#include <commons/log.h>
#include "cliente.h"
#include "armador_de_paquetes.h"
#include <readline/readline.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "desarmador_de_paquetes.h"

//GLOBALES
char* IP_SUSCRIPTOR;
char* IP_GAME_CARD;
char* IP_BROKER;
char*IP_TEAM;
int PUERTO_SUSCRIPTOR;
int PUERTO_GAME_CARD;
int PUERTO_BROKER;
int PUERTO_TEAM;
t_log* logger_obligatorio;
t_log* log_operaciones;

//PROTOTIPOS
int leer_parametros_BROKER_APPEARED_POKEMON(char* ,uint32_t*, uint32_t*, uint32_t*, char*);
int leer_parametros_CAUGHT_POKEMON(uint32_t*, bool*, char*);
int leer_parametros_TEAM_APPEARED_POKEMON(char* ,uint32_t*, uint32_t*, uint32_t*, char*);
int leer_parametros_NEW_POKEMON_BROKER(char* ,uint32_t*, uint32_t*, uint32_t*, char*);
int leer_parametros_NEW_POKEMON(char* ,uint32_t*, uint32_t*, uint32_t*, uint32_t*, char*);
int leer_parametros_CATCH_POKEMON_BROKER(char*, uint32_t*, uint32_t*, char*);
int leer_parametros_CATCH_POKEMON(char* ,uint32_t*, uint32_t*, uint32_t*, char*);
int leer_parametros_GET_POKEMON_GAMECARD(char* ,uint32_t*,char* );
int leer_parametros_GET_POKEMON(char*,char*);
int leer_parametros_SUSCRIPTOR(char* , uint32_t* , char*);

int TEAM_APPEARED_POKEMON(char* ,uint32_t ,uint32_t,uint32_t);

int GAMECARD_NEW_POKEMON(char* ,uint32_t ,uint32_t ,uint32_t ,uint32_t);
int GAMECARD_CATCH_POKEMON(char* ,uint32_t  ,uint32_t ,uint32_t);
int GAMECARD_GET_POKEMON(char*,uint32_t);

int BROKER_NEW_POKEMON(char* ,uint32_t ,uint32_t ,uint32_t);
int BROKER_APPEARED_POKEMON(char* ,uint32_t ,uint32_t ,uint32_t);
int BROKER_CATCH_POKEMON(char* , uint32_t ,uint32_t);
int BROKER_CAUGHT_POKEMON(uint32_t , bool);
int BROKER_GET_POKEMON(char* );

void correr_consola();
int conectarse_como_SUSCRIPTOR(char*, uint32_t);
int obtener_cola(char*);
long calcular_id();
int get_pokemon_gamecard(int socket,char*,uint32_t);
int get_pokemon(int ,char*);
int catch_pokemon_broker(int ,char*,uint32_t ,uint32_t);
int catch_pokemon_gamecard(int ,char*,uint32_t ,uint32_t ,uint32_t);
int new_pokemon_broker(int ,char* ,uint32_t ,uint32_t ,uint32_t);
int new_pokemon_gamecard(int ,char* ,uint32_t ,uint32_t ,uint32_t,uint32_t);
bool conectar_con_otros();
bool conectar_con_broker();
void leer_datos_de_config();
void loggear_conexion(char*);
void loggear_suscripcion_a(char*,uint32_t);
void loggear_llegada_de_mensajes(char*);
void loggear_envio_de_mensajes(char* );


#endif /* CONSOLA_H_ */
