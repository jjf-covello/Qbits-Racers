/*
 * desarmador_de_paquetes.c
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */
#include "desarmador_de_paquetes.h"
#include <semaphore.h>

void* recibir_paquete(int* alocador,int socket){
	void* buffer=NULL;
	if(recv(socket,alocador,sizeof(int),0)!=0){
		buffer=malloc(*alocador);
		recv(socket,buffer,*alocador,0);
	}

	return buffer;

}

void ejecutar_operacion(void* paquete,int socket){

	int codigo_operacion=conseguir_codigo_operacion(paquete);
	ejecutar_y_desarmar(codigo_operacion,paquete+sizeof(int),socket);

}


int conseguir_codigo_operacion(void* paquete){
	int cod_operacion;
	memcpy(&cod_operacion,paquete,sizeof(int));
	return cod_operacion;
}

void deserializar_ejecutar_appeared_pokemon(void* paquete){

	uint32_t pos_y, pos_x,id;
	char*nombre_pokemon;
	int offset=0;
	int longitud_pokemon;

	memcpy(&pos_x,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&pos_y,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&id,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&longitud_pokemon,paquete+offset,sizeof(int));
	offset+=sizeof(int);

	nombre_pokemon = malloc(longitud_pokemon);

	memcpy(nombre_pokemon,paquete+offset,longitud_pokemon);
	t_log* log_global=obtener_log_obligatorio();
	sem_t* sem_log = obetener_semaforo_log();

	sem_wait(sem_log);
	log_info(log_global,"Me llego un APPEARED POKEMON con los siguientes datos: pokemon-> %s,pos_x-> %d,pos_y->%d",nombre_pokemon,pos_x,pos_y);
	sem_post(sem_log);

	APPEARED_POKEMON_TEAM(nombre_pokemon, pos_x, pos_y,id);

	free(nombre_pokemon);
}

void deserializar_ejecutar_localized_pokemon(void* paquete){

	t_list* lista_de_coordenadas = list_create();
	uint32_t cant_de_coordenadas, pos_x, pos_y, id_mensaje;
	char*nombre_pokemon;
	int offset=0; //offset inicial por peso total del paquete
	int longitud_pokemon;
	memcpy(&id_mensaje,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&longitud_pokemon,paquete+offset,sizeof(int));
	offset+=sizeof(int);

	nombre_pokemon = malloc(longitud_pokemon);

	memcpy(nombre_pokemon,paquete+offset,longitud_pokemon);
	offset+=longitud_pokemon;

	memcpy(&cant_de_coordenadas,paquete+offset,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	t_log* log_global=obtener_log_obligatorio();
	sem_t* sem_log = obetener_semaforo_log();

	sem_wait(sem_log);
	log_info(log_global,"Me llego un LOCALIZED POKEMON con los siguientes datos: id_mensaje-> %d,cantidad_de_coordenadas-> %d,",id_mensaje,cant_de_coordenadas);
	sem_post(sem_log);

	if(le_sirve_al_team(nombre_pokemon)){

		for(int i=0; i<cant_de_coordenadas; i++){
			memcpy(&pos_x,paquete+offset,sizeof(uint32_t));
			offset+=sizeof(uint32_t);

			memcpy(&pos_y,paquete+offset,sizeof(uint32_t));
			offset+=sizeof(uint32_t);

			t_tupla* una_tupla=malloc(sizeof(t_tupla));
			una_tupla->pos_x = pos_x;
			una_tupla->pos_y = pos_y;

			list_add(lista_de_coordenadas,una_tupla);
		}
		void appeared_por_coordenada(t_tupla* una_tupla){
			APPEARED_POKEMON_TEAM(nombre_pokemon,una_tupla->pos_x,una_tupla->pos_y,id_mensaje);
		}
		list_iterate(lista_de_coordenadas,appeared_por_coordenada);
		
	}
	
	void destruir_lista(t_tupla* una_tupla){
		free(una_tupla);
	}
	list_destroy_and_destroy_elements(lista_de_coordenadas,destruir_lista);
	
	free(nombre_pokemon);
}

void deserializar_ejecutar_caught_pokemon(void*  paquete){

	uint32_t id_mensaje, resultado;
	int offset=0;
	int longitud_pokemon;
	memcpy(&resultado,paquete+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(&id_mensaje,paquete+offset, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	if(es_mi_id(id_mensaje)){
		t_log* log_global=obtener_log_obligatorio();
		sem_t* sem_log = obetener_semaforo_log();

		sem_wait(sem_log);
		if(resultado == 1011 || resultado == 1){
			log_info(log_global,"Me llego un CAUGHT POKEMON EXITOSO (id_mensaje: %d)",id_mensaje);
		}
		else
		{
			log_info(log_global,"Me llego un CAUGHT POKEMON FALLIDO (id_mensaje: %d)",id_mensaje);
		}
		sem_post(sem_log);
		CAUGHT_POKEMON_TEAM(id_mensaje,resultado);
	}

}

void* ejecutar_y_desarmar(int codigo_operacion,void* paquete,int socket){

	switch(codigo_operacion){

	case APPEARED_POKEMON:
		deserializar_ejecutar_appeared_pokemon(paquete);
		break;

	case LOCALIZED_POKEMON:
		deserializar_ejecutar_localized_pokemon(paquete);
		break;

	case CAUGHT_POKEMON:
		deserializar_ejecutar_caught_pokemon(paquete);
		break;

	default:
		log_info(log_cod_invalido,"Codigo Invalido");
		break;
	}
	return NULL;
}

