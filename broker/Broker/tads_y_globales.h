/*
 * tads_y_globales.h
 *
 *  Created on: 13 abr. 2020
 *      Author: utnso
 */

#ifndef TADS_Y_GLOBALES_H_
#define TADS_Y_GLOBALES_H_

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <stdint.h>
#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>
#include "standard_libraries.h"

//pthread_mutex_t sem_conectar_con_servidor;

//Pedazo de memoria a utilizar
pthread_mutex_t sem_cache;
void* cache_broker;

//Mutex del send porque, por que no
pthread_mutex_t sem_hacer_el_send;

//Logs y config
pthread_mutex_t sem_logs;
t_log* log_oficial;
t_log* log_interno;
t_config* config;

//Colas
pthread_mutex_t sem_cola_new;
t_queue* cola_new_pokemon;
pthread_mutex_t sem_cola_app;
t_queue* cola_appeared_pokemon;
pthread_mutex_t sem_cola_catch;
t_queue* cola_catch_pokemon;
pthread_mutex_t sem_cola_caught;
t_queue* cola_caught_pokemon;
pthread_mutex_t sem_cola_get;
t_queue* cola_get_pokemon;
pthread_mutex_t sem_cola_localized;
t_queue* cola_localized_pokemon;

//Listas de suscriptores
pthread_mutex_t sem_susc_new;
t_list* lista_de_suscriptores_new_pokemon;
pthread_mutex_t sem_susc_app;
t_list* lista_de_suscriptores_appeared_pokemon;
pthread_mutex_t sem_susc_catch;
t_list* lista_de_suscriptores_catch_pokemon;
pthread_mutex_t sem_susc_caught;
t_list* lista_de_suscriptores_caught_pokemon;
pthread_mutex_t sem_susc_get;
t_list* lista_de_suscriptores_get_pokemon;
pthread_mutex_t sem_susc_localized;
t_list* lista_de_suscriptores_localized_pokemon;

//Listas de mensajes enviados
pthread_mutex_t sem_mensajes_new;
t_list* lista_mensajes_new;
pthread_mutex_t sem_mensajes_app;
t_list* lista_mensajes_app;
pthread_mutex_t sem_mensajes_catch;
t_list* lista_mensajes_catch;
pthread_mutex_t sem_mensajes_caught;
t_list* lista_mensajes_caught;
pthread_mutex_t sem_mensajes_get;
t_list* lista_mensajes_get;
pthread_mutex_t sem_mensajes_localized;
t_list* lista_mensajes_localized;

//Lista de particiones
pthread_mutex_t sem_lista_particiones;
t_list* lista_de_particiones;

//Lista de clientes
pthread_mutex_t sem_lista_de_clientes;
t_list* lista_de_clientes;

//Globales sacadas del config
uint32_t TAMANO_MEMORIA;
uint32_t TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
char* ALGORITMO_PARTICION_LIBRE;
char* IP_BROKER;
uint32_t PUERTO_BROKER;
uint32_t FRECUENCIA_COMPACTACION;
char* LOG_FILE;
int COD_ALGORITMO_REEMPLAZO;
int COD_ALGORITMO_BUSQUEDA;
int COD_ALGORITMO_MEMORIA;
pthread_mutex_t sem_acum_id;
uint32_t acum_id_mensaje;
pthread_mutex_t sem_acum_correlativo;
uint32_t acum_id_correlativo;

//TADS
//mensaje_t lo clave en standard_libraries.h

typedef struct Mensaje_enviado {
	mensaje_t* mensaje;
	t_list* cli_ack; //Guardo los ids nomas
	t_list* cli_enviado; //Guardo los ids nomas
	pthread_mutex_t sem_mensaje; //George's request

} mensaje_enviado_t;

typedef struct Metadata{
	uint32_t id_mensaje_contenido;
	uint32_t id_correlativo;
	uint32_t tamanio_reservado; //El que pediste
	uint32_t tamanio_ocupado; //El ocupado posta posta
	bool libre;
	uint32_t timestamp_LRU;
	uint32_t instante_entrada;
	int posicion_en_lista;
	int codigo_de_cola;

} metadata_t;


typedef struct Cliente {
	long id;
	char* ip;
	int puerto;
} cliente_t ;

enum codigo_algoritmo_reemplazo{
	FIFO = 1,
	LRU = 2,
};

enum codigo_algoritmo_busqueda{
	FF = 3,
	BF = 4,
};

enum codigo_algoritmo_memoria{
	PARTICIONES = 5,
	BS = 6,
};

//Structs para funciones usadas con pthread_create
struct parametros_esperar_ack {
	uint32_t id_mensaje_enviado;
	uint32_t cola_a_usar;
    cliente_t* cliente;
    int peso_esperado;
    int return_del_send;
    int socket;
};



#endif /* TADS_Y_GLOBALES_H_ */
