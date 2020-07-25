
#include "consola.h"
#include <math.h>

/*    MANUAL DE COMANDOS

NEW-----> "nombre_proceso NEW_POKEMON pokemon posx posy cantidad"
APPEARED-----> "nombre_proceso APPERED_POKEMON pokemon posx posy id_mensajes"
GET-----> "nombre_proceso GET_POKEMON pokemon"
CATCH-------> "nombre_proceso CATCH pokemon posx posy"
CAUGHT-------> "nombre_proceso CAUGHT id_mensaje ok_fail "

 */

int main(int argc,char* argv[]){
	logger_obligatorio=log_create("logger obligatorio GameBoy.log","GAMEBOY",0,LOG_LEVEL_INFO);
	log_operaciones =log_create("logger operaciones.log","OPERACIONES",0,LOG_LEVEL_INFO);
	leer_datos_de_config();

	switch(argc){
	case 1:
		correr_consola();
		break;
	case 2:
	case 3:
		printf("Argumentos inválidos.");
		break;
	case 4:
		if(!strcmp(argv[1], "BROKER") && !strcmp(argv[2], "GET_POKEMON")){
			BROKER_GET_POKEMON(argv[3]);
		}else if(!strcmp(argv[1], "SUSCRIPTOR")){
			conectarse_como_SUSCRIPTOR(argv[2],atoi(argv[3]));
		}else{
			printf("Argumentos inválidos.");
		}
		break;
	case 5:
		if(!strcmp(argv[1], "GAMECARD") && !strcmp(argv[2], "GET_POKEMON")){
			GAMECARD_GET_POKEMON(argv[3],(uint32_t) atoi(argv[4]));
		}else if(!strcmp(argv[1], "BROKER") && !strcmp(argv[2], "CAUGHT_POKEMON")){
			bool ok_fail;
			if(strcmp(argv[4], "OK") == 0) ok_fail = 1;
			else if(strcmp(argv[4], "FAIL") == 0) ok_fail = 0;
			else if(strcmp(argv[4], "1") == 0) ok_fail = 1;
			else if(strcmp(argv[4], "0") == 0) ok_fail = 0;
			BROKER_CAUGHT_POKEMON((uint32_t) atoi(argv[3]), ok_fail);
		}else{
			printf("Argumentos inválidos.");
		}
		break;
	case 6:
		if(!strcmp(argv[1], "BROKER") && !strcmp(argv[2], "CATCH_POKEMON")){
			BROKER_CATCH_POKEMON(argv[3],(uint32_t) atoi(argv[4]),(uint32_t) atoi(argv[5]));
		}else{
			printf("Argumentos inválidos.");
		}
		break;
	case 7:
		if(!strcmp(argv[1], "BROKER") && !strcmp(argv[2], "APPEARED_POKEMON")){
			BROKER_APPEARED_POKEMON(argv[3],(uint32_t) atoi(argv[4]),(uint32_t) atoi(argv[5]),(uint32_t) atoi(argv[6]));
		}else if(!strcmp(argv[1], "BROKER") && !strcmp(argv[2], "NEW_POKEMON")){
			BROKER_NEW_POKEMON(argv[3],(uint32_t) atoi(argv[4]),(uint32_t) atoi(argv[5]),(uint32_t) atoi(argv[6]));
		}else if(!strcmp(argv[1], "TEAM") && !strcmp(argv[2], "APPEARED_POKEMON")){
			TEAM_APPEARED_POKEMON(argv[3],(uint32_t) atoi(argv[4]),(uint32_t) atoi(argv[5]),(uint32_t) atoi(argv[6]));
		}else if(!strcmp(argv[1], "GAMECARD") && !strcmp(argv[2], "CATCH_POKEMON")){
			GAMECARD_CATCH_POKEMON(argv[3],(uint32_t) atoi(argv[4]),(uint32_t) atoi(argv[5]),(uint32_t) atoi(argv[6]));
		}else  {
			printf("Argumentos inválidos.");
		}
		break;
	case 8:
		if(!strcmp(argv[1], "GAMECARD") && !strcmp(argv[2], "NEW_POKEMON")){
			GAMECARD_NEW_POKEMON(argv[3],(uint32_t) atoi(argv[4]),(uint32_t) atoi(argv[5]),(uint32_t) atoi(argv[6]),(uint32_t) atoi(argv[7]));
		}else{
			printf("Argumentos inválidos.");
		}
		break;
	}
	return 0;
}

