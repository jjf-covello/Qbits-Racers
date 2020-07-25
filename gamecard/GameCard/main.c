/*
 * main.c
 *
 *  Created on: 7 abr. 2020
 *      Author: utnso
 */

#include "main.h"
#include <math.h>
pthread_t* hilo_suscripcion_new;
pthread_t* hilo_suscripcion_catch;
pthread_t* hilo_suscripcion_get;

int main(int argc, char** argv){

	remove("log_server.log");
	remove("log_obligatorio.log");
	log_server=log_create("log_server.log","log_server",0,LOG_LEVEL_INFO);
	log_obligatorio=log_create("log_obligatorio.log","log_obligatorio",0,LOG_LEVEL_INFO);

	levantar_file_system();
	server_levantado=true;
	broker_sigue_vivo = true;
	inicializar_suscripciones();

	/*
	pthread_t* hilo_broker_sigue_vivo;

	pthread_create(&hilo_broker_sigue_vivo,NULL,tocar_a_broker,NULL);
	pthread_detach(hilo_broker_sigue_vivo);
	 */

	levantar_server_multithread();//no se si deberia ser multithread o single thread

	return 0;
}

void inicializar_suscripciones(){

	for(int i=0; i<3; i++){
		suscripcion[i]=0;
	}

	pthread_create(&hilo_suscripcion_new,NULL,suscribirse,NEW_POKEMON);
	pthread_detach(hilo_suscripcion_new);

	pthread_create(&hilo_suscripcion_catch,NULL,suscribirse,CATCH_POKEMON);
	pthread_detach(hilo_suscripcion_catch);

	pthread_create(&hilo_suscripcion_get,NULL,suscribirse,GET_POKEMON);
	pthread_detach(hilo_suscripcion_get);
}


void levantar_file_system(){

	leer_archivo_configuracion();

	if(!file_system_levantado()){

		path_metadata = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+26); //ACA VA UN 23
		memset(path_metadata,'\0',strlen(PUNTO_MONTAJE_TALLGRASS)+26); //ACA VA UN 23
		strcpy(path_metadata,PUNTO_MONTAJE_TALLGRASS);
		path_metadata = strcat(path_metadata,"/Metadata");

		mkdir(path_metadata, S_IRWXU);
		path_metadata=strcat(path_metadata,"/Metadata.bin");
		FILE* arch_metadata = fopen(path_metadata,"wb");
		fclose(arch_metadata);

		t_config* config_metadata = config_create(path_metadata);
		config_set_value(config_metadata,"MAGIC_NUMBER",MAGIC_NUMBER);
		char* block_size = string_itoa(BLOCK_SIZE);
		char* blocks = string_itoa(BLOCKS);
		config_set_value(config_metadata,"BLOCKS",blocks);
		config_set_value(config_metadata,"BLOCK_SIZE",block_size);
		config_save(config_metadata);
		config_destroy(config_metadata);

		crear_bitmap();
		setear_bitmap();

		path_files = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+9); //ACA VA UN 7
		strcpy(path_files,PUNTO_MONTAJE_TALLGRASS);
		path_files = strcat(path_files,"/Files");
		mkdir(path_files, S_IRWXU);

		path_pokemon = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+4+strlen("/Files/Pokemon")); //ACA VA UN 2
		strcpy(path_pokemon,PUNTO_MONTAJE_TALLGRASS);
		path_pokemon = strcat(path_pokemon,"/Files/Pokemon");
		path_pokemon_efectivo = malloc(strlen(path_pokemon)+4+strlen("/Metadata.bin"));//ACA VA UN 2
		strcpy(path_pokemon_efectivo,path_pokemon);

		mkdir(path_pokemon, S_IRWXU);
		path_pokemon_efectivo=strcat(path_pokemon_efectivo,"/Metadata.bin");
		FILE* arch_metadata_pokemon = fopen(path_pokemon_efectivo,"wb");
		fclose(arch_metadata_pokemon);

		t_config* config_metadata_pokemon = config_create(path_pokemon_efectivo);
		config_set_value(config_metadata_pokemon,"DIRECTORY","Y");
		config_save(config_metadata_pokemon);
		config_destroy(config_metadata_pokemon);

		path_blocks = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+10); //ACA VA UN 8
		strcpy(path_blocks,PUNTO_MONTAJE_TALLGRASS);
		path_blocks = strcat(path_blocks,"/Blocks");
		mkdir(path_blocks, S_IRWXU);

		for(int i=0; i<BLOCKS; i++){
			char* path_bloque = malloc(strlen(path_blocks)+10); //Aca va un 7
			char* numero_string = string_itoa(i+1);
			strcpy(path_bloque,path_blocks);
			path_bloque=strcat(path_bloque,"/");
			path_bloque=strcat(path_bloque,numero_string);
			path_bloque=strcat(path_bloque,".bin");
			FILE* arch_block = fopen(path_bloque,"wb");
			for(int j=0; j<BLOCK_SIZE; j++){
				fwrite("-",1,1,arch_block);
			}
			fclose(arch_block);
			free(numero_string);
			free(path_bloque);
		}
		free(block_size);
		free(blocks);
	}else{

		path_metadata = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+26); //ACA VA UN 23
		memset(path_metadata,'\0',strlen(PUNTO_MONTAJE_TALLGRASS)+26); //ACA VA UN 23
		strcpy(path_metadata,PUNTO_MONTAJE_TALLGRASS);
		path_metadata = strcat(path_metadata,"/Metadata");

		path_metadata=strcat(path_metadata,"/Metadata.bin");

		recrear_bitmap();

		path_files = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+9); //ACA VA UN 7
		strcpy(path_files,PUNTO_MONTAJE_TALLGRASS);
		path_files = strcat(path_files,"/Files");

		path_pokemon = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+4+strlen("/Files/Pokemon")); //ACA VA UN 2
		strcpy(path_pokemon,PUNTO_MONTAJE_TALLGRASS);
		path_pokemon = strcat(path_pokemon,"/Files/Pokemon");
		path_pokemon_efectivo = malloc(strlen(path_pokemon)+4+strlen("/Metadata.bin"));//ACA VA UN 2
		strcpy(path_pokemon_efectivo,path_pokemon);


		path_pokemon_efectivo=strcat(path_pokemon_efectivo,"/Metadata.bin");

		path_blocks = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+10); //ACA VA UN 8
		strcpy(path_blocks,PUNTO_MONTAJE_TALLGRASS);
		path_blocks = strcat(path_blocks,"/Blocks");
	}

}

