/*
 * funciones_auxiliares.h
 *
 *  Created on: 17 abr. 2020
 *      Author: utnso
 */

#ifndef FUNCIONES_AUXILIARES_H_
#define FUNCIONES_AUXILIARES_H_

//INCLUDES
#include<stdio.h>
#include <stdbool.h>
#include<stdlib.h>
#include<stdint.h>
#include "globales.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/mman.h>
#include "standard_libraries.h"

//ENUMS
enum yes_no_flag{
	YES = 1001,
	NO = 1002
};

enum codigo_de_error{
	NO_EXISTE_EL_POKEMON_FS = 1003,
	NO_SE_PUDO_ABRIR_ARCHIVO = 1004,
	NO_SE_PUDO_VERIFICAR_POSICIONES=1005,
	CONEXION_CAIDA_CON_BROKER = 1006,
	NEW_POKEMON_FALLO = 1007,
	CATCH_POKEMON_FALLO = 1008,
	GET_POKEMON_FALLO = 1009,
	FALLO_DE_OPERACION = 1010,
	OPERACION_EXITOSA = 1011,
	NO_HAY_BLOQUES=1012
};
//PROTOTIPOS

t_list* obtener_todas_las_posiciones(char* );
void enviar_respuesta_broker(void*);
bool verificar_posiciones(uint32_t ,uint32_t ,char*);
bool esta_abierto_archivo(char*);
bool buscar_pokemon_en_fs(char*);
void abrir_archivo(char*);
void cerrar_archivo(char*);
void decrementar_cantidad_pokemon(uint32_t ,uint32_t ,char*);
void crear_pokemon(char* );
int agregar_posicion(uint32_t,uint32_t,uint32_t,char*);
void aumentar_valor_en_posicion(int,uint32_t ,uint32_t ,char*);
char* obtener_path_metadata(char* );
char* obtener_path_bloque_especifico(char* );
char* obtener_path_metadata_pokemon_especifico(char* );
void cerrar_abrir_archivo(char*, int);
tupla_t* deserializar_registro(char* );
void alterar_posicion_mapeada(char** , uint32_t , uint32_t , int , char* );
void apendear_registro_al_final_del_bloque(char* ,uint32_t ,uint32_t ,uint32_t ,int );
int obtener_nuevo_bloque();
bool posicion_indicada(tupla_t* ,uint32_t ,uint32_t);
void error_envio_de_paquete(int);
void crear_paquete_error_new_pokemon();
void crear_paquete_error_catch_pokemon();
void crear_paquete_error_get_pokemon();
void intentar_abrir_archivo(char*);
int* obtener_dimensiones_array(char**);
char* registro(uint32_t ,uint32_t ,uint32_t );
char* actualizar_bloques(char** );
bool esta_bloque_vacio(char*);
void liberar_bloque(int );
uint32_t obtener_cantidad_coordenadas(t_list*);
void liberar_char_doble_puntero(char**);
void corrimiento_de_bloques(char*);
int obtener_cant_de_coordenadas(t_list*);


#endif /* FUNCIONES_AUXILIARES_H_ */