void correr_consola(){
	char *linea = " ";
	while(linea != '\0'){

		printf("Ingrese un comando...\n");
		linea = readline(">");

		if(!strncmp(linea, "TEAM APPEARED_POKEMON",21)){

			char* pokemon =(char*) malloc(strlen(linea)+1);
			uint32_t pos_x;
			uint32_t pos_y;
			uint32_t id_mensaje;

			if(leer_parametros_TEAM_APPEARED_POKEMON(pokemon, &pos_x, &pos_y, &id_mensaje, linea)){
				if(TEAM_APPEARED_POKEMON(pokemon,pos_x,pos_y,id_mensaje)){
					printf("Enviando mensaje a la cola APPEARED_POKEMON\n");
				} else {
					printf("El mensaje no fue enviado \n");
				}
				free(pokemon);
			}else{
				printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
				free(pokemon);
			}

		}else{
			if(!strncmp(linea, "GAMECARD NEW_POKEMON",20)){

				char* pokemon =(char*) malloc(strlen(linea)+1); //TODO Un asco man
				uint32_t pos_x;
				uint32_t pos_y;
				uint32_t cantidad;
				uint32_t id_mensaje;

				if(leer_parametros_NEW_POKEMON(pokemon, &pos_x, &pos_y, &cantidad, &id_mensaje, linea)){

					if(GAMECARD_NEW_POKEMON(pokemon,pos_x,pos_y,cantidad,id_mensaje)) {
						printf("Enviando mensaje a la cola NEW_POKEMON\n");
					} else {
						printf("El mensaje no fue enviado\n");
					}
					free(pokemon);

				}else{

					printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
					free(pokemon);
				}

			}else{

				if(!strncmp(linea, "GAMECARD CATCH_POKEMON",22)){

					char* pokemon =(char*) malloc(strlen(linea)+1);
					uint32_t pos_x;
					uint32_t pos_y;
					uint32_t id_mensaje;

					if(leer_parametros_CATCH_POKEMON(pokemon, &pos_x, &pos_y, &id_mensaje, linea)){

						if(GAMECARD_CATCH_POKEMON(pokemon, pos_x, pos_y, id_mensaje)){
							printf("Enviando mensaje a la cola CATCH_POKEMON\n");
						} else {
							printf("El mensaje no fue enviado\n");
						}
						free(pokemon);

					}else{
						printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
						free(pokemon);
					}

				}else{
					if(!strncmp(linea, "GAMECARD GET_POKEMON",20)){

						char* pokemon =(char*) malloc(strlen(linea)+1);
						uint32_t id_mensaje;

						if(leer_parametros_GET_POKEMON_GAMECARD(pokemon, &id_mensaje, linea)) {

							if(GAMECARD_GET_POKEMON(pokemon, id_mensaje)){
								printf("Enviando mensaje a la cola GET_POKEMON\n");
							} else {
								printf("El mensaje no fue enviado\n");
							}
							free(pokemon);

						}else{
							printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
							free(pokemon);
						}

					}else{
						if(!strncmp(linea, "BROKER NEW_POKEMON",18)){

							char* pokemon =(char*) malloc(strlen(linea)+1);
							uint32_t pos_x;
							uint32_t pos_y;
							uint32_t cantidad;

							if(leer_parametros_NEW_POKEMON_BROKER(pokemon, &pos_x, &pos_y, &cantidad, linea)){

								if(BROKER_NEW_POKEMON(pokemon,pos_x,pos_y,cantidad)) {
									printf("Enviando mensaje a la cola NEW_POKEMON\n");
								} else {
									printf("El mensaje no fue enviado\n");
								}
								free(pokemon);

							}else{

								printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
								free(pokemon);
							}

						}else{
							if(!strncmp(linea, "BROKER APPEARED_POKEMON", 23)){

								char* pokemon =(char*) malloc(strlen(linea)+1);
								uint32_t pos_x;
								uint32_t pos_y;
								uint32_t id_mensaje;

								if(leer_parametros_BROKER_APPEARED_POKEMON(pokemon , &pos_x, &pos_y, &id_mensaje, linea)){

									if(BROKER_APPEARED_POKEMON(pokemon,pos_x,pos_y,id_mensaje)) {
										printf("Enviando mensaje a la cola APPEARED_POKEMON\n");
									} else {
										printf("El mensaje no fue enviado\n");
									}
									free(pokemon);

								}else{

									printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
									free(pokemon);
								}

							}else{
								if(!strncmp(linea, "BROKER CATCH_POKEMON", 20)){

									char* pokemon =(char*) malloc(strlen(linea)+1);
									uint32_t pos_x;
									uint32_t pos_y;

									if(leer_parametros_CATCH_POKEMON_BROKER(pokemon, &pos_x, &pos_y, linea)){

										if(BROKER_CATCH_POKEMON(pokemon, pos_x, pos_y)){
											printf("Enviando mensaje a la cola CATCH_POKEMON\n");
										} else {
											printf("El mensaje no fue enviado\n");
										}
										free(pokemon);

									}else{
										printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
										free(pokemon);
									}



								}else{
									if(!strncmp(linea, "BROKER CAUGHT_POKEMON", 21)){

										uint32_t id_mensaje;
										bool ok_fail;

										if(leer_parametros_CAUGHT_POKEMON(&id_mensaje, &ok_fail, linea)){

											if(BROKER_CAUGHT_POKEMON(id_mensaje, ok_fail)){
												printf("Enviando mensaje a la cola CAUGHT_POKEMON\n");
											} else {
												printf("El mensaje no fue enviado\n");
											}

										}else{
											printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");

										}


									}else{
										if(!strncmp(linea, "BROKER GET_POKEMON", 18)){

											char* pokemon =(char*) malloc(strlen(linea)+1);

											if(leer_parametros_GET_POKEMON(pokemon, linea)) {

												if(BROKER_GET_POKEMON(pokemon)){
													printf("Enviando mensaje a la cola GET_POKEMON\n");
												} else {
													printf("El mensaje no fue enviado\n");
												}
												free(pokemon);

											}else{
												printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
												free(pokemon);
											}



										}else{
											if(!strncmp(linea, "SUSCRIPTOR", 10)){

												char* nombre_cola =(char*) malloc(strlen(linea)+1);
												uint32_t tiempo;

												if(leer_parametros_SUSCRIPTOR(nombre_cola, &tiempo, linea)) {

													if(conectarse_como_SUSCRIPTOR(nombre_cola,tiempo)){
														printf("Conectandose como suscriptor a la cola --> %s\n", nombre_cola);
													} else {
														printf("No fue posible conectarse como suscriptor\n");
													}
													free(nombre_cola);

												}else{
													printf("No pudimos leer bien los paramatros => No enviamos ningun mensaje\n");
													free(nombre_cola);
												}



											}else{

												printf("Comando invalido, volver a intentar :(\n");
											}

										}
									}
								}
							}
						}
					}}}}

		//TODO: del lado de cada modulo, considerar el caso que caí al recv pero no
		//me mandaron nada, para no manosear paquetes vacios.
		//close(socket_game_card);
		//close(socket_team);
	}
	free(linea);
	printf("Se cerro la consola\n");
}

