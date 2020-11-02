/*Analisis sintactico desendente recursivo ,este permite ser programado por nosotros*/
/////////////////////////INCLUYO BIBLIOTECAS////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUMESTADOS 15 /*TAMAÑO DE LA TABLA DE TRANSICIONES*/
#define NUMCOLS 13

#define TAMLEX 32+1 /*Defino el numero maximo de cracteres,en este caso 32 caracteres*/
#define TAMNOM 20+1


///////Defino Variables

typedef enum {
	INICIO, FIN, LEER, ESCRIBIR, ID, CONSTANTE, PARENIZQUIERDO,
	PARENDERECHO, PUNTOYCOMA, COMA, ASIGNACION, SUMA, RESTA, FDT, ERRORLEXICO
} TOKEN; //nombre de los tokens que pasaran por el arbol de analisis sintactico, es decir futuras hojas del arbol

/*REG_OPERACION, que solo contendrá el valor del token SUMA o RESTA.
2. REG_EXPRESION, que contendrá el tipo de expresión y el valor que le corresponde; este
valor puede ser una cadena (para el caso de un identificador) o un número entero (para el
caso de una constante). La cadena será almacenada en un vector de caracteres previamente
definido y tendrá una longitud máxima de 32 caracteres, límite que se informa en la sección
3.2.1 con respecto a los identificadores.*/
typedef struct{
	TOKEN clase;
	char nombre[TAMLEX];
	int valor;
} REG_EXPRESION; //declaracion de una estructura llamada REG_EXPRESION con tres campos,tipo token,tipo char y tipo int

FILE * archivo; //declaracion de archivos
FILE * archivoSalida;


char buffer[TAMLEX]; //para usar como auxiliar

TOKEN tokenActual;
int flagToken;

typedef struct{
	char identificador[TAMLEX]; //contiene el identificador
	TOKEN t; /* t=0, 1, 2, 3 Palabra Reservada, t=ID=4 Identificador */
} RegTS; //se alacena los tokens y identificadores que se fueron leyendo a lo largo del mprograma


//////////////Declaracion de Funciones///////////////////

void Match(TOKEN token);
void Objetivo(void);
void Programa(void);
void ListaSentencias(void);
void Sentencia(void);
void Expresion(REG_EXPRESION * presul);
void Primaria(REG_EXPRESION * presul);
void OperadorAditivo(char * presul);
void ErrorSintactico();
void ErrorLexico();
TOKEN ProximoToken(void);
void Identificador(REG_EXPRESION * presul);
void ListaExpresiones(void);
void ListaIdentificadores(void);

TOKEN Scanner(void);
int columna(int c);
void AgregarCaracter(int caracter, int posicion);
void LimpiarBuffer(void);
int validacion(char* arch, char c);
int esEstadoFinal(int e);

REG_EXPRESION ProcesarCte(void);
REG_EXPRESION ProcesarId(void);
char * ProcesarOp(void);
void Leer(REG_EXPRESION in);
void Escribir(REG_EXPRESION out);
REG_EXPRESION GenInfijo(REG_EXPRESION e1, char * op, REG_EXPRESION e2);
void Generar(char * co, char * a, char * b, char * c);
char * Extraer(REG_EXPRESION * reg);
int Buscar(char * id, RegTS * TS, TOKEN * t);
void Colocar(char * id, RegTS * TS);
void Chequear(char * s);
void Asignar(REG_EXPRESION izq, REG_EXPRESION der);
void Comenzar(void);
void Terminar(void);
/////////////////gcc2 fuente salida

int main(int argc, char* argv[]) {


    archivo = fopen("entrada.txt", "r+"); 
    // con esto abro el archivo "datos.txt" en la cual hay varios lexemas aser reconocidos
    if (!archivo)
    {
    	// VERIFICO QUE EL ARCHIVO EXISTA 	
        printf("No se pudo abrir el archivo de lectura ");
        return EXIT_FAILURE;
    }

	archivoSalida = fopen("salida.txt", "wb+");

	Objetivo(); /*Este seria el asioma que inicia el proceso de compilacion*/

	fclose(archivo);

	fclose(archivoSalida);
	/*Se procede a cerrar el archivo*/
		
	return EXIT_SUCCESS;
}

RegTS TS[1000] = { {"inicio", INICIO}, {"fin", FIN}, {"leer", LEER}, {"escribir", ESCRIBIR}, {"$", 99} };
int numeroVariableTemporal=1;

/*----------------------------------------------------Validacion-----------------------------------*/


