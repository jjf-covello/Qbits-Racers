/*
 * standard_libraries.h
 *
 *  Created on: 5 abr. 2020
 *      Author: utnso
 */

#ifndef STANDARD_LIBRARIES_H_
#define STANDARD_LIBRARIES_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <commons/log.h>

t_log* log_operaciones;

enum codigos_de_operacion{

	NEW_POKEMON=100,
	APPEARED_POKEMON=101,
	CATCH_POKEMON=102,
	CAUGHT_POKEMON=103,
	GET_POKEMON=104,
	LOCALIZED_POKEMON = 105,
	SUSCRIPTOR = 106

};

#endif /* STANDARD_LIBRARIES_H_ */
