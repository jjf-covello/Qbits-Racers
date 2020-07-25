/*
 * funcionalidades_pokemon.c
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */
#include "funcionalidades_pokemon.h"
#include <errno.h>

void NEW_POKEMON_GAME_CARD(uint32_t id_mensaje,uint32_t pos_y,uint32_t pos_x,char*nombre_pokemon,uint32_t cantidad){

	bool pokemon_en_fs=buscar_pokemon_en_fs(nombre_pokemon);
	if(!pokemon_en_fs){
		log_info(log_obligatorio,"El pokemon %s no se encuentra en el FS", nombre_pokemon);
		crear_pokemon(nombre_pokemon);
	}
	intentar_abrir_archivo(nombre_pokemon);
	abrir_archivo(nombre_pokemon);
	bool posiciones_dentro_del_archivo=verificar_posiciones(pos_x,pos_y,nombre_pokemon);

	if(!posiciones_dentro_del_archivo){
		log_info(log_obligatorio,"No existen las posiciones del archivo de %s", nombre_pokemon);
		int retorno=agregar_posicion(pos_x,pos_y,cantidad,nombre_pokemon);
		if(retorno==NO_HAY_BLOQUES){
			log_info(log_obligatorio,"No pudimos agregar al pokemon %s a nuestros files porque no hay bloques disponibles", nombre_pokemon);
		}
	}else{
		aumentar_valor_en_posicion(cantidad,pos_x,pos_y,nombre_pokemon);
	}
	cerrar_archivo(nombre_pokemon);

	if(!broker_esta_dormido()){
		for(int i = 0; i<cantidad; i++){
			APPEARED_POKEMON_GAME_CARD(nombre_pokemon,pos_x,pos_y,id_mensaje);
		}
	}

}

void APPEARED_POKEMON_GAME_CARD(char*nombre_pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t id_mensaje){

	int peso_pokemon=strlen(nombre_pokemon)+1;
	int peso_buffer=peso_pokemon+sizeof(int)+sizeof(uint32_t)*3;
	void* buffer=malloc(peso_buffer+sizeof(int));
	int offset=0;

	memcpy(buffer+offset,&peso_buffer,sizeof(int));
	offset+=sizeof(int);
	memcpy(buffer+offset,&pos_x,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(buffer+offset,&pos_y,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(buffer+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(buffer+offset,&peso_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(buffer+offset,nombre_pokemon,peso_pokemon);
	offset+=peso_pokemon;

	void* paquete_a_mandar=crear_paquete(APPEARED_POKEMON,buffer);
	enviar_respuesta_broker(paquete_a_mandar);
	free(buffer);
	free(paquete_a_mandar);
}

void CATCH_POKEMON_GAME_CARD(uint32_t id_mensaje,uint32_t pos_y,uint32_t pos_x,char*nombre_pokemon){
	uint32_t resultado;
	bool pokemon_en_fs=buscar_pokemon_en_fs(nombre_pokemon);
	if(!pokemon_en_fs){
		log_info(log_obligatorio,"El pokemon %s no se encuentra en el FS", nombre_pokemon);
		resultado = FALLO_DE_OPERACION;
	}else{

		intentar_abrir_archivo(nombre_pokemon);
		abrir_archivo(nombre_pokemon);
		bool posiciones_dentro_del_archivo=verificar_posiciones(pos_x,pos_y,nombre_pokemon);
		if(!posiciones_dentro_del_archivo){
			log_info(log_obligatorio,"No existen las posiciones del archivo de %s", nombre_pokemon);
			resultado= FALLO_DE_OPERACION;
		}else{
			decrementar_cantidad_pokemon(pos_x,pos_y,nombre_pokemon);
			resultado=OPERACION_EXITOSA;
		}
		cerrar_archivo(nombre_pokemon);
	}

	if(!broker_esta_dormido()){
		CAUGHT_POKEMON_GAME_CARD(resultado,id_mensaje);
	}
}
void CAUGHT_POKEMON_GAME_CARD(uint32_t resultado,uint32_t id_mensaje){
	int peso_buffer=sizeof(uint32_t)*2;
	void* buffer=malloc(peso_buffer+sizeof(int));
	int offset=0;

	memcpy(buffer+offset,&peso_buffer,sizeof(int));
	offset+=sizeof(int);
	memcpy(buffer+offset,&resultado,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(buffer+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);


	void* paquete_a_mandar=crear_paquete(CAUGHT_POKEMON,buffer);
	enviar_respuesta_broker(paquete_a_mandar);
	free(buffer);
	free(paquete_a_mandar);

}
void GET_POKEMON_GAME_CARD(uint32_t id_mensaje,char*nombre_pokemon){

	bool pokemon_en_fs=buscar_pokemon_en_fs(nombre_pokemon);
	t_list* lista_de_posiciones;
	void* buffer;
	if(!pokemon_en_fs){
		lista_de_posiciones=list_create();
	}else{
		intentar_abrir_archivo(nombre_pokemon);

		abrir_archivo(nombre_pokemon);
		lista_de_posiciones=obtener_todas_las_posiciones(nombre_pokemon);
		cerrar_archivo(nombre_pokemon);
	}
	if(!broker_esta_dormido()){
		LOCALIZED_POKEMON_GAME_CARD(lista_de_posiciones,nombre_pokemon,id_mensaje);
	}
}
void LOCALIZED_POKEMON_GAME_CARD(t_list* lista_de_posiciones,char* nombre_pokemon,uint32_t id_mensaje){

	void* buffer;
	int peso_pokemon=strlen(nombre_pokemon)+1;
	uint32_t cantidad_coordenadas = obtener_cantidad_coordenadas(lista_de_posiciones);
	int peso_buffer = sizeof(uint32_t) + sizeof(int) + peso_pokemon + sizeof(uint32_t) + 2*sizeof(uint32_t)*cantidad_coordenadas;
	//int peso_buffer = sizeof(uint32_t) + sizeof(int) + peso_pokemon + sizeof(uint32_t) + 2*sizeof(uint32_t)*lista_de_posiciones->elements_count;
	buffer=malloc(peso_buffer+sizeof(int));
	int offset=0;

	memcpy(buffer+offset,&peso_buffer,sizeof(int));
	offset+=sizeof(int);
	memcpy(buffer+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(buffer+offset,&peso_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(buffer+offset,nombre_pokemon,peso_pokemon);
	offset+=peso_pokemon;

	memcpy(buffer+offset,&cantidad_coordenadas,sizeof(cantidad_coordenadas));
	offset+=sizeof(cantidad_coordenadas);

	void empaquetar_posiciones(tupla_t* tupla){

		for(int i=0; i<tupla->cantidad; i++){
			uint32_t pox = tupla->pos_x;
			uint32_t poy = tupla->pos_y;
			memcpy(buffer+offset,&pox,sizeof(uint32_t));
			offset+=sizeof(uint32_t);
			memcpy(buffer+offset,&poy,sizeof(uint32_t));
			offset+=sizeof(uint32_t);
		}
	}
	list_iterate(lista_de_posiciones,empaquetar_posiciones);

	void* paquete_a_mandar=crear_paquete(LOCALIZED_POKEMON,buffer);
	enviar_respuesta_broker(paquete_a_mandar);

	void destructor(tupla_t* tupla){
		free(tupla);
	}
	list_destroy_and_destroy_elements(lista_de_posiciones,destructor);
	free(buffer);
	free(paquete_a_mandar);


}