void leer_archivo_configuracion(){

	config_server=config_create("config.config");
	TIEMPO_DE_REINTENTO_CONEXION=config_get_int_value(config_server,"TIEMPO_DE_REINTENTO_CONEXION");
	TIEMPO_DE_REINTENTO_OPERACION=config_get_int_value(config_server,"TIEMPO_DE_REINTENTO_OPERACION");
	PUNTO_MONTAJE_TALLGRASS=malloc(strlen(config_get_string_value(config_server,"PUNTO_MONTAJE_TALLGRASS"))+1);
	strcpy(PUNTO_MONTAJE_TALLGRASS,config_get_string_value(config_server,"PUNTO_MONTAJE_TALLGRASS"));
	char* auxIP=config_get_string_value(config_server,"IP_BROKER");
	IP_BROKER = malloc(strlen(auxIP)+1);
	strcpy(IP_BROKER, auxIP);
	PUERTO_BROKER=config_get_int_value(config_server,"PUERTO_BROKER");
	BLOCK_SIZE=config_get_int_value(config_server,"BLOCK_SIZE");
	BLOCKS=config_get_int_value(config_server,"BLOCKS");

	char* aux = config_get_string_value(config_server,"MAGIC_NUMBER");
	MAGIC_NUMBER = malloc(strlen(aux)+1);
	strcpy(MAGIC_NUMBER,aux);

	PUERTO_ESCUCHA= config_get_int_value(config_server,"PUERTO_GAMECARD");
	/*auxIP = config_get_string_value(config_server,"IP_GAMECARD");
	IP = malloc(strlen(auxIP)+1);
	strcpy(IP,auxIP);*/

	PUERTO_SUSCRIPTOR= config_get_int_value(config_server,"PUERTO_SUSCRIPTOR");
	auxIP = config_get_string_value(config_server,"IP_SUCRIPTOR");
	IP_SUSCRIPTOR = malloc(strlen(auxIP)+1);
	strcpy(IP_SUSCRIPTOR,auxIP);

	config_destroy(config_server);
}

