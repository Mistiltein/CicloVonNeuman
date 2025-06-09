#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Instrucao traduzida para o Opcode
#define hlt   0b00000
#define nop   0b00001
#define ldr   0b00010
#define str   0b00011
#define add   0b00100
#define sub   0b00101
#define mul   0b00110
#define div   0b00111
#define cmp   0b01000
#define movr  0b01001
#define and   0b01010
#define or    0b01011
#define xor   0b01100
#define not   0b01101
#define je    0b01110
#define jne   0b01111
#define jl    0b10000
#define jle   0b10001
#define jg    0b10010
#define jge   0b10011
#define jmp   0b10100
#define ld    0b10101
#define st    0b10110
#define movi  0b10111
#define addi  0b11000
#define subi  0b11001
#define muli  0b11010
#define divi  0b11011
#define lsh   0b11100
#define rsh   0b11101

int main()
{
    // Registradores
    unsigned char memoria[154] = {0};
    unsigned int MBR = 0;
    unsigned short int MAR = 0;
    unsigned char IR = 0;
    unsigned char ro0 = 0;
    unsigned char ro1 = 0;
    unsigned short int IMM = 0;
    unsigned short int PC = 0;
    unsigned char E = 0;
    unsigned char L = 0;
    unsigned char G = 0;
    unsigned short int reg[4] = {0};

    memoria[0] = 0xa8;
    memoria[1] = 0x00;
    memoria[2] = 0x96;

    memoria[3] = 0xaa;
    memoria[4] = 0x00;
    memoria[5] = 0x98;

    memoria[6] = 0x28;
    memoria[7] = 0x80;

    memoria[8] = 0xaa;
    memoria[9] = 0x00;
    memoria[10] = 0x94;

    memoria[11] = 0x3a;
    memoria[12] = 0x00;
    memoria[13] = 0xac;
    memoria[14] = 0x00;
    memoria[15] = 0x92;
    memoria[16] = 0x34;
    memoria[17] = 0x80;
    memoria[18] = 0xaa;
    memoria[19] = 0x00;
    memoria[20] = 0x90;
    memoria[21] = 0x23;
    memoria[22] = 0x00;
    memoria[23] = 0xb2;
    memoria[24] = 0x00;
    memoria[25] = 0x8e;
    memoria[144] = 0x20;
    memoria[145] = 0x00;
    memoria[146] = 0x03;
    memoria[147] = 0x00;
    memoria[148] = 0x04;
    memoria[149] = 0x00;
    memoria[150] = 0x05;
    memoria[151] = 0x00;
    memoria[152] = 0x03;
    memoria[153] = 0x00;

    while (PC <= 154) {
        // Busca
        MAR = PC;

        MBR = memoria[MAR] << 8;
        MAR++;
        //printf("MBR APOS PRIMEIRO DESLOCAMENTO: %08x\n", MBR);
        MBR = (MBR | memoria[MAR]) << 8;
        MAR++;
        //printf("MBR APOS SEGUNDO DESLOCAMENTO: %08x\n", MBR);
        MBR = (MBR | memoria[MAR]);
        //printf("MBR APOS TERCEIRO DESLOCAMENTO: %08x\n", MBR);

        // Decodificacao
        IR = MBR >> 19;
        printf("MBR: %X\n", MBR);
        printf("IR: %X\n", IR);

        if (IR == hlt || IR == nop) {
            ro0 = MBR >> 16;
            printf("ro0: %x\n", ro0);
        }

        if (IR == ldr || IR == str || IR == add || IR == sub || IR == mul || IR == div || IR == cmp || IR == movr || IR == and || IR == or || IR == xor) {
            ro0 = (MBR & 0x060000) >> 17; // 0000 0110 0000 0000 0000 0000
            ro1 = (MBR & 0x018000) >> 15; // 0000 0001 1000 0000 0000 0000
            // printf("ro0: %X\n", ro0);
            // printf("ro1: %X\n", ro1);
        }

        if (IR == not) {
            ro0 = (MBR & 0x060000) >> 17; // 0000 0110 0000 0000 0000 0000
            // printf("ro0: %X\n", ro0);
            // printf("ro1: %X\n", ro1);
        }

        if (IR == je || IR == jne || IR == jl || IR == jle || IR == jg || IR == jge || IR == jmp) {
            MAR = MBR >> 16;
            // printf("MAR: %X\n", MAR);
        }

        if (IR == ld || IR == st || IR == movi || IR == addi || IR == subi || IR == muli || IR == divi || IR == lsh || IR == rsh) {
            ro0 = (MBR & 0x060000) >> 17; // 0000 0110 0000 0000 0000 0000
            MAR = (MBR & 0x0000ff);
            // printf("ro0: %X\n", ro0);
            // printf("MAR: %X\n", MAR);
        }

        // Execucao
        switch (IR) {
            case hlt :
                printf("HALT. Execucao Finalizada");
                break;

            case nop :
                PC = PC + 1;
                break;

            case ldr :
                reg[ro0] = reg[ro1];
                PC = PC + 2;
                break;

            case str :
                reg[ro1] = reg[ro0];
                PC = PC + 2;
                break;

            case add :
                reg[ro0] = reg[ro0] + reg[ro1];
                PC = PC + 2;
                break;

            case sub :
                reg[ro0] = reg[ro0] - reg[ro1];
                PC = PC + 2;
                break;

            case mul :
                reg[ro0] = reg[ro0] * reg[ro1];
                PC = PC + 2;
                break;

            case div :
                reg[ro0] = reg[ro0] / reg[ro1];
                PC = PC + 2;
                break;

            case cmp :
                if (reg[ro0] == reg[ro1]) {
                    E = 1;
                } else {
                    E = 0;
                }

                if (reg[ro0] < reg[ro1]) {
                    L = 1;
                } else {
                    L = 0;
                }

                if (reg[ro0] > reg[ro1]) {
                    G = 1;
                } else {
                    G = 0;
                }
                PC = PC + 2;
                break;

            case movr :
                reg[ro0] = reg[ro1];
                PC = PC + 2;
                break;

            case and :
                reg[ro0] = reg[ro0] & reg[ro1];
                PC = PC + 2;
                break;

            case or :
                reg[ro0] = reg[ro0] | reg[ro1];
                PC = PC + 2;
                break;

            case xor :
                reg[ro0] = reg[ro0] ^ reg[ro1];
                PC = PC + 2;
                break;

            case not :
                reg[ro0] = !reg[ro0];
                PC = PC + 1;
                break;

            case je :
                if (E == 1) {
                    PC = MAR;
                } else {
                    PC = PC + 3;
                }
                break;

            case jne :
                if (E == 0) {
                    PC = MAR;
                } else {
                    PC = PC + 3;
                }
                break;

            case jl :
                if (L == 1) {
                    PC = MAR;
                } else {
                    PC = PC + 3;
                }
                break;

            case jle :
                if (L == 1 || E == 1) {
                    PC = MAR;
                } else {
                    PC = PC + 3;
                }
                break;

            case jg :
                if (G == 1) {
                    PC = MAR;
                } else {
                    PC = PC + 3;
                }
                break;

            case jge :
                if (G == 1 || E == 1){
                    PC = MAR;
                } else {
                    PC = PC + 3;
                }
                break;

            case jmp :
                PC = MAR;
                break;

            case ld :
                reg[ro0] = memoria[MAR];
                PC = PC + 3;
                break;

            case st :
                memoria[MAR] = reg[ro0] >> 8;
                PC = PC + 3;
                break;

            case movi :
                reg[ro0] = IMM;
                PC = PC + 3;
                break;

            case addi :
                reg[ro0] = reg[ro0] + IMM;
                PC = PC + 3;
                break;

            case subi :
                reg[ro0] = reg[ro0] - IMM;
                PC = PC + 3;
                break;

            case muli :
                reg[ro0] = reg[ro0] * IMM;
                PC = PC + 3;
                break;

            case divi :
                reg[ro0] = reg[ro0] / IMM;
                PC = PC + 3;
                break;

            case lsh :
                reg[ro0] = reg[ro0] << IMM;
                PC = PC + 3;
                break;

            case rsh :
                reg[ro0] = reg[ro0] >> IMM;
                PC = PC + 3;
                break;
        }

        // Dados na tela
        printf("CPU:\n MBR: %08X\n", MBR);
        printf("MAR: 0x%04x\n", MAR);
        printf("IR: 0x%02X\n", IR);
        printf("ro0: 0x%01X\n", ro0);
        printf("ro1: 0x%01X\n", ro1);
        printf("IMM: 0x%04X\n", IMM);
        printf("PC: 0x%04X\n", PC);
        printf("E: 0x%X\n", E);
        printf("L: 0x%X\n", L);
        printf("G: 0x%X\n", G);
        for (int regPrint = 0; regPrint < 4; regPrint++) {
            printf("reg%d:\t\t0x%04x\n", regPrint, reg[regPrint]);
        }

        for (int impressao = 0; impressao < 154; impressao++) {
            printf("%3d: 0x%02x\t", impressao, memoria[impressao]);

            if ((impressao + 1) % 10 == 0) {
                printf("\n");
            }
        }

        printf("\nPressione ENTER para continuar a execucao do programa.\n");
       while (getchar() != '\n');
    }

    return 0;
}
