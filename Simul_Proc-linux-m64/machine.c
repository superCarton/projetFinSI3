/*!
 * \file machine.c
 * \brief Affiche l'état de la mémoire et simule l'exécution d'un programme
 * \author Mathieu Boutelier
 */

#include <stdio.h>
#include <stdlib.h>

#include "machine.h"
#include "debug.h"
#include "exec.h"
#include "error.h"

//! Chargement d'un programme
/*!
 * En plus d'initialiser les données fournis en paramètre,
 * on affecte les valeurs suivantes aux autres segments :
 *	 	- Le compteur ordinal PC est réinitialisé à l'adresse 0 du programme.
 *		- Le code condition est initialisé au code inconnu CC_U.
 *		- Les registres sont réinitialisés à la valeur 0.
 *		- Le registre SP est initialisé à la valeur de la tête de pile datasize - 1.
 *
 * \param pmach la machine en cours d'exécution
 * \param textsize taille utile du segment de texte
 * \param text le contenu du segment de texte
 * \param datasize taille utile du segment de données
 * \param data le contenu initial du segment de texte
 *
 */
void load_program(Machine *pmach,
              unsigned textsize, Instruction text[textsize],
              unsigned datasize, Word data[datasize],  unsigned dataend)
{
	pmach->_textsize = textsize;
	pmach->_dataend = dataend;
	pmach->_datasize = datasize;
	pmach->_text = text;
	pmach->_data = data;
	pmach->_pc = 0;
	pmach->_cc = CC_U;

	for (int i = 0; i < NREGISTERS; ++i) {
		pmach->_registers[i] = 0;
	}
	
	pmach->_sp = datasize - 1;
}

//! Lecture d'un programme depuis un fichier binaire
/*!
 * On initialise les segments aux valeurs lu dans le fichier
 * dans l'odre décrit dans machine.h.
 *
 * On initialise ensuite la machine à l'aide de la fonction load_program()
 *
 * \param pmach la machine à simuler
 * \param programfile le nom du fichier binaire
 *
 */
void read_program(Machine *mach, const char *programfile)
{
	// Pointeur sur le fichier binaire
	FILE * fp;
	int i;
	unsigned textsize;
	unsigned datasize;
	unsigned dataend;
	Instruction * text;
	Word * data;

	// On ouvre le fichier en lecture
	if ((fp = fopen(programfile, "r")) == NULL) {
		fprintf(stderr, "Ouverture du fichier impossible.\n");
		exit(1);
	}

	// Initialisation de textsize
	fread(&textsize, sizeof(unsigned), 1, fp);

	// Initialisation de datasize
	fread(&datasize, sizeof(unsigned), 1, fp);

	// Initialisation de dataend
	fread(&dataend, sizeof(unsigned), 1, fp);

	text = malloc(textsize * sizeof(Instruction));
	data = malloc(datasize * 4);

	// Initialisation du segment de texte
	for (i = 0; i < textsize; ++i) {
		fread(&text[i]._raw, sizeof(uint32_t), 1, fp);
	}

	// Initialisation du segment de donnée
	for (i = 0; i < dataend; ++i) {
		fread(&data[i], sizeof(uint32_t), 1, fp);
	}
	
	// Fermeture du fichier
	fclose(fp);

	// Initialisation des données dans la machine
	load_program(mach, textsize, text, datasize, data, dataend);
}

//! Affichage du programme et des données
/*!
 * Affichage des données et des instructions sous forme héxadécimal.
 * Dump binaire dans le fichier dump.bin, on peut exécuter ce programme
 * à l'aide de l'option -b de text_simul.c.
 *
 * On écrit les données dans l'ordre de lecture de read_program().
 *
 * \param pmach la machine en cours d'exécution
 *
 */
