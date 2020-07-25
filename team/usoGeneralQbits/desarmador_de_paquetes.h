/*
 * desarmador_de_paquetes.h
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#ifndef DESARMADOR_DE_PAQUETES_H_
#define DESARMADOR_DE_PAQUETES_H_
#include "standard_libraries.h"

typedef struct {
	uint32_t pos_x;
	uint32_t pos_y;
	uint32_t cantidad;
}t_tupla;

//PROTOTIPOS
void* ejecutar_y_desarmar(int ,void*,int );
int conseguir_codigo_operacion(void*);
void ejecutar_operacion(void* ,int);
void* recibir_paquete(int* ,int);
void deserializar_ejecutar_appeared_pokemon(void* paquete);
void deserializar_ejecutar_localized_pokemon(void* paquete);
void deserializar_ejecutar_caught_pokemon(void*  paquete);



#endif /* DESARMADOR_DE_PAQUETES_H_ */