//Funciones Auxiliares

int leer_parametros_TEAM_APPEARED_POKEMON(char* pokemon,uint32_t* pos_x, uint32_t* pos_y, uint32_t* id_mensaje, char* linea){

	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL||parametros[3]==NULL|| parametros[4]==NULL || parametros[5]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(pokemon, parametros[2]);
	*pos_x=(int) atoi(parametros[3]);
	*pos_y=(int) atoi(parametros[4]);
	*id_mensaje=(int) atoi(parametros[5]);

	return 1;
}

int leer_parametros_NEW_POKEMON_BROKER(char* pokemon,uint32_t* pos_x, uint32_t* pos_y, uint32_t* cantidad,char* linea){

	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL||parametros[3]==NULL|| parametros[4]==NULL || parametros[5]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(pokemon, parametros[2]);
	*pos_x=(int) atoi(parametros[3]);
	*pos_y=(int) atoi(parametros[4]);
	*cantidad=(int) atoi(parametros[5]);

	return 1;
}

int leer_parametros_NEW_POKEMON(char* pokemon,uint32_t* pos_x, uint32_t* pos_y, uint32_t* cantidad, uint32_t* id_mensaje,char* linea){

	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL||parametros[3]==NULL|| parametros[4]==NULL || parametros[5]==NULL || parametros[6]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(pokemon, parametros[2]);
	*pos_x=(int) atoi(parametros[3]);
	*pos_y=(int) atoi(parametros[4]);
	*cantidad=(int) atoi(parametros[5]);
	*id_mensaje=(int) atoi(parametros[6]);

	return 1;
}

