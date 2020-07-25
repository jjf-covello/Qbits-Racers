/*
 * globales.h
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */

#ifndef GLOBALES_H_
#define GLOBALES_H_

//includes
#include <commons/config.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/string.h>

//GLOBALES

int TIEMPO_DE_REINTENTO_CONEXION;
int TIEMPO_DE_REINTENTO_OPERACION;
char* PUNTO_MONTAJE_TALLGRASS;
char*IP_BROKER;
int PUERTO_BROKER;
char*IP_SUSCRIPTOR;
int PUERTO_SUSCRIPTOR;
int BLOCK_SIZE;
int BLOCKS;
char* MAGIC_NUMBER;

char* path_metadata;
char* path_files;
char* path_pokemon;
char* path_blocks;
char* path_archivo_metadata;
char* path_pokemon_efectivo;

t_bitarray* bitmap;
t_config* config_server;
t_log* log_server;
t_log* log_obligatorio;
bool broker_sigue_vivo;
int socket_broker;
int suscripcion[3];

//STRUCT
typedef struct{
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t cantidad;
}tupla_t;

#endif /* GLOBALES_H_ */