int validacion (char* archivo,char c){

	int i=0;
	while(archivo[i]!='.'){
		i=i+1;
	}
	if ((archivo[i+1] == c)&&(archivo[i+2] =='\0')){
		return 1;
	}else{
		return 0;
	}
}




/* ------------------------------------------PARSER---------------------------------------------------------------*/
/*Éste verifica si los tokens que recibe del Scanner forman secuencias o construcciones válidas, 
según la Gramática Sintáctica del LP correspondiente.Convierte el flujo de tokens en un árbol de análisis sintáctico,
 generalmente implícito. Este árbol representa, de un modo jerárquico, a la secuencia analizada, donde los tokens que
  forman la construcción son las hojas del árbol.*/
  
void Match(TOKEN token){ /*verifica que haya concordancia con el token*/
	if (ProximoToken()!=token){ //Proximo token retorn el proximo caracter a ser correspondido
		ErrorSintactico();
	}
	flagToken=0;
}
/*Procedimiento de analisis sintactico (Pas)*/
void Objetivo(void)
{
 /* <objetivo> -> <programa> FDT #terminar */
 Programa();
 Match(FDT);
 Terminar();
}


void Programa(void){
	
	/* <programa> -> #comenzar INICIO <listaSentencias> FIN */
	 Comenzar();//de inicio semántico en caso de corresponder
	Match(INICIO);
	ListaSentencias(); /*este es un PAS*/
	Match(FIN);
	
}


void ListaSentencias (void){ 
//<listaSentencias> -> <sentencia> {<sentencia>} 
	Sentencia();// la primera que detecta 
	while(1){//ciclo infinito y se detiene al no encontra sentencias
		switch (ProximoToken()){ 
		case ID: case LEER: case ESCRIBIR: //detecto un token correcto 
			Sentencia(); //procesa la SECUENCIA OPCIONAL
			break; 
			default: 
			return; // si no es sentencia termina la funcion
		}//del análisis de caso
 	}// de la repeticion 
}// del proceso

void Sentencia(void){ //Lo que hacemos aca es verificar la concordancia del token y lo que masomenos se espera
					  /* como esta construida la sentencias,en caso de estar bien se procede a almacenar los eventos en un txt*/
	TOKEN tok = ProximoToken();
	REG_EXPRESION izquierda, derecha;
	switch (tok){
		case ID:  //Un ejemplo seria a:=55+55 o... b:=a
			Identificador(&izquierda);//se pasa por refrencia el valor ,para que esta sea llenado
			Match(ASIGNACION); //Invocacion al scanner y verifica si hay concordancia con el token asigancion
			Expresion(&derecha);//lo mismo ,se pasa por referencia asi es asignado el valor de la derecha
			Asignar(izquierda, derecha); //realizo la acciony escribo el output
			Match(PUNTOYCOMA);//verifica que haya concordacia con el punto y coma
			break;
		case LEER:
			Match(LEER);
			Match(PARENIZQUIERDO);
			ListaIdentificadores();
			Match(PARENDERECHO);
			Match(PUNTOYCOMA);
			break;
		case ESCRIBIR:
			Match(ESCRIBIR);
			Match(PARENIZQUIERDO);
			ListaExpresiones();
			Match(PARENDERECHO);
			Match(PUNTOYCOMA);
			break;
		default:
			ErrorSintactico();
			break;
	}
}

void Expresion(REG_EXPRESION * resultado){
	REG_EXPRESION izquierda, derecha;
	TOKEN t;
	char operador[TAMLEX];
	Primaria(&izquierda);
	for(t = ProximoToken(); t == SUMA || t == RESTA; t = ProximoToken()){
		OperadorAditivo(operador);
		Primaria(&derecha);
		izquierda = GenInfijo(izquierda, operador, derecha);
	}
	*resultado = izquierda;
}

void Primaria(REG_EXPRESION * resultado){
	switch(ProximoToken()){
		case ID:
			Identificador(resultado);
			break;
		case CONSTANTE:
			Match(CONSTANTE);
			*resultado = ProcesarCte();
			break;
		case PARENIZQUIERDO:
			Match(PARENIZQUIERDO);
			Expresion(resultado);
			Match(PARENDERECHO);
			break;
	}
}

void OperadorAditivo(char * resultado){
	TOKEN t = ProximoToken();
	if (t == SUMA || t == RESTA){
		Match(t);
		strcpy(resultado, ProcesarOp());
	}else
		ErrorSintactico();
}

void ErrorSintactico(){ //en caso de nmo haber concordancia,se dice que hay error sintactico
	printf("ERROR SINTACTICO");
}

void ErrorLexico(){
	printf("ERROR LEXICO");
}