int leer_parametros_CATCH_POKEMON_BROKER(char* pokemon, uint32_t* pos_x, uint32_t* pos_y, char* linea){

	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL||parametros[3]==NULL|| parametros[4]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(pokemon, parametros[2]);
	*pos_x=(int) atoi(parametros[3]);
	*pos_y=(int) atoi(parametros[4]);

	return 1;
}

int leer_parametros_CATCH_POKEMON(char* pokemon, uint32_t* pos_x, uint32_t* pos_y, uint32_t* id_mensaje, char* linea){

	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL||parametros[3]==NULL|| parametros[4]==NULL || parametros[5]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(pokemon, parametros[2]);
	*pos_x=(int) atoi(parametros[3]);
	*pos_y=(int) atoi(parametros[4]);
	*id_mensaje=(int) atoi(parametros[5]);

	return 1;
}

int leer_parametros_GET_POKEMON_GAMECARD(char* pokemon,uint32_t* id_mensaje,char* linea){
	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL||parametros[3]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(pokemon, parametros[2]);
	*id_mensaje=(int) atoi(parametros[3]);

	return 1;
}

int leer_parametros_GET_POKEMON(char* pokemon,char* linea){
	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(pokemon, parametros[2]);

	return 1;
}

int leer_parametros_BROKER_APPEARED_POKEMON(char* pokemon,uint32_t* pos_x, uint32_t* pos_y, uint32_t* id_mensaje, char* linea){

	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL||parametros[3]==NULL|| parametros[4]==NULL || parametros[5]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(pokemon, parametros[2]);
	*pos_x=(int) atoi(parametros[3]);
	*pos_y=(int) atoi(parametros[4]);
	*id_mensaje=(int) atoi(parametros[5]);

	return 1;
}

