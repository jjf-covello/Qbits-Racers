/*
 * funciones_auxiliares.c
 *
 *  Created on: 17 abr. 2020
 *      Author: utnso
 */
#include "funciones_auxiliares.h"
bool buscar_pokemon_en_fs(char*nombre_pokemon){

	bool flag=false;
	struct dirent *dir;
	DIR* directorio_pokemon= opendir(path_pokemon);
	while ((dir = readdir(directorio_pokemon)) != NULL){
		if(!strcmp(dir->d_name,nombre_pokemon)){
			flag=true;
			break;
		}
	}
	closedir(directorio_pokemon);
	return flag;
}

char* obtener_path_metadata(char* nombre_pokemon){

	int peso_path = strlen(path_pokemon)+10+strlen(nombre_pokemon)+strlen("/Metadata.bin"); //ACA VA UN 6
	char*path_archivo=malloc(peso_path);
	strcpy(path_archivo,path_pokemon);
	path_archivo=strcat(path_archivo,"/");
	path_archivo=strcat(path_archivo,nombre_pokemon);
	path_archivo=strcat(path_archivo,"/Metadata.bin");
	return path_archivo;

}

bool esta_abierto_archivo(char*nombre_pokemon){

	bool flag=false;
	char* path_archivo = obtener_path_metadata(nombre_pokemon);
	t_config* config_metadata= config_create(path_archivo); //TODO
	char* value=config_get_string_value(config_metadata,"OPEN");
	flag=!strncmp("Y",value,1);
	config_destroy(config_metadata);
	free(path_archivo);	
	return flag;

}

char* obtener_path_metadata_pokemon_especifico(char* nombre_pokemon){

	char*path_archivo=malloc(strlen(path_pokemon)+8+strlen(nombre_pokemon)+strlen("/Metadata.bin")); //ACA VA UN 4
	strcpy(path_archivo,path_pokemon);
	path_archivo=strcat(path_archivo,"/");
	path_archivo=strcat(path_archivo,nombre_pokemon);
	path_archivo=strcat(path_archivo,"/Metadata.bin");
	return path_archivo;
}

bool verificar_posiciones(uint32_t pos_x,uint32_t pos_y,char*nombre_pokemon){

	char*path_archivo=obtener_path_metadata_pokemon_especifico(nombre_pokemon);
	t_config* config_metadata= config_create(path_archivo);
	char** bloques=config_get_array_value(config_metadata,"BLOCKS");
	int i=0;
	bool posicion_encontrada=false;
	char* pos_x_string = string_itoa(pos_x);
	char* pos_y_string = string_itoa(pos_y);
	int size=3+strlen(pos_x_string)+strlen(pos_y_string)+2;

	free(pos_x_string);
	free(pos_y_string);

	while(bloques[i]!=NULL && !posicion_encontrada){
		int offset_registro=0;
		char* path_bloque_especifico=obtener_path_bloque_especifico(bloques[i]);
		int arch_bloque= open(path_bloque_especifico,O_RDWR);
		void*bloque_map=mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, arch_bloque, 0);
		int offset=0;
		char** registros = string_split(bloque_map,"\n");

		while(offset<BLOCK_SIZE && !posicion_encontrada && registros[offset_registro] != NULL){

			offset+=strlen(registros[offset_registro]);

			if(!string_ends_with(registros[offset_registro],"-")){
				char** banana_split = string_split(registros[offset_registro]," ");
				int* dimensiones_banana_split = obtener_dimensiones_array(banana_split);
				if(dimensiones_banana_split[0] == 3){
					//Toda la info esta en un mismo bloque
					tupla_t* tupla=deserializar_registro(registros[offset_registro]);
					posicion_encontrada=posicion_indicada(tupla,pos_x,pos_y);
					free(tupla);
				}else{
					//La info esta repartida en distintos bloques
					if(offset_registro != 0){
						char* path_bloque_splitted=obtener_path_bloque_especifico(bloques[i+1]);
						int arch_bloque_splitted= open(path_bloque_splitted,O_RDWR);
						void*bloque_map_splitted=mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, arch_bloque_splitted, 0);
						char** registros_splitted = string_split(bloque_map_splitted,"\n");

						char* appender = string_new();

						string_append(&appender,registros[offset_registro]);
						string_append(&appender,registros_splitted[0]);

						tupla_t* tupla=deserializar_registro(appender);
						posicion_encontrada=posicion_indicada(tupla,pos_x,pos_y);
						free(tupla);
						free(appender);
						liberar_char_doble_puntero(registros_splitted);
						munmap(bloque_map_splitted,BLOCK_SIZE);
						close(arch_bloque_splitted);
						free(path_bloque_splitted);
					}
				}
				liberar_char_doble_puntero(banana_split);
				free(dimensiones_banana_split);

			}

			offset_registro++;
		}
		liberar_char_doble_puntero(registros);
		munmap(bloque_map, BLOCK_SIZE);
		close(arch_bloque);
		free(path_bloque_especifico);
		i++;

	}
	config_destroy(config_metadata);
	int j=0;
	while(bloques[j]!=NULL){
		free(bloques[j]);
		j++;
	}
	free(bloques);
	free(path_archivo);
	return posicion_encontrada;
}

