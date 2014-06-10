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

void print_op(Instruction instr) {
	if (instr.instr_generic._immediate == 0 && instr.instr_generic._indexed == 0) {
		printf("@0x%.4x", instr.instr_generic._pad);
	}
	else if(instr.instr_generic._immediate == 1 && instr.instr_generic._indexed == 0) {
		printf("#%u", instr.instr_generic._pad);
	}
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
	if (instr.instr_generic._cop == RET || instr.instr_generic._cop == HALT || 
		   instr.instr_generic._cop == ILLOP || instr.instr_generic._cop == NOP) {
		printf("%s", cop_names[instr.instr_generic._cop]);
	}
	else if (instr.instr_generic._cop == LOAD || instr.instr_generic._cop == STORE || 
	   instr.instr_generic._cop == ADD || instr.instr_generic._cop == SUB) {
		printf("%s R%.2d, ", cop_names[instr.instr_generic._cop], instr.instr_generic._regcond);
		print_op(instr);
	}
	else if (instr.instr_generic._cop == PUSH || instr.instr_generic._cop == POP) {
		printf("%s ", cop_names[instr.instr_generic._cop]);
		print_op(instr);
	}
	else {
		printf("%s %s, ", cop_names[instr.instr_generic._cop], condition_names[instr.instr_generic._regcond]);
		print_op(instr);
	}
}