int leer_parametros_CAUGHT_POKEMON(uint32_t* id_mensaje, bool* ok_fail, char* linea){

	char** parametros = string_split(linea, " ");

	if(parametros[2]==NULL||parametros[3]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	*id_mensaje=(int) atoi(parametros[2]);
	//ok_fail=(int) atoi(parametros[3]); //0 is false 1 is true :D
	if(strcmp(parametros[3], "OK") == 0) *ok_fail = 1;
	if(strcmp(parametros[3], "FAIL") == 0) *ok_fail = 0;
	if(strcmp(parametros[3], "1") == 0) *ok_fail = 1;
	if(strcmp(parametros[3], "0") == 0) *ok_fail = 0;

	return 1;
}

int leer_parametros_SUSCRIPTOR(char* nombre_cola, uint32_t* tiempo, char* linea){
	char** parametros = string_split(linea, " ");

	if(parametros[1]==NULL||parametros[2]==NULL){

		printf("Faltan parametros\n");
		return 0;
	}

	strcpy(nombre_cola, parametros[1]);
	*tiempo=(int) atoi(parametros[2]);

	return 1;
}

bool conectar_con_otros(){
	bool flag=false;
	int socket_game_card = conectar_con_servidor(IP_GAME_CARD,PUERTO_GAME_CARD);
	int socket_team=conectar_con_servidor(IP_TEAM,PUERTO_TEAM);
	if(socket_team>3 || socket_game_card>3){ //TODO
		loggear_conexion(IP_GAME_CARD);
		loggear_conexion(IP_TEAM);
		flag=true;
	}

	return flag;


	//TODO preguntar si tenemos unicamente un team por gameboy a la vez


}

void loggear_conexion(char* ip){

	log_info(logger_obligatorio,"Pudimos conectarnos a la IP %s",ip);

}
bool conectar_con_broker(int argc,char* argv[]){
	bool flag=false;
	int socket_broker=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	if(socket_broker>3){
		flag=true;
		loggear_conexion(IP_BROKER);
	}
	return flag;

}

void leer_datos_de_config(){
	config=config_create("config.config");
	IP_SUSCRIPTOR=config_get_string_value(config,"IP_SUSCRIPTOR");
	IP_GAME_CARD=config_get_string_value(config,"IP_GAME_CARD");
	IP_BROKER=config_get_string_value(config,"IP_BROKER");
	IP_TEAM=config_get_string_value(config,"IP_TEAM");
	PUERTO_SUSCRIPTOR=config_get_int_value(config,"PUERTO_SUSCRIPTOR");
	PUERTO_GAME_CARD=config_get_int_value(config,"PUERTO_GAME_CARD");
	PUERTO_BROKER=config_get_int_value(config,"PUERTO_BROKER");
	PUERTO_TEAM=config_get_int_value(config,"PUERTO_TEAM");
}


int BROKER_NEW_POKEMON(char* pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t cantidad){
	int socket_broker = conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	return  new_pokemon_broker( socket_broker, pokemon, pos_x, pos_y, cantidad);
}


int BROKER_APPEARED_POKEMON(char* pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t id_mensaje){
	int size_pokemon=strlen(pokemon)+1;
	int peso_total=sizeof(int)+size_pokemon+sizeof(uint32_t)*3;
	int offset=0;

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,&pos_x,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&pos_y,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&size_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,pokemon,size_pokemon);
	offset+=size_pokemon;

	void*paquete=crear_paquete(APPEARED_POKEMON,datos);
	int socket_broker = conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	enviar_paquete(socket_broker,paquete);
	free(datos);
	free(paquete);
	return 1;
}

int BROKER_CATCH_POKEMON(char* pokemon, uint32_t pos_x,uint32_t  pos_y){

	int socket_broker = conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	return catch_pokemon_broker(socket_broker,pokemon, pos_x, pos_y);

}


int BROKER_CAUGHT_POKEMON(uint32_t id_mensaje, bool ok_fail){ //La variable esta es 255 en un OK y en un FAIL
	int peso_total=sizeof(uint32_t)+sizeof(uint32_t);
	int offset=0;
	uint32_t bool_uint;

	if(ok_fail) {
		bool_uint = 1;
	} else {
		bool_uint = 0;
	}

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,&bool_uint,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	void*paquete=crear_paquete(CAUGHT_POKEMON,datos);
	int socket_broker = conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	enviar_paquete(socket_broker,paquete);
	free(datos);
	free(paquete);


	return 1;
}