bool file_system_levantado(){
	mkdir(PUNTO_MONTAJE_TALLGRASS, S_IRWXU);
	bool flag=(errno==EEXIST);
	return flag;
}

void crear_bitmap(){

	char* path_bitmap = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+27); //ACA VA UN 21
	strcpy(path_bitmap,PUNTO_MONTAJE_TALLGRASS);
	path_bitmap = strcat(path_bitmap,"/Metadata");

	path_bitmap = strcat(path_bitmap,"/Bitmap.bin");
	FILE* arch_bitmap = fopen(path_bitmap,"wb");

	int cantidad_bloques= BLOCKS;
	if((cantidad_bloques % 8) == 0) {
		cantidad_bloques = cantidad_bloques / 8;
	} else {
		cantidad_bloques = (cantidad_bloques / 8) + 1;
	}
	char* vector_bloques = (char*)malloc(cantidad_bloques);
	memset(vector_bloques,'\0',cantidad_bloques);
	fwrite(vector_bloques,cantidad_bloques,1,arch_bitmap);
	free(vector_bloques);
	fclose(arch_bitmap);

	int file_desc_bitarray = open(path_bitmap, O_RDWR, S_IRUSR | S_IWUSR);
	char* array_de_bits = mmap(NULL, cantidad_bloques, PROT_READ | PROT_WRITE, MAP_SHARED, file_desc_bitarray, 0);
	bitmap = bitarray_create_with_mode(array_de_bits,cantidad_bloques,LSB_FIRST);
	free(path_bitmap);

}

void setear_bitmap(){

	for(int i=0; i<BLOCKS; i++){
		bitarray_clean_bit(bitmap, i);
	}
}

void recrear_bitmap(){

	char* path_bitmap = malloc(strlen(PUNTO_MONTAJE_TALLGRASS)+27); //ACA VA UN 21
	strcpy(path_bitmap,PUNTO_MONTAJE_TALLGRASS);
	path_bitmap = strcat(path_bitmap,"/Metadata");
	path_bitmap = strcat(path_bitmap,"/Bitmap.bin");

	int cantidad_bloques= BLOCKS;
	if((cantidad_bloques % 8) == 0) {
		cantidad_bloques = cantidad_bloques / 8;
	} else {
		cantidad_bloques = (cantidad_bloques / 8) + 1;
	}

	int file_desc_bitarray = open(path_bitmap, O_RDWR, S_IRUSR | S_IWUSR);
	char* array_de_bits = mmap(NULL, cantidad_bloques, PROT_READ | PROT_WRITE, MAP_SHARED, file_desc_bitarray, 0);
	bitmap = bitarray_create_with_mode(array_de_bits,cantidad_bloques,LSB_FIRST);
	free(path_bitmap);
}
/*
void suscribirse_new(){
	suscribirse(NEW_POKEMON);
}
void suscribirse_catch(){
	suscribirse(CATCH_POKEMON);
}
void suscribirse_get(){
	suscribirse(GET_POKEMON);
}*/

void tocar_a_broker(){

	sleep(15);

	while(server_levantado){

		sleep(TIEMPO_DE_REINTENTO_CONEXION);
		int socket_escucha = conectar_con_servidor(IP_BROKER,PUERTO_BROKER);

		if(socket_escucha<3){
			broker_sigue_vivo=false;
		}else{
			int peso_de_envio = sizeof(int);
			int codigo_operacion = 108;

			void* paquete;
			paquete= malloc(peso_de_envio+sizeof(int));

			memcpy(paquete,&peso_de_envio,sizeof(int));
			memcpy(paquete+sizeof(int),&codigo_operacion,sizeof(int));

			enviar_paquete2(socket_escucha,paquete);

			free(paquete);

			close(socket_escucha);
		}

		evaluar_posible_kill_de_suscripciones();
	}
}

void enviar_paquete2(int socket, void* paquete_serializado){

	int peso_paquete=0;
	memcpy(&peso_paquete,paquete_serializado,sizeof(int));
	broker_sigue_vivo = send(socket,paquete_serializado,peso_paquete+sizeof(int),0) == peso_paquete+sizeof(int);

}


