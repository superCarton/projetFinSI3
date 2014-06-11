/*!
 * \file debug.c
 * \brief Fonctions de mise au point interactive.
 */
#include <stdbool.h>
#include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
#include "machine.h"

//! Dialogue de mise au point interactive pour l'instruction courante.
/*!
 * Cette fonction gère le dialogue pour l'option \c -d (debug). Dans ce mode,
 * elle est invoquée après l'exécution de chaque instruction.  Elle affiche le
 * menu de mise au point et on exécute le choix de l'utilisateur. Si cette
 * fonction retourne faux, on abandonne le mode de mise au point interactive
 * pour les instructions suivantes et jusqu'à la fin du programme.
 * 
 * \param mach la machine/programme en cours de simulation
 * \return vrai si l'on doit continuer en mode debug, faux sinon
 */
 #define NMAX 10
bool debug_ask(Machine *pmach){
	char input[NMAX];
	printf("DEBUG? ");
	scanf("%s",input);
	if(strcmp(input,"h")==0){
		printf("h\thelp\n");
		printf("c\tcontinue(exit interactive debug mode)\n");
		printf("s\tstep by step(next instruction)\n");
		printf("RET\tstep by step(next instruction)\n");
		printf("r\tprint registers\n");
		printf("d\tprint data memory\n");
		printf("t\tprint text(program) memory\n");
		printf("p\tprint text(program) memory\n");
		printf("m\tprint registers and data memory\n");	
	}
	if(strcmp(input,"c")==0)
		return false;
	if(strcmp(input,"r")==0)
		print_cpu(pmach);
	if(strcmp(input,"d")==0)
		print_data(pmach);
	if(strcmp(input,"t")==0)
		print_program(pmach);
	if(strcmp(input,"p")==0)
		print_program(pmach);
	if(strcmp(input,"m")==0){
		print_cpu(pmach);
		print_data(pmach);
	}
	return true;
}