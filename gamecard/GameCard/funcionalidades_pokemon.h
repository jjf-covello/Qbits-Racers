/*
 * funcionalidades_pokemon.h
 *
 *  Created on: 12 abr. 2020
 *      Author: utnso
 */

#ifndef FUNCIONALIDADES_POKEMON_H_
#define FUNCIONALIDADES_POKEMON_H_
//INCLUDES
#include "funciones_auxiliares.h"


//PROTOTIPOS
//Principales
void NEW_POKEMON_GAME_CARD(uint32_t ,uint32_t ,uint32_t ,char*,uint32_t);
void CATCH_POKEMON_GAME_CARD(uint32_t ,uint32_t ,uint32_t ,char*);
void GET_POKEMON_GAME_CARD(uint32_t ,char*);
void APPEARED_POKEMON_GAME_CARD(char*,uint32_t ,uint32_t,uint32_t );
void CAUGHT_POKEMON_GAME_CARD(uint32_t ,uint32_t );
void LOCALIZED_POKEMON_GAME_CARD(t_list* ,char* ,uint32_t );


#endif /* FUNCIONALIDADES_POKEMON_H_ */
