/*
 * dump_cache.c
 *
 *  Created on: 22 abr. 2020
 *      Author: utnso
 */

#include <signal.h>
#include <commons/temporal.h>
#include <commons/string.h>
#include <time.h>
#include <assert.h>
#include <inttypes.h>
#include "funciones_compartidas.h"

#include "tads_y_globales.h"

char* dos_cifras_maker(int num) {
	char* num_a_retornar = (char*)malloc(2);
	memset(num_a_retornar, '\0', 2);
	char* num_string = string_itoa(num);

	if(num < 10) {
		string_append(&num_a_retornar, "0");
		string_append(&num_a_retornar, num_string);
	} else {
		string_append(&num_a_retornar, num_string);
	}

	free(num_string);

	return num_a_retornar;
}

char* fecha_actual(){
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	char* dia = dos_cifras_maker(tm->tm_mday);
	char* mes = dos_cifras_maker(tm->tm_mon+ 1);
	char* anio = string_itoa(tm->tm_year + 1900);
	char* hora = dos_cifras_maker(tm->tm_hour);
	char* mins = dos_cifras_maker(tm->tm_min);
	char* segs = dos_cifras_maker(tm->tm_sec);

	char* fecha = (char*)malloc(strlen("14/07/2012 10:11:12")); //fecha random para tener el espacio nomas
	memset(fecha, '\0', strlen("14/07/2012 10:11:12"));
	string_append(&fecha, dia);
	string_append(&fecha, "/");
	string_append(&fecha, mes);
	string_append(&fecha, "/");
	string_append(&fecha, anio);
	string_append(&fecha, " ");
	string_append(&fecha, hora);
	string_append(&fecha, ":");
	string_append(&fecha, mins);
	string_append(&fecha, ":");
	string_append(&fecha, segs);

	free(dia);
	free(mes);
	free(anio);
	free(hora);
	free(mins);
	free(segs);

	//assert(strftime(fecha, sizeof(fecha), "%c", tm));

	return fecha;
}

void dump_cache(){
	FILE* archivo_dump;
	char* fecha_y_hora_dump = fecha_actual();

	archivo_dump = fopen("archivo_dump.txt", "a");

	fprintf(archivo_dump, "-------------------------------------------------------------------------------------------------------------------------------------------------\n");
	fprintf(archivo_dump, "Dump: %s\n", fecha_y_hora_dump);
	free(fecha_y_hora_dump);

	//Ojo si hay que poner semaforos a la lista, la funcion del offset ya tiene semaforos adentro
	for(int i = 0; i < list_size(lista_de_particiones); i++){
		metadata_t* metadata = (metadata_t*)list_get(lista_de_particiones, i);

		void* posicion_inicial = cache_broker + obtener_offset_en_cache(i);
		void* posicion_final = posicion_inicial + metadata->tamanio_ocupado;

		if(metadata->libre){
			fprintf(archivo_dump, "Particion %d: %p - %p     [L]     Tam: %" PRIu32 "b\n", i, posicion_inicial, posicion_final, metadata->tamanio_ocupado);
		}
		else{
			fprintf(archivo_dump, "Particion %d: %p - %p     [X]     Tam: %" PRIu32 "b     LRU: %" PRIu32 "     Cola: %d     Id: %" PRIu32 "\n", i, posicion_inicial, posicion_final, metadata->tamanio_ocupado, metadata->timestamp_LRU, metadata->codigo_de_cola, metadata->id_mensaje_contenido);
		}
	}

	fclose(archivo_dump);
}

void controlador_de_seniales(int num_senial){
	if(num_senial == SIGUSR1){
		dump_cache();
		loggear_mensaje("Particiones dumpeadas", 0);
	}
}//La signal se la tiro asi desde donde corresponda: signal(SIGUSR1, controlador_de_seniales);
