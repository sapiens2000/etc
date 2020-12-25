#define _CRT_SECURE_NO_WARNINGS
#define TRUE 1
#define FALSE 0

/* for bit operation */
#define OP_BIT 127		// 7 bits
#define IMM_BIT 4095	// 12 bits
#define REG_BIT 31		// 5 bits

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

//clock cycles
long long cycles = 0;

// registers
long long int regs[32];

// program counter
uint32_t pc = 0;

// memory
#define INST_MEM_SIZE 32*1024
#define DATA_MEM_SIZE 32*1024
unsigned long inst_mem[INST_MEM_SIZE]; //instruction memory
unsigned long long data_mem[DATA_MEM_SIZE]; //data memory

//misc. function
int init(char* filename);

void fetch();//fetch an instruction from a instruction memory
void decode();//decode the instruction and read data from register file
void exe();//perform the appropriate operation 
void mem();//access the data memory
void wb();//write result of arithmetic operation or data read from the data memory if required

void print_cycles();
void print_reg();
void print_pc();

/* global variables */
unsigned long op;
long offset, imm, sign_bit;
unsigned long rd, rs1, rs2;
unsigned long isa;


int main(int ac, char *av[])
{
	
	if (ac <3 )
	{
		printf("./mips_sim filename mode\n");
        return -1;
	}
	

	char done = FALSE;
	if(init(av[1])!=0)
        return -1;
	while (!done)
	{

		fetch();
        decode();
		exe();
		mem();
		wb();


		cycles++;    //increase clock cycle

		//if debug mode, print clock cycle, pc, reg 
		if(*av[2]=='0'){
					 print_cycles();  //print clock cycles
					 print_pc();		 //print pc
					 print_reg();	 //print registers
        }
	
     		// check the exit condition, do not delete!! 
		if (regs[9] == 10)  //if value in $t1 is 10, finish the simulation
			done = TRUE;
	}

    if(*av[2]=='1')
    {
        print_cycles();  //print clock cycles
        print_pc();		 //print pc
        print_reg();	 //print registers
    }

	return 0;
}


/* initialize all datapat elements
//fill the instruction and data memory
//reset the registers
*/
int init(char* filename)
{
	FILE* fp = fopen(filename, "r");
	int i;
	long inst;

	if (fp == NULL)
	{
		fprintf(stderr, "Error opening file.\n");
		return -1;
	}

	/* fill instruction memory */
	i = 0;
	while (fscanf(fp, "%lx", &inst) == 1)
	{
		inst_mem[i++] = inst;
	}


	/*reset the registers*/
	for (i = 0; i<32; i++)
	{
		regs[i] = 0;
	}

	/*reset pc*/
	pc = 0;
    /*reset clock cycles*/
    cycles=0;

	/* reset global variables*/
	isa = 0;
	op = 0;
	offset = 0;
	imm = 0;
	rd = 0;
	rs1 = 0;
	rs2 = 0;

    return 0;
}

void fetch() {
	/* get isa */
	isa = inst_mem[pc / 4];
	pc = pc + 4;
}

void decode() {
	/* get opcode*/
	op = (isa & OP_BIT);

	/* all */
	switch (op) {
		/* add opcode == 0110011 */
	case 51:
		rd = (isa >> 7) & REG_BIT;
		rs1 = (isa >> 15) & REG_BIT;
		rs2 = (isa >> 20) & REG_BIT;
		break;
		/* addi opcode == 0010011 */
	case 19:
		rd = (isa >> 7) & REG_BIT;
		rs1 = (isa >> 15) & REG_BIT;

		/* get sign bit */
		sign_bit = (isa >> 31) & 1;
		/* if negative */
		if (sign_bit == 1) {
			imm = 1048575 << 12;
			imm = imm + ((isa >> 20) & IMM_BIT);
		}
		else
			imm = (isa >> 20) & IMM_BIT;
		break;
		/* sd opcode == 0100011 */
	case 35:
		rs1 = (isa >> 15) & REG_BIT;
		rs2 = (isa >> 20) & REG_BIT;

		/* get sign bit */
		sign_bit = (isa >> 31) & 1;
		/* if negative */
		if (sign_bit == 1) {
			imm = 1048575 << 12;
			imm = imm + (((isa >> 25) & 127) << 5) + ((isa >> 7) & REG_BIT);
		}
		else
			imm = (((isa >> 25) & 127) << 5) + ((isa >> 7) & REG_BIT);
		break;
		/* ld opcode == 0000011 */
	case 3:
		rd = (isa >> 7) & REG_BIT;
		rs1 = (isa >> 15) & REG_BIT;

		/* get sign bit */
		sign_bit = (isa >> 31) & 1;
		/* if negative */
		if (sign_bit == 1) {
			imm = 1048575 << 12;
			imm = (isa >> 20) & IMM_BIT;
		}
		else
			imm = (isa >> 20) & IMM_BIT;
		break;
		/* beq opcode == 1100011 */
	case 99:
		rs1 = (isa >> 15) & REG_BIT;
		rs2 = (isa >> 20) & REG_BIT;
		
		/* get sign bit */
		sign_bit = (isa >> 31) & 1;
		/* if negative */
		if (sign_bit == 1) {
			imm = 1048575 << 12;
			imm = imm + ((((isa >> 7) & 1) << 10) + ((isa >> 8) & 15) + (((isa >> 25) & 63) << 4) + (((isa >> 31) & 1) << 31) << 1);
		}
		else
			imm = ((((isa >> 7) & 1) << 10) + ((isa >> 8) & 15) + (((isa >> 25) & 63) << 4) + (((isa >> 31) & 1) << 31) << 1);
		break;
		/* jal opcode == 1101111 */
	case 111:
		rd = (isa >> 7) & REG_BIT;

		/* get sign bit */
		sign_bit = (isa >> 31) & 1;
		/* if negative */
		if (sign_bit == 1) {
			imm = 4095 << 20;
			imm = imm + (((((isa >> 31) & 1) << 31) + ((isa >> 21) & 1023) + (((isa >> 20) & 1) << 10) + (((isa >> 12) & 255) << 11)) << 1);
		}
		else
			imm = ((((isa >> 31) & 1) << 31) + ((isa >> 21) & 1023) + (((isa >> 20) & 1) << 10) + (((isa >> 12) & 255) << 11)) << 1;
		break;
		/* jalr opcode == 1100111 */
	case 103:
		rd = (isa >> 7) & REG_BIT;
		rs1 = (isa >> 15) & REG_BIT;

		/* get sign bit */
		sign_bit = (isa >> 31) & 1;
		/* if negative */
		if (sign_bit == 1) {
			imm = 1048575 << 12;
			imm = ((isa >> 20) & IMM_BIT) << 1;
		}
		else
			imm = ((isa >> 20) & IMM_BIT) << 1;
		break;
	}
}