void enviar_respuesta_broker(void* paquete_a_mandar){

	/*En caso que no se pueda realizar la conexión con el Broker se
			debe informar por logs y continuar la ejecución.*/
	int codigo_de_operacion;
	int peso_a_enviar;
	memcpy(&peso_a_enviar,paquete_a_mandar,sizeof(int));
	memcpy(&codigo_de_operacion,paquete_a_mandar+sizeof(int),sizeof(int));
	int socket = conectar_con_servidor(IP_BROKER,PUERTO_BROKER);
	int peso_enviado = enviar_paquete(socket, paquete_a_mandar);
	//DUDOUSI ESTE IF
	if(peso_enviado == peso_a_enviar+4){

	}else{
		error_envio_de_paquete(codigo_de_operacion);
	}
	close(socket);

}

void crear_paquete_error_new_pokemon(){

	void* paquete = malloc(sizeof(int));
	int peso = 0;

	memcpy(paquete,&peso,sizeof(int));
	crear_paquete(CATCH_POKEMON_FALLO, paquete);

}

void crear_paquete_error_catch_pokemon(){

	void* paquete = malloc(sizeof(int));
	int peso = 0;

	memcpy(paquete,&peso,sizeof(int));
	crear_paquete(CATCH_POKEMON_FALLO, paquete);

}

void crear_paquete_error_get_pokemon(){

	void* paquete = malloc(sizeof(int));
	int peso = 0;

	memcpy(paquete,&peso,sizeof(int));
	crear_paquete(GET_POKEMON_FALLO, paquete);

}

void error_envio_de_paquete(int codigo_de_operacion){

	switch(codigo_de_operacion){

	case NEW_POKEMON:
		crear_paquete_error_new_pokemon();
		log_info(log_obligatorio,"Tuvimos un problema con el NEW_POKEMON");
		break;

	case CATCH_POKEMON:
		crear_paquete_error_catch_pokemon();
		log_info(log_obligatorio,"Tuvimos un problema con el CATCH_POKEMON");
		break;

	case GET_POKEMON:
		crear_paquete_error_get_pokemon();
		log_info(log_obligatorio,"Tuvimos un problema con el GET_POKEMON");
		break;

	default:
		log_info(log_cod_invalido,"Codigo Invalido");
		break;
	}

}

char* obtener_path_bloque_especifico(char* bloques){

	char* aux = malloc(strlen(bloques)+1);
	strcpy(aux,bloques);
	char* path_bloque_especifico=malloc(strlen(path_blocks)+strlen(bloques)+strlen("/.bin")+10); //Aca va un 3
	strcpy(path_bloque_especifico,path_blocks);
	path_bloque_especifico=strcat(path_bloque_especifico,"/");
	path_bloque_especifico=strcat(path_bloque_especifico,aux);
	path_bloque_especifico=strcat(path_bloque_especifico,".bin");
	free(aux);
	return path_bloque_especifico;
}

t_list* obtener_todas_las_posiciones(char* nombre_pokemon){

	t_list *lista_de_posiciones=list_create();
	char*path_archivo=obtener_path_metadata(nombre_pokemon);
	t_config* config_metadata= config_create(path_archivo);
	char** bloques=config_get_array_value(config_metadata,"BLOCKS");
	int i=0;

	while(bloques[i]!=NULL){
		char* path_bloque_especifico=obtener_path_bloque_especifico(bloques[i]);
		int arch_bloque= open(path_bloque_especifico,O_RDWR);
		void*bloque_map=mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, arch_bloque, 0);

		char** todos_los_registros = string_split(bloque_map,"\n");
		int numero_de_registro = 0;

		char** primer_registro_spliteado = string_split(todos_los_registros[numero_de_registro]," ");

		while(todos_los_registros[numero_de_registro] != NULL){

			tupla_t* tupla;
			char** banana_split = string_split(todos_los_registros[numero_de_registro]," ");
			int* dimensiones_banana_split = obtener_dimensiones_array(banana_split);

			if(dimensiones_banana_split[0] == 3){
				if(!string_ends_with(todos_los_registros[numero_de_registro], "-")){
					tupla=deserializar_registro(todos_los_registros[numero_de_registro]);
					list_add(lista_de_posiciones,tupla);
				}
			}else{
				//Registro partido
				if(numero_de_registro != 0 && !string_ends_with(todos_los_registros[numero_de_registro], "-")){

					char* aux = string_new();

					char* path_bloque_aux=obtener_path_bloque_especifico(bloques[i+1]);
					int arch_aux= open(path_bloque_aux,O_RDWR);
					void*bloque_map_aux=mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, arch_aux, 0);

					char** registro_spliteado = string_split(bloque_map_aux,"\n");

					string_append(&aux, todos_los_registros[numero_de_registro]);
					string_append(&aux,registro_spliteado[0]);

					tupla = deserializar_registro(aux);

					free(aux);
					close(arch_aux);
					munmap(bloque_map_aux,BLOCK_SIZE);
					free(path_bloque_aux);
					liberar_char_doble_puntero(registro_spliteado);

					list_add(lista_de_posiciones,tupla);
				}
			}

			liberar_char_doble_puntero(banana_split);
			free(dimensiones_banana_split);

			numero_de_registro++;
		}

		liberar_char_doble_puntero(todos_los_registros);
		liberar_char_doble_puntero(primer_registro_spliteado);
		close(arch_bloque);
		free(path_bloque_especifico);
		i++;
	}
	liberar_char_doble_puntero(bloques);
	config_destroy(config_metadata);
	free(path_archivo);
	return lista_de_posiciones;
}