void ListaIdentificadores(void){
	TOKEN t;
	REG_EXPRESION reg;
	Identificador(&reg);
	Leer(reg);
	for ( t = ProximoToken(); t == COMA; t = ProximoToken()){
		Match(COMA);
		Identificador(&reg);
		Leer(reg);
	}
}

void ListaExpresiones(void){
	TOKEN t;
	REG_EXPRESION reg;
	Expresion(&reg);
	Escribir(reg);
	for ( t = ProximoToken(); t == COMA; t = ProximoToken()){
		Match(COMA);
		Expresion(&reg);
		Escribir(reg);
	}
}

TOKEN ProximoToken(void){
	if (flagToken!=1){
		tokenActual = Scanner();
		flagToken = 1;
		if(tokenActual == ERRORLEXICO){
			ErrorLexico();
		}
		if (tokenActual == ID){
			Buscar(buffer, TS, &tokenActual);
		}

	}
	return tokenActual;
}

void Identificador(REG_EXPRESION * resultado){
	Match(ID);
	*resultado = ProcesarId();
}

/* -----------------------------------------------SCANNER------------------------------------------------------*/
/*Es importante tener en cuenta que el Scanner es una rutina que produce y
 	retorna la representación del correspondiente token, uno por vez, 
 	en la medida que es invocada por el Parser.*/
int esEstadoFinal(int e){ /*evalua siu es estado final*/
	switch (e){
		case 0: case 1: case 3: case 11: case 14:
			return 0;
	}
	return 1;
}

TOKEN Scanner(void){/*Esa funcion simula al scanner(analizador lexico) ,y 
				devuelve un token,esto lo hace con la tabal de transiciones */
	char c;
	int col, i = 0, estado_actual = 0;
	/*Esta es la tabla de transiciones con NUMESTADOS = 15 Y NUMCOL= 13*/
	static int tabla [NUMESTADOS][NUMCOLS]={
		{1,3,5,6,7,8,9,10,11,14,13,0,14},
		{1,1,2,2,2,2,2,2,2,2,2,2,2},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{4,3,4,4,4,4,4,4,4,4,4,4,4},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{14,14,14,14,14,14,14,14,14,12,14,14,14},
		{14,14,14,14,14,14,14,14,14,12,14,14,14},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{14,14,14,14,14,14,14,14,14,14,14,14,14}
	};/*Si nos fijamos al AFD que se ascoia a esta tabla ,podemos ver mas claro como 
		son los identificadore los operadores sus estados final e inicial*/

	LimpiarBuffer();/*Limpio el buffer,este es un vector que contiene los caracteres 
					que son ingresados a medida que se va reconociendo como 
					necesitamos ingresar unos nuevos se debe limpiar*/

	do{
		c = getc(archivo);/*La funcion getc trae caracter a caracter del archivo
							La función getc retorna el carácter siguiente desde el stream de entrada apuntado por stream. Si el stream está en el final de fichero, el indicador del final de fichero para el stream es activado y getc retorna EOF. 
							Si ocurre un error de lectura, el indicador de error para el stream es activado y getc retorna EOF.*/
		col = columna(c);
		estado_actual = tabla[estado_actual][col];
		if (col!= 11){
			AgregarCaracter(c, i); /*Agreamos en el BUFFER EL CARACTER Y SU CONTADOR*/
			i++;
		}
	}while (!esEstadoFinal(estado_actual));

	switch(estado_actual){
		case 2:
			if (col != 11){
				ungetc(c, archivo);
				buffer[i-1] = '\0';
			}
			return ID;
		case 4:
			if (col != 11){
				ungetc(c, archivo);
				buffer[i-1] = '\0';
			}
			return CONSTANTE;
		case 5:
			return SUMA;
		case 6:
			return RESTA;
		case 7:
			return PARENIZQUIERDO;
		case 8:
			return PARENDERECHO;
		case 9:
			return COMA;
		case 10:
			return PUNTOYCOMA;
		case 12:
			return ASIGNACION;
		case 13:
			return FDT;
		case 14:
			return ERRORLEXICO;

	}
	return FDT;
}

int columna(int c){ // matcheo cada caracter con su lexema
	if(isalpha(c)){
		return 0;
	}else if (isdigit(c)){
		return 1;
	}else if (isspace(c)){
		return 11;
	}
	switch (c){
		case '+':
			return 2;
		case '-':
			return 3;
		case '(':
			return 4;
		case ')':
			return 5;
		case ',':
			return 6;
		case ';':
			return 7;
		case ':':
			return 8;
		case '=':
			return 9;
		case EOF:
			return 10;
		default:
			return 12;

	}
}

