/*!
 * \file machine.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "machine.h"
#include "debug.h"
#include "exec.h"
#include "error.h"

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

void print_mem(Instruction * text, unsigned textsize, Word * data, unsigned datasize)
{
	char *c;
	int i;
	printf("Instruction text[] = {\n\t");
	for (i = 0; i < textsize; ++i) {
		c = (i+1) % 4 ? ", " : ",\n\t";
		printf("0x%.8x%s",text[i]._raw,c);
	}
	printf("\n};\nunsigned textsize = %d;\n", textsize);

	printf("\nWord data[] = {\n\t");
	for (i = 0; i < datasize; ++i) {
		c = (i+1) % 4 ? ", " : ",\n\t";
		printf("0x%.8x%s",data[i],c);
	}
	printf("\n};\nunsigned datasize = %d;\n", datasize);
}

void read_program(Machine *mach, const char *programfile)
{
	FILE * fp;
	int i;

	unsigned textsize;
	unsigned datasize;
	unsigned dataend;
	Instruction * text;
	Word * data;

	if ((fp = fopen(programfile, "r")) == NULL) {
		fprintf(stderr, "Ouverture du fichier impossible.\n");
		exit(1);
	}

	fread(&textsize, sizeof(unsigned), 1, fp);
	fread(&datasize, sizeof(unsigned), 1, fp);
	fread(&dataend, sizeof(unsigned), 1, fp);

	text = malloc(textsize * sizeof(Instruction));
	data = malloc(datasize * 4);

	//printf("text\n");
	for (i = 0; i < textsize; ++i) {
		fread(&text[i]._raw, sizeof(uint32_t), 1, fp);
		//print_mem(text, textsize, data, datasize);

	}

	//printf("data\n");
	for (i = 0; i < dataend; ++i) {
		fread(&data[i], sizeof(uint32_t), 1, fp);
		//print_mem(text, textsize, data, datasize);
	}
	fclose(fp);

	load_program(mach, textsize, text, datasize, data, dataend);
}

void write_word(uint32_t word, FILE * fp, int addr)
{
	char *c;

	c = (addr+1) % 4 ? ", " : ",\n\t";
	fwrite(&word, sizeof(uint32_t), 1, fp);
	printf("0x%.8x%s",word,c);
}

void dump_memory(Machine *pmach)
{
	FILE * fp;
	int i;

	// Ouverture du fichier dump.bin
	if ((fp = fopen("dump.bin", "w")) == NULL) {
		fprintf(stderr, "Ouverture du fichier impossible.\n");
		exit(1);
	}

	fwrite(&pmach->_textsize, sizeof(int), 1, fp);
	fwrite(&pmach->_datasize, sizeof(int), 1, fp);
	fwrite(&pmach->_dataend, sizeof(int), 1, fp);

	printf("Instruction text[] = {\n\t");
	for (i = 0; i < pmach->_textsize; ++i) {
		write_word(pmach->_text[i]._raw, fp, i);
	}
	printf("\n};\nunsigned textsize = %d;\n", pmach->_textsize);

	printf("\nWord data[] = {\n\t");
	for (i = 0; i < pmach->_datasize; ++i) {
		write_word(pmach->_data[i], fp, i);
	}
	printf("\n};\nunsigned datasize = %d;\n", pmach->_datasize);
	printf("unsigned dataend = %d;\n", pmach->_dataend);

	fclose(fp);
}

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
