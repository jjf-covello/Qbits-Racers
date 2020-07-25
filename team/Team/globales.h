#ifndef GLOBALES_H_
#define GLOBALES_H_

//includes
#include <stdint.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <math.h>
#include "standard_libraries.h"

//GLOBALES

t_list* POSICIONES_ENTRENADORES; //=[[1,2],[3,7],[5,5],[8,1]]
t_list* POKEMON_ENTRENADORES; //=[[Pikachu,Squirtle,Pidgey],[Squirtle,Charmander],[Bulbasaur],[Weedle]]
t_list* OBJETIVOS_ENTRENADORES; //=[[Pikachu,Pikachu,Squirtle,Pidgey],[Pikachu,Charmander,Charmander],[Squirtle,Bulbasaur],[Weedle, Weedle, Kakuna]]
t_list* LISTA_ID;
t_list* LISTA_ATRAPANDO;
t_list* LISTA_DE_ENTRENADORES;

t_queue* COLA_NEW;
t_queue* COLA_READY;
t_queue* COLA_BLOCK;
t_queue* COLA_EXEC;
t_queue* COLA_EXIT;

pthread_mutex_t SEM_TRANSICION;
pthread_mutex_t SEM_LOGS;

t_dictionary* DIC_POKEMON_ENTRENADORES;
t_dictionary* DIC_OBJETIVOS_ENTRENADORES;
t_dictionary* DIC_FALTANTES_ENTRENADORES;
t_dictionary* DIC_VERIFICADOR_MENSAJES;
t_dictionary* DIC_MAPA_POKEMON;

pthread_mutex_t termino_todo;
pthread_mutex_t SEM_DIC_FALTANTES_ENTRENADORES;
pthread_mutex_t SEM_DIC_MAPA_POKEMON;
pthread_mutex_t SEM_DIC_OBJETIVOS_ENTRENADORES;
pthread_mutex_t SEM_DIC_POKEMON_ENTRENADORES;
pthread_mutex_t SEM_DIC_VERIFICADOR_MENSAJES;
pthread_mutex_t SEM_LISTA_ATRAPANDO;
pthread_mutex_t SEM_LISTA_ID;
pthread_mutex_t SEM_OBJETIVOS_ENTRENADORES;
pthread_mutex_t SEM_POKEMON_ENTRENADORES;
pthread_mutex_t SEM_POSICIONES_ENTRENADORES; //SOLO LLAMAMOS ESTE DIC EN EL MAIN
pthread_mutex_t SEM_LISTA_DE_ENTRENADORES;
pthread_mutex_t SEM_LOG_METRICAS;

sem_t finalizar_proceso;
sem_t sem_entrenadores_ready;
sem_t semaforo_mandar_get;

char* IP_TEAM;
int PUERTO_TEAM;
char* IP_SUSCRIPCION;
int PUERTO_SUSCRIPCION;
int TIEMPO_RECONEXION; //=30
int RETARDO_CICLO_CPU; //=2
char* ALGORITMO_PLANIFICACION; //=RR
int QUANTUM; //=2
char* IP_BROKER; //=127.0.0.1
int ESTIMACION_INICIAL; //=0
int PUERTO_BROKER; //=5002
char* LOG_FILE; // =/home/utnso/log_team1.txt
int suscripcion[3];
float ALPHA_SJF;
int socket_broker;
t_config * config_server;
t_log * log_server;
t_log * log_obligatorio;
t_log * log_metricas;
bool estoy_planificando;
int cantidad_de_ciclos_cpu_total;
int cantidad_de_cambios_de_contexto;
int cantidad_de_deadlocks_resueltos;
/*
 cliclos cpu por entrenador
 ciclos cpu totales
 cambios de contexto
 deadlocks de entrenadores resueltos

 * */
//STRUCT
typedef struct{
	uint32_t pos_x;
	uint32_t pos_y;
}coordenadas_t;

typedef struct{
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t flag;
}tupla_t;

typedef struct{
	char* pokemon;
	uint32_t id;
	int entrenador;
}atrapando_t;

typedef struct{
	coordenadas_t* posicion_actual;
	coordenadas_t* posicion_deseada;
	uint32_t estado_actual;
	uint32_t id_pokemon;
	sem_t semaforo_entrenador;
	sem_t semaforo_entre_en_ready;
	sem_t semaforo_puedo_hacer_otra_rafaga;
	sem_t termine_operacion;
	pthread_mutex_t sem_operaciones_entrenador;
	uint32_t id_entrenador;
	bool estoy_ocioso;
	bool estoy_esperando_un_intercambio;
	bool estoy_esperando_un_caught;
	long estimacion_anterior;
	long estimacion_actual;
	t_list* pokemones_que_tengo_para_intercambiar;
	t_list* pokemones_que_me_faltan;
	int ciclos_que_ejecute;
	pthread_mutex_t cambio_de_estado;
}entrenador_t;

//ENUMS
enum estados{
	NEW=2000,
	BLOCK=2001,
	READY=2002,
	EXEC=2003,
	EXIT=2004,
};

enum flags{
	LO_ESTAN_BUSCANDO=8001,
	NADIE_LO_BUSCA=8000
};

#endif /* GLOBALES_H_ */
