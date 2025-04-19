#include "ej1.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

/**
*	crea y destruye a una lista vacía
*/
void test_create_destroy_list(){
	string_proc_list * list	= string_proc_list_create_asm();
	printf(list->first == NULL ? "Lista creada correctamente\n" : "Error al crear la lista\n");
	printf(list->last == NULL ? "Lista creada correctamente\n" : "Error al crear la lista\n");
	string_proc_list_destroy(list);
}

/**
*	crea y destruye un nodo
*/
void test_create_destroy_node(){
	printf("Creando nodo vacío\n");

	string_proc_node* node = string_proc_node_create_asm(0, "hash");

	// Mostrar contenido del nodo
	printf("Hash: %s\n", node->hash);
	printf("Type: %d\n", node->type);

	// Verificar punteros
	printf(node->next == NULL ? "Next NULL correctamente\n" : "Error: next no es NULL\n");
	printf(node->previous == NULL ? "Previous NULL correctamente\n" : "Error: previous no es NULL\n");

	printf("Destruyendo nodo\n");
	string_proc_node_destroy(node);
}

/**
 * 	crea una lista y le agrega nodos
*/
void test_create_list_add_nodes()
{	
	string_proc_list * list = string_proc_list_create_asm();

	string_proc_list_add_node_asm(list, 0, "hola");
	printf(strcmp(list->first->hash, "hola") == 0 ? "Nodo creado correctamente\n" : "Error al crear el nodo\n");

	string_proc_list_add_node_asm(list, 0, "a");
	printf(strcmp(list->first->next->hash, "a") == 0 ? "Nodo creado correctamente\n" : "Error al crear el nodo\n");

	string_proc_list_add_node_asm(list, 0, "todos!");
	printf(strcmp(list->first->next->next->hash, "todos!") == 0 ? "Nodo creado correctamente\n" : "Error al crear el nodo\n");

	printf(strcmp(list->first->next->next->previous->hash, "a") == 0 ? "Nodo creado correctamente\n" : "Error al crear el nodo\n");

	string_proc_list_destroy(list);
}


/**
 * 	crea una lista y le agrega nodos. Luego aplica la lista a un hash.
*/

void test_list_concat()
{
	string_proc_list * list	= string_proc_list_create_asm();
	string_proc_list_add_node_asm(list, 0, "hola");
	string_proc_list_add_node_asm(list, 0, "a");
	string_proc_list_add_node_asm(list, 0, "todos!");
	string_proc_list_add_node_asm(list, 1, "hola");
	string_proc_list_add_node_asm(list, 1, "a");
	string_proc_list_add_node_asm(list, 1, "todos!");
	string_proc_list_add_node_asm(list, 2, "hola");
	string_proc_list_add_node_asm(list, 2, "a");
	string_proc_list_add_node_asm(list, 2, "todos!");
	string_proc_list_add_node_asm(list, 3, "hola");
	string_proc_list_add_node_asm(list, 3, "a");
	string_proc_list_add_node_asm(list, 3, "todos!");

	string_proc_list_concat_asm(list, 0, "junta-constelaciones-mision-estrellas:");
}

/**
* Corre los test a se escritos por los alumnos	
*/
void run_tests(){

	/* Aqui pueden comenzar a probar su codigo */
	test_create_destroy_list();

	test_create_destroy_node();

	test_create_list_add_nodes();

	test_list_concat();
}

int main (void){
	run_tests();
	return 0;    
}

