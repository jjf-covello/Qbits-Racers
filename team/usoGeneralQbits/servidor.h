/*
 * servidor.h
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/config.h>
#include <semaphore.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "standard_libraries.h"


//GLOBALES
t_log* logger;
t_config* config;
pthread_mutex_t mutex_log;
bool server_levantado;
int PUERTO_ESCUCHA;
char* IP;


//PROTOTIPOS
int levantar_server_multithread();
void atender_cliente(int );
int levantar_server_singlethread();
void leer_config_y_setear();
void atender_single_request(int );
bool patova(int );

#endif /* SERVIDOR_H_ */