void AgregarCaracter(int caracter, int posicion){
	buffer[posicion] = caracter;
}

void LimpiarBuffer(void){
	int i;
	for (i=0; i < TAMLEX; i++){
		buffer[i]='\0';
	}
}

/*-----------------------------Rutinas Semanticas-------------------------------*/
/*Tenmos a la rutina semanticas que estas funciones estan asociadas a cada no terminal,hacen una determinada accion*/


REG_EXPRESION ProcesarCte(void){
	/* convierte cadena que representa número a número entero y construye un
registro semántico */
	REG_EXPRESION reg;
	reg.clase = CONSTANTE;
	strcpy(reg.nombre, buffer);
	sscanf(buffer, "%d", &reg.valor);
	return reg;
}

REG_EXPRESION ProcesarId(void){
	REG_EXPRESION reg;
	Chequear(buffer);
	reg.clase = ID;
	strcpy(reg.nombre, buffer);
	return reg;
}

char * ProcesarOp(void){
	return buffer;
}

void Leer(REG_EXPRESION in){
	Generar("Read", in.nombre, "Entera", "");
}

void Escribir(REG_EXPRESION out){
	Generar("Write", Extraer(&out), "Entera", "");
}

REG_EXPRESION GenInfijo(REG_EXPRESION e1, char * op, REG_EXPRESION e2){
	REG_EXPRESION reg;
	char cadenaTemporal[TAMLEX] = "Temp&";
	char numero[TAMLEX];
	char cadenaOperador[TAMLEX];

	if (op[0] == '-')
		strcpy(cadenaOperador, "Restar");
	if (op[0] == '+')
		strcpy(cadenaOperador, "Sumar");
	sprintf(numero, "%d", numeroVariableTemporal);
	numeroVariableTemporal++;
	strcat(cadenaTemporal, numero);
	if (e1.clase == ID)
		Chequear(Extraer(&e1));
	if (e2.clase == ID)
		Chequear(Extraer(&e2));
	Chequear(cadenaTemporal);
	Generar(cadenaOperador, Extraer(&e1), Extraer(&e2), cadenaTemporal);
	strcpy(reg.nombre, cadenaTemporal);
	return reg;
}

/*Generar: una función que recibe cuatro argumentos que son cadenas, que corresponden al
código de operación y a los tres operandos de cada instrucción de la MV; esta función
producirá la correspondiente instrucción en el flujo de salida.*/
void Generar(char * co, char * a, char * b, char * c){
	fprintf(archivoSalida, "%s %s", co, a);

	if (b[0]!='\0')
		fprintf(archivoSalida, "%c%s", ',', b);

	if (c[0]!='\0')
		fprintf(archivoSalida, "%c%s", ',', c);

	fprintf(archivoSalida, "\n");
}
/*Extraer: una función tal que dado un registro semántico, retorna la cadena que contiene.
Esta cadena puede ser un identificador, un código de operación, representar una constante
antes de ser convertida a número entero, etc.*/
char * Extraer(REG_EXPRESION * reg){
	return reg->nombre;
}

int Buscar(char * id, RegTS * TS, TOKEN * t){ //dado una cadena que representa un 
//identificador determina si esta en la tabla de simbolos
	int i = 0;
	while (strcmp("$", TS[i].identificador)){ //strcmp -> hace una comparacion de strings
		if (!strcmp(id, TS[i].identificador)){
			*t = TS[i].t;
			return 1;
		}
		i++;
	}
	return 0;
}

void Colocar(char * id, RegTS * TS){ //Coloca una cadena en la tabla de simbolos
	int i = 4;
	while (strcmp("$", TS[i].identificador)) i++;
	if (i <= 999){
		strcpy(TS[i].identificador, id );
		TS[i].t = ID;
		strcpy(TS[++i].identificador, "$" );
	}
}
void Chequear(char * s){ /* chequea que un carater este en la tabal de simbolos*/
	TOKEN t;
	if (!Buscar(s, TS, &t)){
		Colocar(s, TS);//LO ALAMCENA EN LA TABLA DE SIMBOLOS
		Generar("Declara", s, "Entera", ""); //GENERA EL OUTPUT
	}
}

void Asignar(REG_EXPRESION izq, REG_EXPRESION der){
	/* genera la instrucción para la asignación */
	Generar("Almacena", Extraer(&der), izq.nombre, ""); //escribe en el salid.txt
}

void Comenzar(void){
Generar("COMIENZO", "", "", "");
}

void Terminar(void){
	Generar("FIN", "", "", "");
}
