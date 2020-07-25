/*
 * cliente.h
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_
#include "standard_libraries.h"
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <commons/config.h>
#include <arpa/inet.h>


#define SERVER_NAME_LEN_MAX 255

//GLOBALES
t_config* config;

//PROTOTIPOS
int conectar_con_servidor(char*,int);
#endif /* CLIENTE_H_ */
