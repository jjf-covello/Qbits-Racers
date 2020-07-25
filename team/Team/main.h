#ifndef MAIN_H_
#define MAIN_H_

#endif
#include <stdbool.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "globales.h"

//PROTOTIPOS

int leer_config();
t_list * obtener_lista_de_lista(char*);

void setear_dic_pokemon();
void setear_dic_objetivos();
void setear_dic_faltantes();
void levantar_server();

void inicializar_suscripciones();
void suscribirse(int );
void enviar_ack(int);
void enviar_fin_de_suscripcion(int);
void enviar_suscripcion(int, int);
long calcular_id();
void inicializar_team();
void CREATE_ENTRENADORES();
void loggear_metricas_finales_por(entrenador_t*);
void loggear_metricas_finales_globales();

