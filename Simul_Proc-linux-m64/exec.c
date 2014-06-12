/*!
 *
 * \file exec.c
 * \brief Décode, exécute et affiche les instructions
 * \author Romain Guillot
 */

#include <stdio.h>
#include <unistd.h>
#include "exec.h"
#include "error.h"

bool instr_illop(Machine *pmach, Instruction instr);
bool instr_nop(Machine *pmach, Instruction instr);
bool instr_load(Machine *pmach, Instruction instr);
bool instr_store(Machine *pmach, Instruction instr);
bool instr_add(Machine *pmach, Instruction instr);
bool instr_sub(Machine *pmach, Instruction instr);
bool instr_branch(Machine *pmach, Instruction instr);
bool instr_call(Machine *pmach, Instruction instr);
bool instr_ret(Machine *pmach, Instruction instr);
bool instr_push(Machine *pmach, Instruction instr);
bool instr_pop(Machine *pmach, Instruction instr);
bool instr_halt(Machine *pmach, Instruction instr);
bool cmp_op(Machine *pmach, Instruction instr);
unsigned calculate_adress(Machine *pmach, Instruction instr);
void set_cc(Machine *pmach, Word value);
void error_instruction(Machine *pmach, Error err);
void check_index_register(Machine *pmach, int index);
void check_sp(Machine *pmach, int sp);
void check_adress_data(Machine *pmach, unsigned adress);

//! Décodage et exécution d'une instruction
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return faux après l'exécution de \c HALT ; vrai sinon
 */
bool decode_execute(Machine *pmach, Instruction instr){

	bool b = false;
	switch (instr.instr_generic._cop){
		case ILLOP : b = instr_illop(pmach, instr); break;
		case NOP : b = instr_nop(pmach, instr); break;
		case LOAD : b = instr_load(pmach, instr); break;
		case STORE : b = instr_store(pmach, instr); break;
		case ADD : b = instr_add(pmach, instr); break;
		case SUB : b = instr_sub(pmach, instr); break;
		case BRANCH : b = instr_branch(pmach, instr); break;
		case CALL : b = instr_call(pmach, instr); break;
		case RET : b = instr_ret(pmach, instr); break;
		case PUSH : b = instr_push(pmach, instr); break;
		case POP : b = instr_pop(pmach, instr); break;
		case HALT : b = instr_halt(pmach, instr); break;
		default : error_instruction(pmach, ERR_UNKNOWN); break;
	}
	return b;
}

//! Exécution de ILLOP
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return toujours FAUX
 * Interruption du programme
 */
bool instr_illop(Machine *pmach, Instruction instr){
	
	error_instruction(pmach, ERR_NOERROR);
	return false;
}

//! Exécution de NOP
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return toujours vrai
 * Ne rien faire
 */
bool instr_nop(Machine *pmach, Instruction instr){
	
	return true;
}

//! Décodage et exécution de LOAD
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_load(Machine *pmach, Instruction instr){

	check_index_register(pmach, instr.instr_generic._regcond);

	if (instr.instr_generic._immediate == 0){ //!< Mode absolu

		unsigned addr = calculate_adress(pmach, instr);
		check_adress_data(pmach, addr);
		pmach->_registers[instr.instr_generic._regcond] =  pmach->_data[addr];

	} else if (instr.instr_generic._immediate == 1){ //!< Mode immédiat
		pmach->_registers[instr.instr_generic._regcond] = instr.instr_immediate._value;
	}
	else { //!< Instruction illégale
		error_instruction(pmach, ERR_ILLEGAL);
	}

	set_cc(pmach, pmach->_registers[instr.instr_generic._regcond]);
	return true;
}

//! Décodage et exécution de STORE
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_store(Machine *pmach, Instruction instr){

	if (instr.instr_generic._immediate == 0){ //!< Mode absolu

		check_index_register(pmach, instr.instr_generic._regcond);
		unsigned addr = calculate_adress(pmach, instr);
		check_adress_data(pmach, addr);
		if (addr >= pmach->_dataend){
			error_instruction(pmach, ERR_SEGDATA);
		}
		pmach->_data[addr] = pmach->_registers[instr.instr_generic._regcond];

	} else { //!< Instruction illégale
		error_instruction(pmach, ERR_IMMEDIATE);

	}
	
	return true;
}

