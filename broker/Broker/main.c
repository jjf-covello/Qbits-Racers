/*
 * main.c
 *
 *  Created on: 13 abr. 2020
 *      Author: utnso
 */

#include "tads_y_globales.h"
#include "dump_cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void leer_del_config();
void inicializar_listas();
void inicializar_lista_de_particiones();
void inicializar_colas();
void inicializar_semaforos();

int main(){

	signal(SIGUSR1, controlador_de_seniales);

	//Generador de ids
	acum_id_mensaje = 0;
	acum_id_correlativo = 0;

	//Abrir config y obtener valores
	config = config_create("BROKER_CONFIG.config");
	leer_del_config();

	//Incializar logs.
	remove(LOG_FILE);
	remove("LOG_INTERNO.log");
	log_oficial = log_create(LOG_FILE, "BROKER", 0, LOG_LEVEL_DEBUG);
	log_interno = log_create("LOG_INTERNO.log","BROKER", 0, LOG_LEVEL_DEBUG);

	//Setear estructuras administrativas
	inicializar_listas();
	inicializar_lista_de_particiones();
	inicializar_colas();

	//Inicializar semaforos
	inicializar_semaforos();

	//Malloc gigante
	cache_broker = malloc(TAMANO_MEMORIA);

	//Seteo de algoritmos
	if(strcmp(ALGORITMO_MEMORIA, "PARTICIONES") == 0) COD_ALGORITMO_MEMORIA= PARTICIONES;
	if(strcmp(ALGORITMO_MEMORIA, "BS") == 0) COD_ALGORITMO_MEMORIA = BS;
	if(strcmp(ALGORITMO_REEMPLAZO, "FIFO") == 0) COD_ALGORITMO_REEMPLAZO = FIFO;
	if(strcmp(ALGORITMO_REEMPLAZO, "LRU") == 0) COD_ALGORITMO_REEMPLAZO = LRU;
	if(strcmp(ALGORITMO_PARTICION_LIBRE, "FF") == 0) COD_ALGORITMO_BUSQUEDA = FF;
	if(strcmp(ALGORITMO_PARTICION_LIBRE, "BF") == 0) COD_ALGORITMO_BUSQUEDA = BF;

	//Log stuff
	char* pid = string_itoa(getpid());
	loggear_mensaje(pid, 0);
	printf("PID Broker: %s\n", pid);
	free(pid);

	//Inicializar server
	levantar_server_multithread(IP_BROKER, PUERTO_BROKER);

	return 0;
}

void leer_del_config() {

	int iterador_tamanio = 2;
	TAMANO_MEMORIA = config_get_int_value(config, "TAMANO_MEMORIA");
/* CONFIAMOS EN LOS SAN CONFIG
	while(iterador_tamanio <= TAMANO_MEMORIA) {
		iterador_tamanio = iterador_tamanio * 2;
	}
	TAMANO_MEMORIA = iterador_tamanio / 2;
*/

	TAMANO_MINIMO_PARTICION = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");
	ALGORITMO_MEMORIA = config_get_string_value(config, "ALGORITMO_MEMORIA");
	ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	ALGORITMO_PARTICION_LIBRE = config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE");
	IP_BROKER = config_get_string_value(config, "IP_BROKER");
	PUERTO_BROKER = config_get_int_value(config, "PUERTO_BROKER");
	FRECUENCIA_COMPACTACION = config_get_int_value(config, "FRECUENCIA_COMPACTACION");
	LOG_FILE = config_get_string_value(config, "LOG_FILE");
}

void inicializar_listas() {
	lista_de_suscriptores_new_pokemon = list_create();
	lista_de_suscriptores_appeared_pokemon = list_create();
	lista_de_suscriptores_catch_pokemon = list_create();
	lista_de_suscriptores_caught_pokemon = list_create();
	lista_de_suscriptores_get_pokemon = list_create();
	lista_de_suscriptores_localized_pokemon = list_create();
	lista_mensajes_app = list_create();
	lista_mensajes_catch = list_create();
	lista_mensajes_caught = list_create();
	lista_mensajes_get = list_create();
	lista_mensajes_localized = list_create();
	lista_mensajes_new = list_create();
	lista_de_clientes = list_create();
}

void inicializar_lista_de_particiones() {
	lista_de_particiones = list_create();
	metadata_t* metadata_inicial = (metadata_t*)malloc(sizeof(metadata_t));
	metadata_inicial->codigo_de_cola = 0;
	metadata_inicial->id_mensaje_contenido = -1;
	metadata_inicial->libre = true;
	metadata_inicial->tamanio_ocupado = TAMANO_MEMORIA;
	metadata_inicial->tamanio_reservado = TAMANO_MEMORIA;
	metadata_inicial->timestamp_LRU = get_timestamp();
	metadata_inicial->instante_entrada = get_timestamp();
	metadata_inicial->posicion_en_lista = 0;
	list_add(lista_de_particiones, metadata_inicial);

}

void inicializar_colas() {
	cola_new_pokemon = queue_create();
	cola_appeared_pokemon = queue_create();
	cola_catch_pokemon = queue_create();
	cola_caught_pokemon = queue_create();
	cola_get_pokemon = queue_create();
	cola_localized_pokemon = queue_create();
}

void inicializar_semaforos() {
	sem_init(&sem_cache, 0, 1);
	sem_init(&sem_logs, 0, 1);

	sem_init(&sem_cola_app, 0, 1);
	sem_init(&sem_cola_catch, 0, 1);
	sem_init(&sem_cola_caught, 0, 1);
	sem_init(&sem_cola_get, 0, 1);
	sem_init(&sem_cola_localized, 0, 1);
	sem_init(&sem_cola_new, 0, 1);

	sem_init(&sem_susc_app, 0, 1);
	sem_init(&sem_susc_catch, 0, 1);
	sem_init(&sem_susc_caught, 0, 1);
	sem_init(&sem_susc_get, 0, 1);
	sem_init(&sem_susc_localized, 0, 1);
	sem_init(&sem_susc_new, 0, 1);

	sem_init(&sem_lista_particiones, 0, 1);

	sem_init(&sem_acum_correlativo, 0, 1);
	sem_init(&sem_acum_id, 0, 1);

	sem_init(&sem_mensajes_app, 0, 1);
	sem_init(&sem_mensajes_catch, 0, 1);
	sem_init(&sem_mensajes_caught, 0, 1);
	sem_init(&sem_mensajes_get, 0, 1);
	sem_init(&sem_mensajes_localized, 0, 1);
	sem_init(&sem_mensajes_new, 0, 1);

	sem_init(&sem_lista_de_clientes, 0, 1);

	sem_init(&sem_hacer_el_send, 0, 1);

}