tupla_t* deserializar_registro(char* registro){
	tupla_t* tupla= (tupla_t*)malloc(sizeof(tupla_t));

	char** datos = string_split(registro," ");

	tupla->pos_x = atoi(datos[0]);
	tupla->pos_y = atoi(datos[1]);
	tupla->cantidad = atoi(datos[2]);

	liberar_char_doble_puntero(datos);

	return tupla;
}

bool posicion_indicada(tupla_t* pos,uint32_t pos_x,uint32_t pos_y){

	return pos->pos_x == pos_x && pos->pos_y == pos_y;
}

void liberar_bloque(int bloque_a_liberar){
	bitarray_clean_bit(bitmap,bloque_a_liberar-1);
}

bool esta_bloque_vacio(char* path){
	int arch_bloque = open(path,O_RDWR);
	char* bloque_map = (char*)mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, arch_bloque, 0);

	bool flag = true;
	for(int i=0; i<BLOCK_SIZE; i++){
		flag = bloque_map[i] == '-' && flag;
	}

	munmap(bloque_map,BLOCK_SIZE);
	close(arch_bloque);

	return flag;
}

char* actualizar_bloques(char** bloques_actuales){
	int* dimensiones =obtener_dimensiones_array(bloques_actuales);
	int un_int = dimensiones[1]+dimensiones[0]+1;
	char* bloques_actualizados = malloc(un_int);
	memset(bloques_actualizados,'\0',un_int);
	char* path;
	int offset=0;
	int i=0;
	bool flag = true;
	strncpy(bloques_actualizados,"[",1);
	offset++;

	while(bloques_actuales!=NULL && bloques_actuales[i]!=NULL){
		path = obtener_path_bloque_especifico(bloques_actuales[i]);
		if(!esta_bloque_vacio(path)){
			if(!flag){
				strncpy(bloques_actualizados+offset,",",1);
				offset++;
			}

			strcpy(bloques_actualizados+offset, bloques_actuales[i]);
			offset+=strlen(bloques_actuales[i]);
			flag=false;
		}else{
			int bloque_a_liberar = atoi(bloques_actuales[i]);
			liberar_bloque(bloque_a_liberar);
		}
		i++;
		free(path);
	}

	strcpy(bloques_actualizados+offset,"]");
	free(dimensiones);
	return bloques_actualizados;
}