//! Décodage et exécution de ADD
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_add(Machine *pmach, Instruction instr){
	
	check_index_register(pmach, instr.instr_generic._regcond);

	if (instr.instr_generic._immediate == 0){ //!< Mode absolu

		unsigned addr = calculate_adress(pmach, instr);
		check_adress_data(pmach, addr);
		pmach->_registers[instr.instr_generic._regcond] += pmach->_data[addr];

	} else if (instr.instr_generic._immediate == 1) { //!< Mode immédiat
		pmach->_registers[instr.instr_generic._regcond] += instr.instr_immediate._value;
	} else { //!< Instruction illégale
		error_instruction(pmach, ERR_ILLEGAL);
	}

	set_cc(pmach, pmach->_registers[instr.instr_generic._regcond]);
	return true;
}

//! Décodage et exécution de SUB
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_sub(Machine *pmach, Instruction instr){

	check_index_register(pmach, instr.instr_generic._regcond);
 
	if (instr.instr_generic._immediate == 0){ //!< Mode absolu

		unsigned addr = calculate_adress(pmach, instr);
		check_adress_data(pmach, addr);
		pmach->_registers[instr.instr_generic._regcond] -= pmach->_data[addr];

	} else if (instr.instr_generic._immediate == 1) { //!< Mode immédiat
		pmach->_registers[instr.instr_generic._regcond] -= instr.instr_immediate._value;
	} else { //!< Instruction illégale
		error_instruction(pmach, ERR_ILLEGAL);
	}

	set_cc(pmach, pmach->_registers[instr.instr_generic._regcond]);
	return true;
}

//! Décodage et exécution de BRANCH
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_branch(Machine *pmach, Instruction instr){
	
	if (instr.instr_generic._immediate == 0){ //!< Mode absolu
		if (cmp_op(pmach, instr)){ //!< Verification condition
			unsigned addr = calculate_adress(pmach, instr);
			if (addr < 0 || addr >= pmach->_textsize){ //!< Verification emplacement pc
				error_instruction(pmach, ERR_SEGTEXT); 
			}			
			pmach->_pc = addr;
		}
	} else { //!< Instruction illégale
		error_instruction(pmach, ERR_IMMEDIATE);
	}

	return true;
}

//! Décodage et exécution de CALL
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_call(Machine *pmach, Instruction instr){

	if (instr.instr_generic._immediate == 0){ //!< Mode absolu
		if (cmp_op(pmach, instr)){ //!< Verification condition
			check_sp(pmach, pmach->_sp);
			unsigned addr = calculate_adress(pmach, instr);
			pmach->_data[pmach->_sp] = pmach->_pc;
			pmach->_sp -= 1;
			if (addr < 0 || addr >= pmach->_textsize){ //!< Verification emplacement pc
				error_instruction(pmach, ERR_SEGTEXT); 
			}
			pmach->_pc = addr;
		}
	} else { //!< Instruction illégale
		error_instruction(pmach, ERR_IMMEDIATE);
	}

	return true;
}

//! Décodage et exécution de RET
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_ret(Machine *pmach, Instruction instr){
	
	check_sp(pmach, pmach->_sp + 1);
	pmach->_sp += 1;
	pmach->_pc = pmach->_data[pmach->_sp];
	return true;
}

//! Décodage et exécution de PUSH
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_push(Machine *pmach, Instruction instr){

	check_sp(pmach, pmach->_sp);

	if (instr.instr_generic._immediate == 0){ //!< Mode absolu

		unsigned addr = calculate_adress(pmach, instr);
		check_adress_data(pmach, addr);
		pmach->_data[pmach->_sp] = pmach->_data[addr];

	} else if (instr.instr_generic._immediate == 1){ //!< Mode immediat

		pmach->_data[pmach->_sp] = instr.instr_immediate._value;

	} else { //!< Instruction illégale
		error_instruction(pmach, ERR_ILLEGAL);
	}

	check_sp(pmach, pmach->_sp - 1);
	pmach->_sp -= 1;
	return true;
}

//! Décodage et exécution de POP
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai
 */
bool instr_pop(Machine *pmach, Instruction instr){

	if (instr.instr_generic._immediate != 0){ //!< Instruction illégale
		error_instruction(pmach, ERR_IMMEDIATE);
	}

	unsigned addr = calculate_adress(pmach, instr);
	check_adress_data(pmach, addr);
	check_sp(pmach, pmach->_sp + 1);
	pmach->_sp += 1;
	if (addr >= pmach->_dataend){
		error_instruction(pmach, ERR_SEGDATA);
	}
	pmach->_data[addr] = pmach->_data[pmach->_sp];

	return true;
}

