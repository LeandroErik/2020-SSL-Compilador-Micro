#ifndef ASDR
#define ASDR
/*Analisis sintactico desendente recursivo ,este permite ser programado por nosotros*/
/**********INCLUYO BIBLIOTECAS************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/**********Inicio Declaraciones globales***********/


#define TAMLEX 33 //declaracion de variable global para ser usado como tamanio maximo de los lexemas, en este caso seran 32 caracteres + '/0' que indica fin de palabra
#define NUMESTADOS 15 /*TAMA�O DE LA TABLA DE TRANSICIONES*/
#define NUMCOLS 13
#define TAMNOM 20+1 


/**********Fin Declaraciones globales***********/

/************Inicio declaraciones de variables************/
//declaracion de archivos

FILE * archivoSalida;
FILE * archivo; 




//Nombre de los tokens que pasaran por el arbol de analisis sintactico, es decir futuras hojas del arbol
typedef enum {
	INICIO, FIN, LEER, ESCRIBIR, ID, CONSTANTE, PARENIZQUIERDO,
	PARENDERECHO, PUNTOYCOMA, COMA, ASIGNACION, SUMA, RESTA, FDT, ERRORLEXICO
} TOKEN; 


//Registro de la tabla de simbolos
typedef struct{
	char identificador[TAMLEX]; //ID=identificador
	TOKEN t; /* t=0, 1, 2, 3 Palabra Reservada, t=ID=4 Identificador */
} RegTS;



char buffer[TAMLEX]; //para usar como auxiliar
 
//Estructura llamada REG_EXPRESION con tres campos,tipo token,tipo char y tipo int.Este es el registro para las expresiones
typedef struct{
	TOKEN clase;
	char nombre[TAMLEX];
	int valor;
} REG_EXPRESION; 


TOKEN tokenActual;
int flagToken;

/************Fin declaraciones de variables************/


/*************** Inicio Declaracion de Funciones o Rutinas ***************/

//PAS son funciones void, debido a que en ansi C son las usadas para los procedimientos de analisis sintactico (PAS)

void Objetivo(void);
void Programa(void);
void ListaSentencias(void);
void Sentencia(void);
void ListaIdentificadores(void);
void Identificador(REG_EXPRESION * presul);
void ListaExpresiones(void);
void Expresion(REG_EXPRESION * presul);
void Primaria(REG_EXPRESION * presul);
void OperadorAditivo(char * presul);


//Rutinas semanticas
REG_EXPRESION ProcesarCte(void);
REG_EXPRESION ProcesarId(void);
char * ProcesarOp(void);
void Leer(REG_EXPRESION in);
void Escribir(REG_EXPRESION out);
REG_EXPRESION GenInfijo(REG_EXPRESION e1, char * op, REG_EXPRESION e2);

//Funciones auxiliares
void Match(TOKEN token);
TOKEN ProximoToken(void);
void ErrorLexico();
void ErrorSintactico();
void Generar(char * co, char * a, char * b, char * c);
TOKEN Scanner(void);
char * Extraer(REG_EXPRESION * reg);
int Buscar(char * id, RegTS * TS, TOKEN * t);
void Colocar(char * id, RegTS * TS);
void Chequear(char * s);
void Comenzar(void);
void Terminar(void);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);


//Scanner

int columna(int c);
void AgregarCaracter(int caracter, int posicion);
void LimpiarBuffer(void);
int validacion(char* arch, char c);
int esEstadoFinal(int e);


#endif