void alterar_posicion_mapeada(char** bloques, uint32_t pos_x, uint32_t pos_y, int valor, char* nombre_pokemon){

	int i=0;
	bool termine_de_operar=false;
	int offset_registro=0;
	char* pos_x_string = string_itoa(pos_x);
	char* pos_y_string = string_itoa(pos_y);
	int size=3+strlen(pos_x_string)+strlen(pos_y_string)+1;

	while(bloques[i]!=NULL && !termine_de_operar){
		int offset=0;

		char* path_bloque_especifico=obtener_path_bloque_especifico(bloques[i]);
		int file_descriptor_block = open(path_bloque_especifico, O_RDWR);
		void* block_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor_block, 0);

		char** todos_los_registros = string_split(block_mapeado,"\n");
		int registro_actual = 0;

		while(todos_los_registros[registro_actual] != NULL && !termine_de_operar){

			offset+=strlen(todos_los_registros[registro_actual]);
			offset++;

			if(strncmp(todos_los_registros[registro_actual],"-",1)){
				tupla_t* una_tupla;

				char** banana_split = string_split(todos_los_registros[registro_actual]," ");
				int* dimensiones_banana_split = obtener_dimensiones_array(banana_split);
				if(dimensiones_banana_split[0] == 3){
					//Toda la info esta en un mismo bloque
					una_tupla=deserializar_registro(todos_los_registros[registro_actual]);
				}else{
					//La info esta repartida en distintos bloques
					if(registro_actual != 0 && !string_ends_with(todos_los_registros[registro_actual],"-")){
						char* path_bloque_splitted=obtener_path_bloque_especifico(bloques[i+1]);
						int arch_bloque_splitted= open(path_bloque_splitted,O_RDWR);
						void*bloque_map_splitted=mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, arch_bloque_splitted, 0);
						char** registros_splitted = string_split(bloque_map_splitted,"\n");

						char* appender = string_new();

						string_append(&appender,todos_los_registros[registro_actual]);
						string_append(&appender,registros_splitted[0]);

						una_tupla=deserializar_registro(appender);
						free(appender);
						liberar_char_doble_puntero(registros_splitted);
						munmap(bloque_map_splitted,BLOCK_SIZE);
						close(arch_bloque_splitted);
						free(path_bloque_splitted);
					}
				}
				uint32_t nuevo_valor = una_tupla->cantidad+valor;

				if(posicion_indicada(una_tupla,pos_x,pos_y)){
					//Encontre la tupla
					char* cant_antigua_string = string_itoa(una_tupla->cantidad);
					int diferencia = offset - 1 - strlen(cant_antigua_string);


					char* metadata_pokemon = obtener_path_metadata_pokemon_especifico(nombre_pokemon);
					t_config* un_config = config_create(metadata_pokemon);
					char* nuevo_valor_string = string_itoa(nuevo_valor);

					if(nuevo_valor>0){
						int cantidad_antigua = strlen(cant_antigua_string);
						int cantidad_actual = strlen(nuevo_valor_string);

						if(cantidad_actual == cantidad_antigua){ //Caso bonito: no hay corrimiento de bytes
							//Estoy actualizando una cantidad
							if(dimensiones_banana_split[0] == 3){ // DIFERENCIA>0
								//Escribo un registro
								int tope = strlen(todos_los_registros[registro_actual]);
								apendear_registro_al_final_del_bloque(path_bloque_especifico,pos_x,pos_y,nuevo_valor,offset-tope-1); //AGREGAMOS UN +1 PERO DUDOUSI
							}else{
								//Escribo registro partido

								int contador = 0;
								int tope = strlen(nuevo_valor_string); //ACA HABIA UN +1 CUALQUIER RECLAMO -> CALL JORGE
								int centinela_particion = 0;

								while(contador+offset<BLOCK_SIZE){
									memcpy(block_mapeado+offset-tope+contador, nuevo_valor_string+centinela_particion, 1);
									contador++;
									centinela_particion++;
								}

								contador = 0;
								char* path_segundo_bloque;
								int fd_segundo_bloque;
								void* segundo_bloque_mapeado;
								char** registro_segundo_bloque;

								if(bloques[i+1]!=NULL){

									path_segundo_bloque=obtener_path_bloque_especifico(bloques[i+1]);
									fd_segundo_bloque = open(path_segundo_bloque, O_RDWR);
									segundo_bloque_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_segundo_bloque, 0);

									registro_segundo_bloque = string_split(segundo_bloque_mapeado,"\n");

								}else{

									int new_block = obtener_nuevo_bloque();
									char* bloque_a_agregar = string_itoa(new_block);
									int* un_int_puntero = obtener_dimensiones_array(bloques);
									int longitud_total = un_int_puntero[0];
									int peso_array = un_int_puntero[1];

									agregar_bloque_a_metadata(bloques, longitud_total, bloque_a_agregar, un_config, peso_array);

									path_segundo_bloque=obtener_path_bloque_especifico(bloque_a_agregar);
									fd_segundo_bloque = open(path_segundo_bloque, O_RDWR);
									segundo_bloque_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_segundo_bloque, 0);
									registro_segundo_bloque = string_split(segundo_bloque_mapeado,"\n");

									free(bloque_a_agregar);
									//Agrego free
									free(un_int_puntero);
								}

								int segundo_tope = strlen(registro_segundo_bloque[0]);
								int inicio = segundo_tope - strlen(cant_antigua_string);
								while(contador+inicio<segundo_tope){ //while(contador<segundo_tope)
									memcpy(segundo_bloque_mapeado+inicio+contador, nuevo_valor_string+centinela_particion, 1);
									contador++;
									centinela_particion++;
								}

								/*int antiguo_size = config_get_int_value(un_config,"SIZE");
								int nuevo_size = antiguo_size - tope - segundo_tope;
								char* peso_string = string_itoa(nuevo_size);

								char** bloques_actuales = config_get_array_value(un_config, "BLOCKS");
								char* bloques_actualizados = actualizar_bloques(bloques_actuales);
								config_set_value(un_config,"BLOCKS",bloques_actualizados);
								free(bloques_actualizados);

								config_set_value(un_config,"SIZE",peso_string);
								config_save(un_config);
								free(peso_string);*/

								//Agrego free
								close(fd_segundo_bloque);
								munmap(segundo_bloque_mapeado,BLOCK_SIZE);
								free(path_segundo_bloque);
								liberar_char_doble_puntero(registro_segundo_bloque);

							}
						}else{

							int* dimensiones = obtener_dimensiones_array(bloques);
							int bloques_a_mover = dimensiones[0]-i;

							//Le damos la papita al mono(es decir, agarra toodo lo que hay atrás)
							char* el_papota_del_curso = malloc(BLOCK_SIZE-offset+bloques_a_mover*BLOCK_SIZE);

							memcpy(el_papota_del_curso,block_mapeado+offset,BLOCK_SIZE-offset);
							for(int j=1; j<bloques_a_mover; j++){
								char* path_restantes=obtener_path_bloque_especifico(bloques[i+j]);
								int fd_restantes = open(path_restantes, O_RDWR);
								void* bloque_mapeado_restantes = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_restantes, 0);

								memcpy(el_papota_del_curso+offset+BLOCK_SIZE*(j-1),bloque_mapeado_restantes,BLOCK_SIZE);

								munmap(bloque_mapeado_restantes,BLOCK_SIZE);
								close(fd_restantes);
								free(path_restantes);
							}

							//Hay que mover toodo dependiendo de los bytes que nos agreguen
							if(cantidad_actual>cantidad_antigua){
								//Meto mas info
								int ubicacion_del_registro= strlen(todos_los_registros[registro_actual]);
								apendear_registro_al_final_del_bloque(path_bloque_especifico,pos_x,pos_y,nuevo_valor,offset-ubicacion_del_registro);
							}

							//Muevo toodo para la izquierda o para la derecha
							int desplazamiento = cantidad_antigua - cantidad_actual; //TODO ESTO ROMPE SI EL ULTIMO BLOQUE ESTA LLENO!!
							memcpy(block_mapeado+offset-desplazamiento,el_papota_del_curso,BLOCK_SIZE-offset);

							for(int j=1; j<bloques_a_mover; j++){
								char* path_restantes=obtener_path_bloque_especifico(bloques[i+j]);
								int fd_restantes = open(path_restantes, O_RDWR);
								void* bloque_mapeado_restantes = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_restantes, 0);

								memcpy(bloque_mapeado_restantes,el_papota_del_curso+BLOCK_SIZE-offset+(j-1)*BLOCK_SIZE,BLOCK_SIZE);

								munmap(bloque_mapeado_restantes,BLOCK_SIZE);
								close(fd_restantes);
								free(path_restantes);

							}

							int antiguo_size = config_get_int_value(un_config,"SIZE");
							int nuevo_size = antiguo_size - desplazamiento;
							char* peso_string = string_itoa(nuevo_size);

							config_set_value(un_config,"SIZE",peso_string);
							config_save(un_config);

							//Agrego free
							free(dimensiones);
						}

					}else{
						//Estoy borrando una tupla
						char** banana_split = string_split(todos_los_registros[registro_actual]," ");
						int* dimensiones_banana_split = obtener_dimensiones_array(banana_split);

						if(dimensiones_banana_split[0] == 3){
							//Borro todo el registro que se encuentra en esta posicion
							int contador = 0;
							int tope = strlen(todos_los_registros[registro_actual]);

							while(contador<tope){
								memcpy(block_mapeado+offset-tope+contador-1, "-", 1);
								contador++;
							}

							int antiguo_size = config_get_int_value(un_config,"SIZE");
							int nuevo_size = antiguo_size - tope - 1;
							char* peso_string = string_itoa(nuevo_size);

							if(nuevo_size%BLOCK_SIZE == 0){
								//Borre justo el registro
								/*char** bloques_actuales = config_get_array_value(un_config, "BLOCKS");
								char* bloques_actualizados = actualizar_bloques(bloques_actuales);

								config_set_value(un_config,"BLOCKS",bloques_actualizados);

								free(bloques_actualizados);*/
							}

							config_set_value(un_config,"SIZE",peso_string);
							config_save(un_config);
							free(peso_string);

						}else{
							//Borro el registro esta partido
							if(registro_actual != 0){
								int contador = 0;
								int tope = strlen(todos_los_registros[registro_actual])+1;

								while(contador+offset-tope<BLOCK_SIZE){
									memcpy(block_mapeado+offset-tope+contador, "-", 1);
									contador++;
								}
								contador = 0;

								char* path_segundo_bloque=obtener_path_bloque_especifico(bloques[i+1]);
								int fd_segundo_bloque = open(path_segundo_bloque, O_RDWR);
								void* segundo_bloque_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_segundo_bloque, 0);

								char** registro_segundo_bloque = string_split(segundo_bloque_mapeado,"\n");
								int segundo_tope = strlen(registro_segundo_bloque[0]); //El +1 para borrar el \n

								while(contador<segundo_tope){
									memcpy(segundo_bloque_mapeado+contador, "-", 1);
									contador++;
								}

								int antiguo_size = config_get_int_value(un_config,"SIZE");
								int nuevo_size = antiguo_size - tope - segundo_tope;
								char* peso_string = string_itoa(nuevo_size);

								/*char** bloques_actuales = config_get_array_value(un_config, "BLOCKS");
								char* bloques_actualizados = actualizar_bloques(bloques_actuales);
								config_set_value(un_config,"BLOCKS",bloques_actualizados);
								free(bloques_actualizados);*/

								config_set_value(un_config,"SIZE",peso_string);
								config_save(un_config);
								free(peso_string);
								//liberar_char_doble_puntero(bloques_actuales);
								liberar_char_doble_puntero(registro_segundo_bloque);

								//Agrego free
								close(fd_segundo_bloque);
								munmap(segundo_bloque_mapeado,BLOCK_SIZE);
								free(path_segundo_bloque);
							}
						}
						int inicio_del_corrimiento = offset - strlen(todos_los_registros[offset_registro]);
						corrimiento_de_bloques(nombre_pokemon);
						liberar_char_doble_puntero(banana_split);
						free(dimensiones_banana_split);
					}

					free(cant_antigua_string);
					config_destroy(un_config);
					free(nuevo_valor_string);
					free(metadata_pokemon);

					termine_de_operar = true;
				}
				liberar_char_doble_puntero(banana_split);
				free(dimensiones_banana_split);
				//Agrego free
				free(una_tupla);
			}
			registro_actual++;
		}

		liberar_char_doble_puntero(todos_los_registros);
		munmap(block_mapeado, BLOCK_SIZE);
		close(file_descriptor_block);
		free(path_bloque_especifico);
		i++;
	}

	free(pos_x_string);
	free(pos_y_string);
}

