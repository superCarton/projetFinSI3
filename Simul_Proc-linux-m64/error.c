#include <stdio.h>
#include <stdlib.h>

#include "error.h"
/*
 * !Affichage d'une erreur et fin du simulateur.
 *
 * Toutes les erreurs étant fatales on ne revient jamais de cette fonction.
 * L'attribut noreturn est une extension (non standard) de GNU C qui indique ce fait.
 * \Paramètres
 *		err: code de l'erreur
 *  	addr: adresse de l'erreur 
 */
void error(Error err, unsigned addr){
	switch(err){
		case 0:
			printf("There is no error,%#.4x\n",addr);
			exit(0);
		case 1:
			printf("Instruction inconnue,%#.4x\n",addr);
			exit(1);
		case 2:
			printf("Instruction illégale,%#.4x\n",addr);
			exit(1);
		case 3:
			printf("Condition illégale,%#.4x\n",addr);
			exit(1);
		case 4:
			printf("Valeur immédiate interdite,%#.4x\n",addr);
			exit(1);
		case 5:
			printf("Violation de taille du segment de texte,%#.4x\n",addr);
			exit(1);
		case 6:
			printf("Violation de taille du segment de données,%#.4x\n",addr);
			exit(1);
		case 7:
			printf("Violation de taille du segment de pile,%#.4x\n",addr);
			exit(1);
		default:
			exit(1);
	}
}
/*!Affichage d'un avertissement.
* \Paramètres
*     	warn:	code de l'avertissement
*    	addr:	adresse de l'erreur 
*/
void warning(Warning warn, unsigned addr){
	if(warn==WARN_HALT)
		printf("Fin normale du programme (sur HALT): %#.4x\n",addr);
}