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
		case ERR_NOERROR://!< Pas d'erreur
			printf("There is no error: %#.4x\n",addr);
			exit(0);
		case ERR_UNKNOWN://!< Instruction inconnue
			printf("ERROR: Instruction inconnue %#.4x\n",addr);
			exit(1);
		case ERR_ILLEGAL://!< Instruction illégale
			printf("ERROR: Instruction illégale %#.4x\n",addr);
			exit(1);
		case ERR_CONDITION://!< Condition illégale
			printf("ERROR: Condition illégale %#.4x\n",addr);
			exit(1);
		case ERR_IMMEDIATE://!< Valeur immédiate interdite
			printf("ERROR: Valeur immédiate interdite %#.4x\n",addr);
			exit(1);
		case ERR_SEGTEXT://!< Violation de taille du segment de texte
			printf("ERROR: Violation de taille du segment de texte %#.4x\n",addr);
			exit(1);
		case ERR_SEGDATA://!< Violation de taille du segment de données
			printf("ERROR: Violation de taille du segment de données %#.4x\n",addr);
			exit(1);
		case ERR_SEGSTACK://!< Violation de taille du segment de pile
			printf("ERROR: Violation de taille du segment de pile %#.4x\n",addr);
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
	if(warn==WARN_HALT)//!< Fin normale du programme (sur HALT)
		printf("WARNING: HALT reached at address %#.4x\n",addr);
}