void corrimiento_de_bloques(char* nombre_pokemon){

	char* path = obtener_path_metadata_pokemon_especifico(nombre_pokemon);
	t_config* un_config = config_create(path);
	char** bloques = config_get_array_value(un_config,"BLOCKS");
	int x = 0;
	char* papota_del_curso = string_new();

	while(bloques[x] != NULL){

		char* path_primer_bloque = obtener_path_bloque_especifico(bloques[x]);
		int fd_primer_bloque = open(path_primer_bloque, O_RDWR);
		void* primer_bloque_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_primer_bloque, 0);

		char** registros = string_split(primer_bloque_mapeado,"\n");
		int nro_de_registro = 0;

		while(registros[nro_de_registro] != NULL){

			char** banana_split = string_split(registros[nro_de_registro]," ");
			int* dimensiones_banana_split = obtener_dimensiones_array(banana_split);

			if(dimensiones_banana_split[0] == 3){
				if(!string_starts_with(registros[nro_de_registro],"-")){
					string_append(&papota_del_curso,registros[nro_de_registro]);
					string_append(&papota_del_curso,"\n");
				}

			}else{
				//Esta partido
				if(nro_de_registro!=0){
					if(!string_starts_with(registros[nro_de_registro],"-")){

						char* path_segundo_bloque = obtener_path_bloque_especifico(bloques[x+1]);
						int fd_segundo_bloque = open(path_segundo_bloque, O_RDWR);
						void* segundo_bloque_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_segundo_bloque, 0);

						char** registro_spliteado = string_split(segundo_bloque_mapeado,"\n");

						string_append(&papota_del_curso,registros[nro_de_registro]);

						string_append(&papota_del_curso,registro_spliteado[0]);

						string_append(&papota_del_curso,"\n");

						munmap(segundo_bloque_mapeado,BLOCK_SIZE);
						close(fd_segundo_bloque);
						free(path_segundo_bloque);
						liberar_char_doble_puntero(registro_spliteado);
					}
				}
			}
			liberar_char_doble_puntero(banana_split);
			free(dimensiones_banana_split);

			nro_de_registro++;
		}
		munmap(primer_bloque_mapeado,BLOCK_SIZE);
		close(fd_primer_bloque);
		free(path_primer_bloque);
		///
		liberar_char_doble_puntero(registros);

		x++;
	}

	int nuevo_peso = config_get_int_value(un_config,"SIZE");
	if(nuevo_peso<(x-1)*BLOCK_SIZE){

		char* path_bloque = obtener_path_bloque_especifico(bloques[x-1]);
		int fd_bloque = open(path_bloque, O_RDWR);
		void* bloque_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_bloque, 0);
		int aux = 0;

		while(aux<BLOCK_SIZE){
			memcpy(bloque_mapeado+aux,"-",1);
			aux++;
		}

		//
		munmap(bloque_mapeado,BLOCK_SIZE);
		close(fd_bloque);
		free(path_bloque);
	}

	int segundo_aux = 0;
	int offset_del_papota = 0;

	while(bloques[segundo_aux] != NULL && offset_del_papota<nuevo_peso){

		char* path_bloque3 = obtener_path_bloque_especifico(bloques[segundo_aux]);
		int fd_bloque3 = open(path_bloque3, O_RDWR);
		void* bloque_mapea3 = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_bloque3, 0);
		int tercer_aux = 0;

		while(tercer_aux<BLOCK_SIZE && offset_del_papota<nuevo_peso){

			memcpy(bloque_mapea3+tercer_aux,papota_del_curso + offset_del_papota,1);
			tercer_aux++;
			offset_del_papota++;

		}
		munmap(bloque_mapea3,BLOCK_SIZE);
		close(fd_bloque3);
		free(path_bloque3);

		segundo_aux++;
	}

	if(segundo_aux == 0){
		segundo_aux++;
		//Borre todos los pokemons de un bloque
	}
	char* path_bloque_guion= obtener_path_bloque_especifico(bloques[segundo_aux-1]);
	int fd_bloque_guion = open(path_bloque_guion, O_RDWR);
	void* bloque_mapeado_guion = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, fd_bloque_guion, 0);


	int inicio = ((strlen(papota_del_curso))%BLOCK_SIZE);
	int cant_de_guiones = BLOCK_SIZE - inicio;
	int guiones_escritos = 0;

	while(guiones_escritos < cant_de_guiones){
		memcpy(bloque_mapeado_guion+inicio+guiones_escritos,"-",1);
		guiones_escritos++;
	}

	char* bloques_actualizados = actualizar_bloques(bloques);
	config_set_value(un_config,"BLOCKS",bloques_actualizados);
	config_save(un_config);
	free(bloques_actualizados);

	config_destroy(un_config);
	free(path);
	liberar_char_doble_puntero(bloques);

	///
	munmap(bloque_mapeado_guion,BLOCK_SIZE);
	close(fd_bloque_guion);
	free(path_bloque_guion);
	free(papota_del_curso);
}