void dump_memory(Machine *pmach)
{
	FILE * fp;
	int i;
	char *c;

	// Ouverture du fichier dump.bin en écriture
	if ((fp = fopen("dump.bin", "w")) == NULL) {
		fprintf(stderr, "Ouverture du fichier impossible.\n");
		exit(1);
	}

	// Ecriture de la taille du segment texte
	fwrite(&pmach->_textsize, sizeof(int), 1, fp);

	// Ecriture de la taille du segment de données
	fwrite(&pmach->_datasize, sizeof(int), 1, fp);

	// Ecriture de l'adresse de fin des données
	fwrite(&pmach->_dataend, sizeof(int), 1, fp);

	// Ecriture du segment texte
	printf("Instruction text[] = {\n\t");
	for (i = 0; i < pmach->_textsize; ++i) {
		c = (i+1) % 4 ? ", " : ",\n\t";
		fwrite(&pmach->_text[i]._raw, sizeof(uint32_t), 1, fp);
		printf("0x%.8x%s",pmach->_text[i]._raw,c);
	}
	printf("\n};\nunsigned textsize = %d;\n", pmach->_textsize);

	// Ecriture du segment de données
	printf("\nWord data[] = {\n\t");
	for (i = 0; i < pmach->_datasize; ++i) {
		c = (i+1) % 4 ? ", " : ",\n\t";
		fwrite(&pmach->_data[i], sizeof(uint32_t), 1, fp);
		printf("0x%.8x%s",pmach->_data[i],c);
	}
	printf("\n};\nunsigned datasize = %d;\n", pmach->_datasize);
	printf("unsigned dataend = %d;\n", pmach->_dataend);

	// Fermeture du fichier
	fclose(fp);
}

//! Affichage des instructions du programme
/*!
 * \param pmach la machine en cours d'exécution
 */
void print_program(Machine *pmach)
{
	printf("\n*** PROGRAM (size: %d) ***\n", pmach->_textsize);

	int i;

	for(i = 0; i < pmach->_textsize; ++i) {
		printf("0x%.4x: 0x%.8x\t", i, pmach->_text[i]._raw);
		print_instruction(pmach->_text[i], i);
		printf("\n");
	}
}

//! Affichage des données du programme
/*!
 * \param pmach la machine en cours d'exécution
 */
void print_data(Machine *pmach)
{
	int i;
	Word word;
	char c = '\t';

	printf("\n*** DATA (size: %d, end = 0x%.8x (%d)) ***\n", pmach->_datasize, pmach->_dataend, pmach->_dataend);

	for(i = 0; i < pmach->_datasize; ++i) {
		c = (i+1) % 3 ? '\t': '\n';
		word = *(pmach->_data + i);
		printf("0x%.4x: 0x%.8x %d%c",i,word,word,c);
	}

	printf("\n");
}

//! Affichage des registres du CPU
/*!
 * \param pmach la machine en cours d'exécution
 */
void print_cpu(Machine *pmach)
{
	char c;
	int i;
	Word word;
	
	printf("\n*** CPU ***\n");

	switch(pmach->_cc) {
		case CC_Z:
			c = 'Z';
			break;
		case CC_N:
			c = 'N';
			break;
		case CC_P:
			c = 'P';
			break;
		default:
			c = 'U';
	}

	printf("PC: 0x%.8x\tCC: %c\n\n", pmach->_pc, c);

	for(i = 0; i < NREGISTERS; ++i) {
		c = (i+1) % 3 ? '\t': '\n';
		word = pmach->_registers[i];
		printf("R%.2d: 0x%.8x %d%c",i,word,word,c);
	}
	printf("\n");
}

//! Simulation
/*!
 * Appel de la fonction error() si la valeur de pc est plus grande
 * que la valeur de textsize.
 *
 * Affichage de la ligne à exécuter avec la fonction trace().
 *
 * Décodage et exécution de l'instruction avec la fonction decode_execute().
 *
 * Lancement de la fonction de débugage avec debug_ask().
 *
 * On incrémente la valeur de PC à chaque itération de la boucle. 
 *
 * \param pmach la machine en cours d'exécution
 * \param debug mode de mise au point (pas à apas) ?
 */
void simul(Machine *pmach, bool debug)
{
	bool execute = true;

	while(execute) {
		if (pmach->_pc >= pmach->_textsize) {
			error(ERR_SEGTEXT, pmach->_pc);
		} 
		pmach->_pc = pmach->_pc + 1;
		trace("Executing", pmach, pmach->_text[pmach->_pc - 1], pmach->_pc - 1);
		execute = decode_execute(pmach, pmach->_text[pmach->_pc - 1]);
		if (debug) {
			debug = debug_ask(pmach);
		}	
	}
}