//! Décodage et exécution de HALT
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return faux
 */
bool instr_halt(Machine *pmach, Instruction instr){
	warning(WARN_HALT, pmach->_pc-1);
	return false;
}

//! Verification du code operande
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return vrai si cc respecte la condition; faux sinon
 */
bool cmp_op(Machine *pmach, Instruction instr){

	bool b = false;
	int cc = pmach->_cc;

	switch (instr.instr_generic._regcond){	
		case NC : b = true; break; //!< Résultat quelconque
		case EQ : b = (cc == CC_Z) ? true : false; break; //!< Résultat nul
		case NE : b = (cc == CC_P || cc == CC_N) ? true : false; break; //!< Résultat différent de 0
		case GT : b = (cc == CC_P) ? true : false; break; //!< Résultat positif
		case GE : b = (cc == CC_Z || cc == CC_P) ? true : false; break; //!< Résultat positif ou nul
		case LT : b = (cc == CC_N) ? true : false; break; //!< Résultat négatif
		case LE : b = (cc == CC_N || cc == CC_Z) ? true : false; break; //!< Résultat négatif ou nul
		default : error_instruction(pmach, ERR_CONDITION); break;
	}

	return b;		
}

//! Assignation du code operande
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param value valeur du dernier resultat
 */
void set_cc(Machine *pmach, Word value){
	
	if (value<0){ //!< Valeur négative
		pmach->_cc = CC_N;
	} else if (value>0){ //!< Valeur positive
		pmach->_cc = CC_P;
	} else { //!< Valeur nulle
		pmach->_cc = CC_Z;
	}
}

//! Calcule l'adresse en fonction de l'instruction
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param instr l'instruction à exécuter
 * \return l'adresse
 * on suppose que instr.instr_generic._immediate = 0
 */
unsigned calculate_adress(Machine *pmach, Instruction instr){
	
	unsigned addr;
	if (instr.instr_generic._indexed == 1){ //!< Mode indexé
		check_index_register(pmach, instr.instr_indexed._rindex);
		addr = pmach->_registers[instr.instr_indexed._rindex]+instr.instr_indexed._offset;
	} else if (instr.instr_generic._indexed == 0){ //!< Mode absolu
		addr = instr.instr_absolute._address;
	}

	return addr;
}

//! Verifie que l'on accede pas à un registre inconnu
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param index l'indexe du registre à acceder
 */
void check_index_register(Machine *pmach, int index){
	if (index<0 || index > (NREGISTERS-1)){
		error_instruction(pmach, ERR_ILLEGAL);
	} 
}

//! Verifie que l'on accede pas en dehors de la pile, ou que la pile est pleine
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param sp l'indexe de la pile à acceder
 */
void check_sp(Machine *pmach, int sp){
	if (sp < pmach->_dataend || sp > (pmach->_datasize-1)){
		error_instruction(pmach, ERR_SEGSTACK);
	}
}

//! Verifie que l'on accede pas en dehors de la memoire
/*!
 * \param pmach la machine/programme en cours d'exécution
 * \param sp l'adresse de la memoire à acceder
 */
void check_adress_data(Machine *pmach, unsigned adress){
	if (adress < 0 || adress >= pmach->_datasize){
		error_instruction(pmach, ERR_SEGDATA);
	}
}

//! Instruction illégale
/*!
 * Affiche l'instruction illégale
 * Arrete l'éxécution du programme
 * \param pmach la machine/programme en cours d'exécution
 */
void error_instruction(Machine *pmach, Error err){
	error(err, pmach->_pc);
}

//! Trace de l'exécution
/*!
 * On écrit l'adresse et l'instruction sous forme lisible.
 *
 * \param msg le message de trace
 * \param pmach la machine en cours d'exécution
 * \param instr l'instruction à exécuter
 * \param addr son adresse
 */
void trace(const char *msg, Machine *pmach, Instruction instr, unsigned addr){

	printf("TRACE: %s: 0x%.4x: ", msg, (uint32_t)addr);
	print_instruction(instr, addr);
	printf("\n");
}