void evaluar_posible_kill_de_suscripciones(){

	if(!broker_sigue_vivo){
		log_info(log_obligatorio,"Broker no esta conectado, por lo tanto vamos a tratar de conectarnos nuevamente");
		pthread_cancel(hilo_suscripcion_new);
		pthread_cancel(hilo_suscripcion_catch);
		pthread_cancel(hilo_suscripcion_get);
		inicializar_suscripciones();
	}
}

void suscribirse(int cola){

	int socket_escucha=0;

	//while(server_levantado){

	socket_escucha=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);

	while(socket_escucha<3){ //&& server_levantado
		//sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"No pudimos conectarnos con Broker (socket= %d)",socket_escucha);
		//sem_post(&SEM_LOGS);
		sleep(TIEMPO_DE_REINTENTO_CONEXION);

		//sem_wait(&SEM_LOGS);
		log_info(log_obligatorio,"Vamos a volver a conectar a la cola %d, despues de %d (s) ",cola,TIEMPO_DE_REINTENTO_CONEXION);
		//sem_post(&SEM_LOGS);
		socket_escucha=conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	}
	switch(cola){

	case NEW_POKEMON:
		suscripcion[0]=socket_escucha;
		break;
	case CATCH_POKEMON:
		suscripcion[1]=socket_escucha;
		break;
	case GET_POKEMON:
		suscripcion[2]=socket_escucha;
		break;

	}
	//sem_wait(&SEM_LOGS);
	log_info(log_obligatorio,"Pudimos conectarnos con Broker (socket= %d)!",socket_escucha);
	//sem_post(&SEM_LOGS);

	enviar_suscripcion(socket_escucha,cola);
	int retorno=0;
	/*while(server_levantado && retorno!=57){
		retorno=atender_suscripcion(socket_escucha);
		if(retorno!=57 && broker_sigue_vivo){
			enviar_ack(socket_escucha);
		}
		//5138008
	}*/
	close(socket_escucha);

	//}
}

void enviar_ack(int socket_escucha){
	int ack=5138008;
	void* paquete= malloc(sizeof(int));
	memcpy(paquete,&ack,sizeof(int));
	send(socket_escucha,paquete,sizeof(int),0);
	free(paquete);
}


void enviar_fin_de_suscripcion(int socket_escucha,int cola){
	int peso=sizeof(int)+sizeof(long);
	int offset=0;
	long id = calcular_id();

	void* paquete= malloc(peso+sizeof(int));
	memcpy(paquete,&peso,sizeof(int));
	offset+=sizeof(int);
	memcpy(paquete+offset,&cola,sizeof(int));
	offset+=sizeof(int);
	memcpy(paquete+offset,&id,sizeof(long));

	void* para_envio=crear_paquete(TERMINAR_SUSCRIPCION,paquete);
	enviar_paquete(socket_escucha,para_envio);
	//sem_wait(&SEM_LOGS);
	log_info(log_obligatorio,"Ya no estoy suscripto a la cola %d",cola);
	//sem_post(&SEM_LOGS);
	free(para_envio);
	free(paquete);



}
void enviar_suscripcion(int socket_escucha, int cola){

	int offset=0;
	long id = calcular_id();
	int peso_ip = strlen(IP_SUSCRIPTOR)+1;
	int peso = 3 * sizeof(int) + sizeof(long) + peso_ip;

	void* paquete= malloc(peso + sizeof(int));
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
	enviar_paquete(socket_escucha,para_envio);
	//sem_wait(&SEM_LOGS);
	//log_info(log_obligatorio,"Me suscribi a la cola %d",cola);
	//sem_post(&SEM_LOGS);
	free(para_envio);
	free(paquete);
}

long calcular_id(){

	t_config* un_config = config_create("config.config");

	long retorno = 0;
	char* IP_GAMECARD = config_get_string_value(un_config,"IP_GAMECARD");
	char** aux = string_split(IP_GAMECARD,".");
	//127.0.0.1
	//127001
	long PUERTO_GAMECARD = config_get_long_value(un_config, "PUERTO_GAMECARD");

	for(int i=3;i>0;i--){
		retorno+=(atoi(aux[i])*pow(10,3-i));
	}

	retorno += PUERTO_GAMECARD;

	liberar_char_doble_puntero(aux);

	config_destroy(un_config);

	return retorno;
}
