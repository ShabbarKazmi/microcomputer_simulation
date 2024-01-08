/**************************************************
**Description : Program that simulates a mircroputer by reading in an encoded binary file representing
**instructions, and decoding and executing those instructions
**Author : Teagen Protheroe, Shabbar Kazmi
** 3/1/22
***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned int FILE_SIZE_ALLOCATION = 32;
const unsigned int INSTRUCTION_SIZE = 2;
const unsigned int FIVE_BITS = 5;
const unsigned int ONE_BIT = 1;
const unsigned int REMOVE_UPPER_FIVE_BITS = 7;
const unsigned int REMOVE_UPPER_FOUR_BITS = 15;
const unsigned int REMOVE_UPPER_EIGHT_BITS = 255;
const unsigned int REMOVE_LAST_BIT = 127;
const unsigned int REMOVE_UPPER_THREE_BITS = 31;
const unsigned int SEVEN_BITS = 7;

#define LDI 0
#define ADD 1
#define AND 2
#define OR 3
#define XOR 4
#define PRT 5
#define RDD 6
#define BLE 7

unsigned int num_bytes;
unsigned int PC;
unsigned char *buffer;
unsigned char *IR;
unsigned char op = -1;
unsigned char ri = 0;
unsigned char rj = 0;
unsigned char rk = 0;
unsigned short imm = 0;
unsigned char addr = 0;
unsigned char reg[16];
FILE *outFile;
FILE *inFile;

/*
 * Function:  write_to_file
 * ------------------------
 * Builds an output string consisting of the necessary parts
 * for each instruction and writes it to an output file
 *
 */
void write_to_file()
{
    // Storage for each decoded instruction
    char instructionOut[32];
    switch ((op))
    {
    case LDI:
        sprintf(instructionOut, "%d: LDI R%x %d\n", PC, ri, imm);
        break;
    case ADD:
        sprintf(instructionOut, "%d: ADD R%x R%x R%x\n", PC, ri, rj, rk);
        break;
    case AND:
        sprintf(instructionOut, "%d: AND R%x R%x R%x\n", PC, ri, rj, rk);
        break;
    case OR:
        sprintf(instructionOut, "%d: OR R%x R%x R%x\n", PC, ri, rj, rk);
        break;
    case XOR:
        sprintf(instructionOut, "%d: XOR R%x R%x R%x\n", PC, ri, rj, rk);
        break;
    case PRT:
        sprintf(instructionOut, "%d: PRT R%x\n", PC, ri);
        break;
    case RDD:
        sprintf(instructionOut, "%d: RDD R%x\n", PC, ri);
        break;
    case BLE:
        sprintf(instructionOut, "%d: BLE R%x R%x %d\n", PC, ri, rj, addr);
        break;
    default:
        printf("Error: Invalid Instruction\n");
        break;
    }
    fputs(instructionOut, outFile); // Write to output file
}

/*
 * Function:  decode_instruction
 * -----------------------------
 * Takes an encoded instruction as input and outputs all the instructions parts
 *
 * instruction: encoded instruction
 *
 * op: instruction opcode
 * ri: instruction register
 * rj: instruction register
 * rk: instruction register
 * imm: instruction immediate data
 * addr: instruction address to jump to
 *
 */
