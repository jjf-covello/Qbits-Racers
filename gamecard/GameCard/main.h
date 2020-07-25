/*
 * main.h
 *
 *  Created on: 7 abr. 2020
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_
#include "servidor.h"
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
#include "globales.h"

//PROTOTIPOS
void levantar_file_system();
void leer_archivo_configuracion();
bool file_system_levantado();
void crear_bitmap();
void setear_bitmap();
void recrear_bitmap();
void inicializar_suscripciones();
void suscribirse_new();
void suscribirse_catch();
void suscribirse_get();
void tocar_a_broker();
void enviar_paquete2(int, void*);
void evaluar_posible_kill_de_suscripciones();
void suscribirse(int);
void enviar_ack(int);
void enviar_fin_de_suscripcion(int,int);
void enviar_suscripcion(int, int);
long calcular_id();
#endif /* MAIN_H_ */