int BROKER_GET_POKEMON(char* pokemon){
	int socket_broker = conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	return get_pokemon(socket_broker, pokemon);
}

int TEAM_APPEARED_POKEMON(char* pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t id_mensaje){
	int size_pokemon=strlen(pokemon)+1;
	int peso_total=sizeof(int)+size_pokemon+sizeof(uint32_t)*3;
	int offset=0;

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,&pos_x,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&pos_y,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(datos+offset,&size_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,pokemon,size_pokemon);
	offset+=size_pokemon;

	void*paquete=crear_paquete(APPEARED_POKEMON,datos);
	int socket_team = conectar_con_servidor(IP_TEAM,PUERTO_TEAM);
	enviar_paquete(socket_team,paquete);
	free(datos);
	free(paquete);
	return 1;
}

int GAMECARD_NEW_POKEMON(char* pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t cantidad,uint32_t id_mensaje){
	int socket_game_card = conectar_con_servidor(IP_GAME_CARD,PUERTO_GAME_CARD);
	return new_pokemon_gamecard( socket_game_card, pokemon, pos_x, pos_y, cantidad,id_mensaje);
}
int GAMECARD_CATCH_POKEMON(char* pokemon,uint32_t  pos_x,uint32_t  pos_y, uint32_t  id_mensaje){
	int socket_game_card = conectar_con_servidor(IP_GAME_CARD,PUERTO_GAME_CARD);
	return catch_pokemon_gamecard(socket_game_card,pokemon, pos_x, pos_y, id_mensaje);
}
int GAMECARD_GET_POKEMON(char* pokemon,uint32_t id_mensaje){
	int socket_game_card = conectar_con_servidor(IP_GAME_CARD,PUERTO_GAME_CARD);
	return get_pokemon_gamecard(socket_game_card,pokemon,id_mensaje);
}

int new_pokemon_gamecard(int socket,char* pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t cantidad, uint32_t id_mensaje){
	int size_pokemon=strlen(pokemon)+1;
	int peso_total=sizeof(int)+size_pokemon+sizeof(uint32_t)*3+sizeof(uint32_t);
	int offset=0;

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,&pos_x,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&pos_y,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&cantidad,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&size_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,pokemon,size_pokemon);
	offset+=size_pokemon;

	void*paquete=crear_paquete(NEW_POKEMON,datos);
	enviar_paquete(socket,paquete);
	free(datos);
	free(paquete);
	return 1;

}


int new_pokemon_broker(int socket,char* pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t cantidad){
	int size_pokemon=strlen(pokemon)+1;
	int peso_total=sizeof(int)+size_pokemon+sizeof(uint32_t)*3;
	int offset=0;

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,&pos_x,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&pos_y,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&cantidad,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&size_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,pokemon,size_pokemon);
	offset+=size_pokemon;

	void*paquete=crear_paquete(NEW_POKEMON,datos);
	enviar_paquete(socket,paquete);
	free(datos);
	free(paquete);
	return 1;

}

int catch_pokemon_gamecard(int socket,char*pokemon,uint32_t pos_x,uint32_t pos_y,uint32_t id_mensaje ){
	int size_pokemon=strlen(pokemon)+1;
	int peso_total=sizeof(int)+size_pokemon+sizeof(uint32_t)*2+sizeof(uint32_t);
	int offset=0;

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,&pos_x,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&pos_y,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&size_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,pokemon,size_pokemon);
	offset+=size_pokemon;

	void*paquete=crear_paquete(CATCH_POKEMON,datos);
	enviar_paquete(socket,paquete);
	free(datos);
	free(paquete);
	return 1;
}

int catch_pokemon_broker(int socket,char*pokemon,uint32_t pos_x,uint32_t pos_y){
	int size_pokemon=strlen(pokemon)+1;
	int peso_total=sizeof(int)+size_pokemon+sizeof(uint32_t)*2;
	int offset=0;

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,&pos_x,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&pos_y,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&size_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,pokemon,size_pokemon);
	offset+=size_pokemon;

	void*paquete=crear_paquete(CATCH_POKEMON,datos);
	enviar_paquete(socket,paquete);
	free(datos);
	free(paquete);
	return 1;
}

