/*
 * desarmador_de_paquetes.h
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#ifndef DESARMADOR_DE_PAQUETES_H_
#define DESARMADOR_DE_PAQUETES_H_
#include "standard_libraries.h"

//PROTOTIPOS
void* ejecutar_y_desarmar(int ,void*,int );
int conseguir_codigo_operacion(void*);
void ejecutar_operacion(void* ,int);
void* recibir_paquete(int* ,int);

#endif /* DESARMADOR_DE_PAQUETES_H_ */