void cerrar_abrir_archivo(char*nombre_pokemon, int yes_no_flag ){

	char*path_archivo=obtener_path_metadata(nombre_pokemon);
	t_config* config_metadata= config_create(path_archivo);

	if(yes_no_flag == YES){
		config_set_value(config_metadata,"OPEN","Y");
	}else{
		config_set_value(config_metadata,"OPEN","N");
	}
	config_save(config_metadata);
	config_destroy(config_metadata);
	free(path_archivo);
}

void abrir_archivo(char*nombre_pokemon){
	cerrar_abrir_archivo(nombre_pokemon, YES );

}
void cerrar_archivo(char*nombre_pokemon){
	cerrar_abrir_archivo(nombre_pokemon, NO );
}

void decrementar_cantidad_pokemon(uint32_t pos_x,uint32_t pos_y,char*nombre_pokemon){
	aumentar_valor_en_posicion(-1,pos_x,pos_y,nombre_pokemon);
}

void crear_pokemon(char* nombre_pokemon){

	char* path_new_pokemon= malloc(strlen(nombre_pokemon)+ strlen(path_pokemon)+8+strlen("/Metadata.bin")); //ACA VA UN 4
	strcpy(path_new_pokemon,path_pokemon);
	path_new_pokemon=strcat(path_new_pokemon,"/");
	path_new_pokemon=strcat(path_new_pokemon,nombre_pokemon);
	mkdir(path_new_pokemon,S_IRWXU);
	path_new_pokemon=strcat(path_new_pokemon,"/Metadata.bin");

	FILE* archivo= fopen(path_new_pokemon,"wb");
	int peso = strlen("DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=N")+1;
	char* giorgios_idea = malloc(peso);
	strcpy(giorgios_idea,"DIRECTORY=N\nSIZE=0\nBLOCKS=[]\nOPEN=N");
	fwrite(giorgios_idea,peso,1,archivo);
	free(giorgios_idea);
	fclose(archivo);

	free(path_new_pokemon);
}