int get_pokemon_gamecard(int socket,char*pokemon,uint32_t id_mensaje){

	int size_pokemon=strlen(pokemon)+1;
	int peso_total=sizeof(int)+size_pokemon+sizeof(uint32_t);
	int offset=0;

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,&id_mensaje,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	memcpy(datos+offset,&size_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,pokemon,size_pokemon);
	offset+=size_pokemon;

	void*paquete=crear_paquete(GET_POKEMON,datos);
	enviar_paquete(socket,paquete);
	free(datos);
	free(paquete);

	return 1;

}

int get_pokemon(int socket,char*pokemon){

	int size_pokemon=strlen(pokemon)+1;
	int peso_total=sizeof(int)+size_pokemon;
	int offset=0;
	//uint32_t id_mensaje = 0; //ESTO LO HABIAMOS HARDCODEADO

	void*datos=malloc(peso_total+sizeof(int));
	memcpy(datos+offset,&peso_total,sizeof(int));
	offset+=sizeof(int);
	//memcpy(datos+offset,&id_mensaje,sizeof(uint32_t));
	//offset+=sizeof(uint32_t);
	memcpy(datos+offset,&size_pokemon,sizeof(int));
	offset+=sizeof(int);
	memcpy(datos+offset,pokemon,size_pokemon);
	offset+=size_pokemon;

	void*paquete=crear_paquete(GET_POKEMON,datos);
	enviar_paquete(socket,paquete);
	free(datos);
	free(paquete);

	return 1;

}


void loggear_suscripcion_a(char* nombre_cola,uint32_t tiempo){
	char* numero_string=string_itoa(tiempo);
	char*msj=malloc(strlen(numero_string)+1+43+strlen(nombre_cola)+1);
	msj=strcat(strcat(strcat(strcat("Me suscribi a la cola de mensajes ", nombre_cola)," durante "),numero_string),"\n");
	log_info(logger_obligatorio,msj);
	free(msj);
	free(numero_string);
}

void loggear_llegada_de_mensajes(char* cola){
	char*msj=malloc(30+strlen(cola)+1);
	msj=strcat(strcat("Me llego el mensaje de la cola", cola),"\n");
	log_info(logger_obligatorio,msj);
	free(msj);
}

void loggear_envio_de_mensajes(char* receptor){
	char*msj=malloc(21+strlen(receptor)+1);
	msj=strcat(strcat("Le envie un mensaje a", receptor),"\n");
	log_info(logger_obligatorio,msj);
	free(msj);
}

uint32_t get_timestamp() {
	return (unsigned)time(NULL);
}

void recibir_request(int socket, uint32_t tiempo){

	long tiempo_limite = get_timestamp() + tiempo;

	void* buffer;
	int alocador;

	buffer = recibir_paquete(&alocador,socket);

	while(alocador>0){

		ejecutar_operacion(buffer,socket);
		free(buffer);
		buffer = recibir_paquete(&alocador,socket);

		if(get_timestamp() >= tiempo_limite)
			break;
	}

	close(socket);
}

int obtener_cola(char* nombre_cola){

	if(!strcmp(nombre_cola,"NEW_POKEMON")){
		return NEW_POKEMON;
	}else if (!strcmp(nombre_cola,"APPEARED_POKEMON")){
		return APPEARED_POKEMON;
	}else if (!strcmp(nombre_cola,"GET_POKEMON")){
		return GET_POKEMON;
	}else if (!strcmp(nombre_cola,"LOCALIZED_POKEMON")){
		return LOCALIZED_POKEMON;
	}else if (!strcmp(nombre_cola,"CATCH_POKEMON")){
		return CATCH_POKEMON;
	}else if (!strcmp(nombre_cola,"CAUGHT_POKEMON")){
		return CAUGHT_POKEMON;
	}
	return -57;
}