void decode_instruction(char instruction[], char *op,
                        char *ri, char *rj, char *rk, short *imm, char *addr)
{
    *op = (instruction[0] >> FIVE_BITS) & REMOVE_UPPER_FIVE_BITS; // Get first three bits of instruction
    *ri = (instruction[0] >> ONE_BIT) & REMOVE_UPPER_FOUR_BITS;   // Gets next four bits of instruction
                                                                  // for first register
    switch ((*op))
    {
    case LDI:
        *imm = ((unsigned char)(instruction[0] & 1) << SEVEN_BITS) // Gets next eight bits of instruction
               + ((unsigned char)instruction[1] >> ONE_BIT);
        break;
    case ADD:
    case AND:
    case OR:
    case XOR:
        *rj = (instruction[0] & 1) + (instruction[1] >> FIVE_BITS)  // Gets next fours bits of instruction
              & REMOVE_UPPER_FIVE_BITS;                             // for register j
        *rk = (instruction[1] >> ONE_BIT) & REMOVE_UPPER_FOUR_BITS; // Gets the next fours bits of instuction
        break;                                                      // for register k

    case PRT: // Prints out value of rk
    case RDD: // Takes in lower 8 8its of rk

        break;

    case BLE:                                                      // Branches if ri is lesser than rj
        *rj = (instruction[0] & 1) + (instruction[1] >> FIVE_BITS) // Gets next fours bits of instruction
              & REMOVE_UPPER_FIVE_BITS;                            // For register j
        *addr = instruction[1] & REMOVE_UPPER_THREE_BITS;          // Jumps to specified address
        break;
    default:
        printf("Error: Invalid Instruction\n");
    }
}

/*
 * Function:  execute_program
 * --------------------------
 * Take in every encoded instruction, decode each one, and execute each one while
 * simulating a microputer
 *
 * program: array containing every encoded instruction
 * num_instructions: number of instructions to execute
 *
 */
int execute_program(char program[], int num_instructions)
{
    int inputVal; // for storing user input

    for (int PC = 0; PC < num_instructions * 2; PC += 2)
    {
        IR[0] = program[PC];     // Get lower 8 bits
        IR[1] = program[PC + 1]; // Gets upper 8 bits

        decode_instruction(IR, &op, &ri, &rj, &rk, &imm, &addr);
        switch (op)
        {

        case LDI:
            reg[ri] = imm;
            break;
        case ADD:

            reg[rk] = reg[ri] + reg[rj];
            break;
        case AND:

            reg[rk] = reg[ri] & reg[rj];
            break;
        case OR:

            reg[rk] = reg[ri] | reg[rj];
            break;
        case XOR:

            reg[rk] = reg[ri] ^ reg[rj];
            break;

        case PRT:

            printf("%d", reg[ri]);
            break;

        case RDD:
            scanf("%d", &inputVal);
            reg[ri] = inputVal & REMOVE_UPPER_EIGHT_BITS;
            break;

        case BLE:
            if (reg[ri] <= reg[rj])
            {
                PC = addr - 2;
            }
            break;
        default:
            printf("Error: Invalid Instruction\n");
            return -1;
        }
    }
    return 0;
}

/*
 * Function:  read_file
 * --------------------
 * Reads in a binary file containing encoded instructions
 *
 * input: name of input file specifed by user
 * output: name of output file specified by user
 *
 */
int read_file(char input[], char output[])
{

    buffer = (char *)malloc(FILE_SIZE_ALLOCATION);
    inFile = fopen(input, "rb");
    if (!inFile)
    {
        printf("Unable to open file!\n");
        return -1;
    }

    num_bytes = fread(buffer, INSTRUCTION_SIZE, FILE_SIZE_ALLOCATION, inFile);
    fclose(inFile); // might need to change position to for cleanliness sake
    outFile = fopen(output, "w");
    for (int i = 0; i < num_bytes * 2; i += 2)
    {
        PC = i;
        IR[0] = buffer[i];     // Get lower bits
        IR[1] = buffer[i + 1]; // Get upper bit
        decode_instruction(IR, &op, &ri, &rj, &rk, &imm, &addr);
        write_to_file();
    }
    fclose(outFile);

    return 0;
}

/*
 * Function:  main
 * ---------------
 * Get user provided input and output file from command line, and execute program
 *
 * argc: number of arguements
 * argv: array containing arguements
 *
 */
int main(int argc, char *argv[])
{
    IR = (char *)malloc(INSTRUCTION_SIZE);
    read_file(argv[1], argv[2]);
    execute_program(buffer, num_bytes);
}