void agregar_bloque_a_metadata(char** vector_viejo,int longitud_total,char* bloque_a_agregar,t_config*metadata,int peso_array){

	int peso_vector = peso_array+strlen(bloque_a_agregar)+3+longitud_total;
	char* vector=malloc(peso_vector);
	memset(vector, '\0', peso_vector);

	if(peso_array==0){
		vector = strcat(vector,"[");
		vector = strcat(vector,bloque_a_agregar);
		vector = strcat(vector,"]");
	}else{
		vector = strcat(vector,"[");
		vector = strcat(vector,vector_viejo[0]);

		for(int x=1; x<longitud_total;x++){
			vector = strcat(vector,",");
			vector = strcat(vector,vector_viejo[x]);
		}
		vector = strcat(vector,",");
		vector = strcat(vector,bloque_a_agregar);
		vector = strcat(vector,"]");
	}
	config_set_value(metadata,"BLOCKS",vector);
	config_save(metadata);
	free(vector);
}

int* obtener_dimensiones_array(char**blocks){
	int* dimensiones=malloc(sizeof(int)*2);
	int longitud_total=0;
	int peso_array=0;
	while(blocks!=NULL && blocks[longitud_total]!=NULL){
		peso_array+= strlen(blocks[longitud_total]);
		peso_array++;
		longitud_total++;
	}
	dimensiones[0]=longitud_total;
	dimensiones[1]=peso_array;
	return dimensiones;
}

char* registro(uint32_t pos_x,uint32_t pos_y,uint32_t cantidad){
	int offset=0;

	char* pos_x_string = string_itoa(pos_x);
	char* pos_y_string = string_itoa(pos_y);
	char* cantidad_string = string_itoa(cantidad);

	char* registro = string_new();

	string_append(&registro,pos_x_string);
	string_append(&registro," ");
	string_append(&registro,pos_y_string);
	string_append(&registro," ");
	string_append(&registro,cantidad_string);
	string_append(&registro,"\n");

	free(pos_x_string);
	free(pos_y_string);
	free(cantidad_string);

	return registro;
}

int agregar_posicion(uint32_t pos_x,uint32_t pos_y,uint32_t cantidad,char*nombre_pokemon){

	char* path_metadata_especifica=obtener_path_metadata_pokemon_especifico(nombre_pokemon);
	t_config* un_config=config_create(path_metadata_especifica);
	char** blocks=config_get_array_value(un_config,"BLOCKS");
	int size_config=config_get_int_value(un_config,"SIZE");

	//int peso_registro = 3+sizeof(uint32_t)*3;

	int peso_array=0;
	int longitud_total=0;
	int* dimensiones_blocks=obtener_dimensiones_array(blocks);
	longitud_total=dimensiones_blocks[0];
	peso_array=dimensiones_blocks[1];
	char* path_bloque;

	char* pos_x_string = string_itoa(pos_x);
	char* pos_y_string = string_itoa(pos_y);
	char* cantidad_string = string_itoa(cantidad);
	int size=3+strlen(pos_x_string)+strlen(pos_y_string)+strlen(cantidad_string);

	if(size_config%BLOCK_SIZE==0 || longitud_total==0){//Necesito pedir un nuevo bloque
		int nuevo_bloque= obtener_nuevo_bloque();
		if(nuevo_bloque<0){
			log_info(log_obligatorio,"No hay mas bloques en fs");
			return NO_HAY_BLOQUES;
		}
		char* bloque_string= string_itoa(nuevo_bloque);
		path_bloque=obtener_path_bloque_especifico(bloque_string);
		apendear_registro_al_final_del_bloque(path_bloque,pos_x,pos_y,cantidad,0);
		agregar_bloque_a_metadata(blocks,longitud_total,bloque_string,un_config,peso_array);
		free(bloque_string);
		free(path_bloque);
	}else{
		//Puedo escribir adentro del bloque
		if(BLOCK_SIZE>((size_config%BLOCK_SIZE)+size)){
			//Escribo en el espacio que queda del bloque
			path_bloque = obtener_path_bloque_especifico(blocks[longitud_total-1]);
			apendear_registro_al_final_del_bloque(path_bloque,pos_x,pos_y,cantidad,size_config%BLOCK_SIZE); //TODO Hay que arreglar esta funcion
			free(path_bloque);
		}else{
			//Escritura partida
			int bytes_a_escribir = BLOCK_SIZE - (size_config%BLOCK_SIZE);
			path_bloque = obtener_path_bloque_especifico(blocks[longitud_total-1]);

			int file_descriptor_block = open(path_bloque, O_RDWR);
			void* block_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor_block, 0);

			int offset = 0;
			char* escritor = string_new();

			/*escritor = strcat(pos_x_string," ");
			escritor = strcat(escritor,pos_y_string);
			escritor = strcat(escritor," ");
			escritor = strcat(escritor,cantidad_string);
			escritor = strcat(escritor,"\n");*/

			string_append(&escritor,pos_x_string);
			string_append(&escritor," ");
			string_append(&escritor,pos_y_string);
			string_append(&escritor," ");
			string_append(&escritor,cantidad_string);
			string_append(&escritor,"\n");

			int verificador = strlen(escritor);

			memcpy(block_mapeado+(size_config%BLOCK_SIZE),escritor,bytes_a_escribir);

			munmap(block_mapeado,BLOCK_SIZE);
			close(file_descriptor_block);
			free(path_bloque);

			if(verificador > bytes_a_escribir){
				//Abrimos otro bloque
				int nuevo_bloque= obtener_nuevo_bloque();
				if(nuevo_bloque<0){
					log_info(log_obligatorio,"No hay mas bloques en fs");
					return NO_HAY_BLOQUES;
				}
				char* bloque_string= string_itoa(nuevo_bloque);
				path_bloque=obtener_path_bloque_especifico(bloque_string);

				int file_descriptor_proximo_bloque = open(path_bloque, O_RDWR);
				void* proximo_bloque = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor_proximo_bloque, 0);

				int bytes_a_copiar = verificador - bytes_a_escribir;
				memcpy(proximo_bloque,(void*)escritor+bytes_a_escribir,bytes_a_copiar);

				agregar_bloque_a_metadata(blocks,longitud_total,bloque_string,un_config,peso_array);

				munmap(proximo_bloque,BLOCK_SIZE);
				close(file_descriptor_proximo_bloque);
				free(bloque_string);
				free(path_bloque);
			}
			free(escritor);

		}
	}
	free(dimensiones_blocks);
	free(path_metadata_especifica);
	int j=0;
	while(blocks[j]!=NULL){
		free(blocks[j]);
		j++;
	}
	free(blocks);
	char* size_string=string_itoa(size+size_config);
	config_set_value(un_config,"SIZE",size_string);
	config_save(un_config); //Lo agrego aca para que se guarde en la compu
	config_destroy(un_config);
	free(size_string);
	free(pos_x_string);
	free(pos_y_string);
	free(cantidad_string);

	return OPERACION_EXITOSA;
}