long calcular_id(){

	t_config* un_config = config_create("config.config");

	long retorno = 0;
	char* IP_GAMEBOY = config_get_string_value(un_config,"IP_GAMEBOY");
	char** aux = string_split(IP_TEAM,".");
	//127.0.0.1
	//127001
	long PUERTO_GAMEBOY = config_get_long_value(un_config, "PUERTO_GAMEBOY");

	for(int i=3;i>0;i--){
		retorno+=(atoi(aux[i])*pow(10,3-i));
	}

	retorno += PUERTO_GAMEBOY;

	config_destroy(un_config);
	free(aux);

	return retorno;
}

struct parametros_suscribirse {
	uint32_t tiempo;
	char* nombre_cola;
};

void suscribirse(void* parametros_recibidos){

	struct parametros_suscribirse* parametros = (struct parametros_suscribirse*) parametros_recibidos;

	int peso_ip= strlen(IP_SUSCRIPTOR)+1;
	int peso=sizeof(int)+sizeof(long)+peso_ip+sizeof(int)*2;
	int offset=0;
	long id = calcular_id();
	int cola = 0;

	if(strcmp(parametros->nombre_cola, "NEW_POKEMON") == 0) cola = NEW_POKEMON;
	if(strcmp(parametros->nombre_cola, "APPEARED_POKEMON") == 0) cola = APPEARED_POKEMON;
	if(strcmp(parametros->nombre_cola, "CATCH_POKEMON") == 0) cola = CATCH_POKEMON;
	if(strcmp(parametros->nombre_cola, "CAUGHT_POKEMON") == 0) cola = CAUGHT_POKEMON;
	if(strcmp(parametros->nombre_cola, "GET_POKEMON") == 0) cola = GET_POKEMON;
	if(strcmp(parametros->nombre_cola, "LOCALIZED_POKEMON") == 0) cola = LOCALIZED_POKEMON;

	void* paquete= malloc(peso+sizeof(int));
	memcpy(paquete,&peso,sizeof(int));
	offset+=sizeof(int);
	memcpy(paquete+offset,&cola,sizeof(int));
	offset+=sizeof(int);
	memcpy(paquete+offset,&id,sizeof(long));
	offset+=sizeof(long);
	memcpy(paquete+offset,&peso_ip,sizeof(int));
	offset+=sizeof(int);
	memcpy(paquete+offset,IP_SUSCRIPTOR,peso_ip);
	offset+=peso_ip;
	memcpy(paquete+offset,&PUERTO_SUSCRIPTOR,sizeof(int));

	void* para_envio=crear_paquete(SUSCRIPTOR,paquete);
	int socket_broker = conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	enviar_paquete(socket_broker,para_envio);
	//sem_wait(&SEM_LOGS); mepa que gameboy no tiene estos semaforos
	log_info(log_operaciones,"Me suscribi a la cola %s", parametros->nombre_cola);
	//sem_post(&SEM_LOGS);
	free(para_envio);
	free(paquete);
}

int conectarse_como_SUSCRIPTOR(char* nombre_cola, uint32_t tiempo){

	pthread_t* hilo_suscriptor;

	struct parametros_suscribirse* parametros = (struct parametros_suscribirse*)malloc(sizeof(struct parametros_suscribirse));
	parametros->nombre_cola = (char*)malloc(strlen(nombre_cola)+1);
	memcpy(parametros->nombre_cola, nombre_cola, strlen(nombre_cola)+1);
	parametros->tiempo = tiempo;

	pthread_create(&hilo_suscriptor, NULL, (void*)suscribirse, parametros);
	pthread_detach(hilo_suscriptor);

	//Con sleep --> NO paralelo
	//Sin sleep --> Queda la escucha pero puedo paralelizar cosas

	sleep(tiempo);

	return 1;

}

