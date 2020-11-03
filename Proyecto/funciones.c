#include "funciones.h"
RegTS TS[1000] = {
    {
        "inicio",
        INICIO
    },
    {
        "fin",
        FIN
    },
    {
        "leer",
        LEER
    },
    {
        "escribir",
        ESCRIBIR
    },
    {
        "$",
        99
    }
};
int numeroVariableTemporal=1;


/* ------------------------------------------PARSER---------------------------------------------------------------*/

void Objetivo(void){
 /* <objetivo> -> <programa> FDT #terminar */
 Programa();
 Match(FDT);
 Terminar();
}


void Programa(void){
	Match(INICIO);
	ListaSentencias();
	Match(FIN);
	Terminar();
}

void ListaSentencias(void){
	Sentencia();
	while(1){
		switch(ProximoToken()){
			case ID: case LEER: case ESCRIBIR:
				Sentencia();
				break;
			default:
				return;
		}
	}
}

void Sentencia(void){
	TOKEN tok = ProximoToken();
	REG_EXPRESION izquierda, derecha;
	switch (tok){
		case ID:
			Identificador(&izquierda);
			Match(ASIGNACION);
			Expresion(&derecha);
			Asignar(izquierda, derecha);
			Match(PUNTOYCOMA);
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

void ErrorSintactico(){
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
void Match(TOKEN token){
	if (ProximoToken()!=token){
		ErrorSintactico();
	}
	flagToken=0;
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
 	retorna la representaci?n del correspondiente token, uno por vez, 
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
		{ 1, 3, 5, 6, 7, 8, 9,10,11,14,13, 0,14},
		{ 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
		{14,14,14,14,14,14,14,14,14,14,14,14,14},
		{ 4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,4},
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
							La funci?n getc retorna el car?cter siguiente desde el stream de entrada apuntado por stream. Si el stream est? en el final de fichero, el indicador del final de fichero para el stream es activado y getc retorna EOF. 
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
/*Cada registro sem�ntico contendr� datos sobre cada
operando, como, por ejemplo, d�nde est� almacenado y cu�l es su valor*/
REG_EXPRESION ProcesarCte(void){
	/* Convierte cadena que representa numero a entero y construye un registro semantico */
	REG_EXPRESION reg;
	reg.clase = CONSTANTE;
	strcpy(reg.nombre, buffer);
	sscanf(buffer, "%d", &reg.valor);
	return reg;
}

REG_EXPRESION ProcesarId(void){
	 /* Declara ID y construye el correspondiente registro semantico */
	REG_EXPRESION reg;
	Chequear(buffer);
	reg.clase = ID;
	strcpy(reg.nombre, buffer);
	return reg;
}

char * ProcesarOp(void){
	 /* Declara OP y construye el correspondiente registro semantico */
	return buffer;
}

void Leer(REG_EXPRESION in){
	/* Genera la instruccion para leer */
	Generar("Lee", in.nombre, "Entera", "");
}

void Escribir(REG_EXPRESION out){
	/* Genera la instruccion para escribir */
	Generar("Escribe", Extraer(&out), "Entera", "");
}

REG_EXPRESION GenInfijo(REG_EXPRESION e1, char * op, REG_EXPRESION e2){
	/* Genera la instruccion para una operacion infija y construye un registro semantico con el resultado */
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

void Generar(char * co, char * a, char * b, char * c){
	fprintf(archivoSalida, "%s %s", co, a);

	if (b[0]!='\0')
		fprintf(archivoSalida, "%c%s", ',', b);

	if (c[0]!='\0')
		fprintf(archivoSalida, "%c%s", ',', c);

	fprintf(archivoSalida, "\n");
}

char * Extraer(REG_EXPRESION * reg){
	return reg->nombre;
}

int Buscar(char * id, RegTS * TS, TOKEN * t){
	int i = 0;
	while (strcmp("$", TS[i].identificador)){
		if (!strcmp(id, TS[i].identificador)){
			*t = TS[i].t;
			return 1;
		}
		i++;
	}
	return 0;
}

void Colocar(char * id, RegTS * TS){
	int i = 4;
	while (strcmp("$", TS[i].identificador)) i++;
	if (i <= 999){
		strcpy(TS[i].identificador, id );
		TS[i].t = ID;
		strcpy(TS[++i].identificador, "$" );
	}
}
void Chequear(char * s){
	TOKEN t;
	if (!Buscar(s, TS, &t)){
		Colocar(s, TS);
		Generar("Declara", s, "Entera", "");
	}
}

void Asignar(REG_EXPRESION izq, REG_EXPRESION der){
	Generar("Almacena", Extraer(&der), izq.nombre, "");
}

void Comenzar(void){

}

void Terminar(void){
	Generar("Se terminaron las instrucciones", "", "", "");
}



///
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