void apendear_registro_al_final_del_bloque(char* path_bloque,uint32_t pos_x,uint32_t pos_y,uint32_t nueva_cantidad,int offset){

	int file_bloque= open(path_bloque,O_RDWR);
	void* bloque_mapeado = mmap(NULL,BLOCK_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, file_bloque, 0);
	uint32_t cantidad=nueva_cantidad;
	char* registro_pegado= registro(pos_x,pos_y,cantidad);
	int size_registro=strlen(registro_pegado);
	memcpy(bloque_mapeado+offset,registro_pegado,size_registro);
	munmap(bloque_mapeado,BLOCK_SIZE);
	free(registro_pegado);
	close(file_bloque);

}

void aumentar_valor_en_posicion(int valor,uint32_t pos_x ,uint32_t pos_y,char*nombre_pokemon){

	char*path_bloque_especifico=obtener_path_metadata(nombre_pokemon);
	t_config* config_metadata= config_create(path_bloque_especifico);
	char** bloques=config_get_array_value(config_metadata,"BLOCKS");
	alterar_posicion_mapeada(bloques, pos_x, pos_y, valor, nombre_pokemon);
	config_destroy(config_metadata);
	free(path_bloque_especifico);
	int j=0;
	while(bloques[j]!=NULL){
		free(bloques[j]);
		j++;
	}
	free(bloques);
}


int obtener_nuevo_bloque(){
	int i=0;
	int bloque_totales=bitarray_get_max_bit(bitmap);
	while(i< bloque_totales){
		if(!bitarray_test_bit(bitmap,i)){
			bitarray_set_bit(bitmap,i);
			return i+1;
		}
		i++;
	}
	return -1;
}

void intentar_abrir_archivo(char* nombre_pokemon){
	bool no_se_puede_abrir_archivo=esta_abierto_archivo(nombre_pokemon);
	while(no_se_puede_abrir_archivo){
		log_info(log_obligatorio,"No se puede abrir el archivo de %s", nombre_pokemon);
		sleep(TIEMPO_DE_REINTENTO_OPERACION);
		no_se_puede_abrir_archivo=esta_abierto_archivo(nombre_pokemon);
	}
}

uint32_t obtener_cantidad_coordenadas(t_list* lista_de_posiciones){

	uint32_t cantidad_total_coordenadas = 0;

	for(int i=0; i<list_size(lista_de_posiciones); i++){
		tupla_t* una_tupla = list_get(lista_de_posiciones,i);
		cantidad_total_coordenadas += una_tupla->cantidad;
	}

	return cantidad_total_coordenadas;
}

void liberar_char_doble_puntero(char**char_doble_puntero){
	int i=0;
	while(char_doble_puntero[i]!=NULL){
		free(char_doble_puntero[i]);
		i++;
	}
	free(char_doble_puntero);

}

int obtener_cant_de_coordenadas(t_list* lista_de_posiciones){

	int contador = 0;

	void contar_tuplas(tupla_t* una_tupla){
		contador+=una_tupla->cantidad;
	}
	list_iterate(lista_de_posiciones,contar_tuplas);

	return contador;
}

bool broker_esta_dormido(){

	bool retorno = false;

	for(int i=0; i<3; i++){
		retorno = retorno || suscripcion[i]<3;
	}

	return retorno;
}
