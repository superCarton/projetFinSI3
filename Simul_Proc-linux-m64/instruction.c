/*!
 * \file instruction.c
 * \brief Description du jeu d'instruction.
 * \author Jonathan Pujol
 * \date 11 juin 2014
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "instruction.h"

//! Forme imprimable des codes opérations
const char *cop_names[] = {
	"ILLOP", "NOP", "LOAD", "STORE", "ADD", "SUB", "BRANCH", "CALL", "RET", "PUSH", "POP", "HALT"
};


//! Forme imprimable des conditions
const char *condition_names[] = {
	"NC", "EQ", "NE", "GT", "GE", "LT", "LE"
};

//! Impression de la variable de l'instruction
/*!
 *La fonction print_op permet d'imprimer la variable de l'instruction passée en paramètre.
 *Elle regarde de quel type est l'instruction (direct, indirect, ...) et choisi le mode d'affichage en fonction.
 *
 *\param instr l'instruction dont on doit imprimer la variable
 */
void print_op(Instruction instr) {
	// instruction à valeur absolu
	if (instr.instr_generic._immediate == 0 && instr.instr_generic._indexed == 0) {
		printf("@0x%.4x", instr.instr_generic._pad);
	}
	// instruction immédiate
	else if(instr.instr_generic._immediate == 1 && instr.instr_generic._indexed == 0) {
		printf("#%u", instr.instr_generic._pad);
	}
	// instruction à une adresse relative indexée
	else if (instr.instr_generic._immediate == 0 && instr.instr_generic._indexed == 1) {
		printf("%d[R%.2u]", instr.instr_indexed._offset, instr.instr_indexed._rindex);
	}
}

//! Impression d'une instruction sous forme lisible (désassemblage)
/*!
 * \param instr l'instruction à imprimer
 * \param addr son adresse
 */
void print_instruction(Instruction instr, unsigned addr) {
	switch ( instr.instr_generic._cop) {
		case RET :
		case HALT :
		case ILLOP :
		case NOP :
		{
			printf("%s", cop_names[instr.instr_generic._cop]);
		}
		break;

		case LOAD :
		case STORE :
		case ADD :
		case SUB : 
		{
			printf("%s R%.2d, ", cop_names[instr.instr_generic._cop], instr.instr_generic._regcond);
			print_op(instr);
		}
		break;

		case PUSH :
		case POP :
		{
			printf("%s ", cop_names[instr.instr_generic._cop]);
			print_op(instr);
		}
		break;

		default :
		{
			printf("%s %s, ", cop_names[instr.instr_generic._cop], condition_names[instr.instr_generic._regcond]);
			print_op(instr);	
		}
	}
}