void exe() {
	/* all */
	switch (op) {		
		/* sd opcode == 0100011 */
	case 35:
		offset = regs[rs1] + imm;
		break;
		/* ld opcode == 0000011 */
	case 3:
		offset = regs[rs1] + imm;
		break;
		/* beq opcode == 1100011 */
	case 99:
		offset = pc + imm - 4;
		if (regs[rs1] == regs[rs2]) {
			pc = offset;
		}
		break;
		/* jal opcode == 1101111 */
	case 111:
		offset = pc + imm - 4;
		pc = offset;
		if (rd != 0)
			/* return address */
			regs[rd] = (long long int)offset - (long long int)imm + 4;
		break;
		/* jalr opcode == 1100111 */
	case 103:
		offset = regs[rs1] + imm;	
		/* return address */
		if(rd != 0)
			regs[rd] = pc;
		pc = offset;
		break;
	}
}

void mem() {
	/* sd */
	switch (op) {
		/* sd opcode == 0100011 */
	case 35:
		data_mem[offset] = regs[rs2];
		break;
	}
}

void wb() {
	/* add, addi, ld */
	switch (op) {
		/* add opcode == 0110011 */
	case 51:
		if (rd != 0)
			regs[rd] = regs[rs1] + regs[rs2];
		break;
		/* addi opcode == 0010011 */
	case 19:
		if (rd != 0)
			regs[rd] = regs[rs1] + imm;
		break;
		/* ld opcode == 0000011 */
	case 3:
		if (rd != 0)
			regs[rd] = data_mem[offset];
		break;
	}
}

void print_cycles()
{
	printf("---------------------------------------------------\n");

	printf("Clock cycles = %lld\n", cycles);
}

void print_pc()
{
	printf("PC	   = %u\n\n", pc);
}

void print_reg()
{
	printf("x0   = %lld\n", regs[0]);
	printf("x1   = %lld\n", regs[1]);
	printf("x2   = %lld\n", regs[2]);
	printf("x3   = %lld\n", regs[3]);
	printf("x4   = %lld\n", regs[4]);
	printf("x5   = %lld\n", regs[5]);
	printf("x6   = %lld\n", regs[6]);
	printf("x7   = %lld\n", regs[7]);
	printf("x8   = %lld\n", regs[8]);
	printf("x9   = %lld\n", regs[9]);
	printf("x10  = %lld\n", regs[10]);
	printf("x11  = %lld\n", regs[11]);
	printf("x12  = %lld\n", regs[12]);
	printf("x13  = %lld\n", regs[13]);
	printf("x14  = %lld\n", regs[14]);
	printf("x15  = %lld\n", regs[15]);
	printf("x16  = %lld\n", regs[16]);
	printf("x17  = %lld\n", regs[17]);
	printf("x18  = %lld\n", regs[18]);
	printf("x19  = %lld\n", regs[19]);
	printf("x20  = %lld\n", regs[20]);
	printf("x21  = %lld\n", regs[21]);
	printf("x22  = %lld\n", regs[22]);
	printf("x23  = %lld\n", regs[23]);
	printf("x24  = %lld\n", regs[24]);
	printf("x25  = %lld\n", regs[25]);
	printf("x26  = %lld\n", regs[26]);
	printf("x27  = %lld\n", regs[27]);
	printf("x28  = %lld\n", regs[28]);
	printf("x29  = %lld\n", regs[29]);
	printf("x30  = %lld\n", regs[30]);
	printf("x31  = %lld\n", regs[31]);
	printf("\n");
}
