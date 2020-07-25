/*
 * standard_libraries.h
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#ifndef STANDARD_LIBRARIES_H_
#define STANDARD_LIBRARIES_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <commons/log.h>

pthread_mutex_t sem_conexion;
pthread_mutex_t sem_conectar_con_servidor;

typedef struct Mensaje {
	uint32_t id_mensaje;
	uint32_t id_correlativo;
	uint32_t cod_cola;

} mensaje_t;

enum codigos_de_operacion{

	NEW_POKEMON=100,
	APPEARED_POKEMON=101,
	CATCH_POKEMON=102,
	CAUGHT_POKEMON=103,
	GET_POKEMON=104,
	LOCALIZED_POKEMON = 105,
	SUSCRIPTOR = 106,
	DISCONNECT = 107,
	QBIT = 108

};
t_log* log_cod_invalido;


#endif /* STANDARD_LIBRARIES_H_ */
