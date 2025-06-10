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


// Separando a posição da memória
void separaPosicao(char *entrada, char *posicao) {
    sscanf(entrada, "%[^;]", posicao);
}

// Converte hexadecimal para inteiro
int hexaParaInt(const char *hexa) {
    return (int)strtol(hexa, NULL, 16);
}

// Retorna a próxima posição de separador
int proxInstrucao(const char *entrada, int ini, char dive) {
    char *p = strchr(entrada + ini, dive);
    return p ? (int)(p - entrada) + 1 : strlen(entrada);
}

// Separa a instrução principal
void separaInstrucao(char *entrada, char *instrucao, int ini) {
    int i = 0;
    while (entrada[ini + i] && entrada[ini + i] != '/') {
        instrucao[i] = entrada[ini + i];
        i++;
    }
    if (entrada[ini + i] == '/') instrucao[i++] = '/';
    instrucao[i] = '\0';
}

// Extrai valor em hexadecimal
int extraiValor(char *entrada, int ini) {
    char valorHex[6];
    sscanf(entrada + ini, "%5s", valorHex);
    return hexaParaInt(valorHex);
}

// Obtém o opcode
int getOpcode(const char *op) {
    if (strcmp(op, "hlt") == 0) return hlt;
    if (strcmp(op, "nop") == 0) return nop;
    if (strcmp(op, "ldr") == 0) return ldr;
    if (strcmp(op, "str") == 0) return str;
    if (strcmp(op, "add") == 0) return add;
    if (strcmp(op, "sub") == 0) return sub;
    if (strcmp(op, "mul") == 0) return mul;
    if (strcmp(op, "div") == 0) return div;
    if (strcmp(op, "cmp") == 0) return cmp;
    if (strcmp(op, "movr") == 0) return movr;
    if (strcmp(op, "and") == 0) return and;
    if (strcmp(op, "or") == 0) return or;
    if (strcmp(op, "xor") == 0) return xor;
    if (strcmp(op, "not") == 0) return not;
    if (strcmp(op, "je") == 0) return je;
    if (strcmp(op, "jne") == 0) return jne;
    if (strcmp(op, "jl") == 0) return jl;
    if (strcmp(op, "jle") == 0) return jle;
    if (strcmp(op, "jg") == 0) return jg;
    if (strcmp(op, "jge") == 0) return jge;
    if (strcmp(op, "jmp") == 0) return jmp;
    if (strcmp(op, "ld") == 0) return ld;
    if (strcmp(op, "st") == 0) return st;
    if (strcmp(op, "movi") == 0) return movi;
    if (strcmp(op, "addi") == 0) return addi;
    if (strcmp(op, "subi") == 0) return subi;
    if (strcmp(op, "muli") == 0) return muli;
    if (strcmp(op, "divi") == 0) return divi;
    if (strcmp(op, "lsh") == 0) return lsh;
    if (strcmp(op, "rsh") == 0) return rsh;
    return -1;
}

// Codifica e armazena instruções na memória
void guardaInstrucao(const char *instrucao, int pos, char *mem) {
    char op[10], arg1[10], arg2[10] = "";
    int opcode = -1, reg1 = 0, reg2 = 0, valor_imm = 0;
    unsigned short cod = 0;

    sscanf(instrucao, "%s %[^,], %s", op, arg1, arg2);
    opcode = getOpcode(op);
    if (opcode == -1) return;

    if (arg1[0] == 'r' || arg1[0] == 'R') reg1 = atoi(arg1 + 1);

    if (arg2[0] == 'r' || arg2[0] == 'R') {
        reg2 = atoi(arg2 + 1);
        cod = (opcode << 11) | (reg1 << 9) | (reg2 << 7);
        mem[pos] = (cod >> 8) & 0xFF;
        mem[pos + 1] = cod & 0xFF;
    } else if (strlen(arg2) > 0) {
        valor_imm = (int)strtol(arg2, NULL, 16);
        cod = (opcode << 11) | (reg1 << 9);
        mem[pos] = (cod >> 8) & 0xFF;
        mem[pos + 1] = 0x00;
        mem[pos + 2] = valor_imm & 0xFF;
    }
}

// Decodifica linha de texto e armazena na memória
void decodificaStringEGuardaNaMemoria(char *entrada, unsigned char *memoria) {
    char posicao[6], instrucao[50];
    int ini = 0;

    memset(posicao, '\0', sizeof(posicao));
    memset(instrucao, '\0', sizeof(instrucao));

    separaPosicao(entrada, posicao);
    int pos = hexaParaInt(posicao);
    ini = proxInstrucao(entrada, ini, ';');

    // Pega o tipo direto
    char tipo = entrada[ini];
    ini = proxInstrucao(entrada, ini, ';');

    if (tipo == 'i') {
        separaInstrucao(entrada, instrucao, ini);
        if (pos >= 154 - 1) return;
        guardaInstrucao(instrucao, pos, (char *)memoria);
    } else if (tipo == 'd') {
        int valor = extraiValor(entrada, ini);
        if (pos >= 154 - 1) return;
        memoria[pos] = (valor >> 8) & 0xFF;
        memoria[pos] = valor & 0xFF;
    }
}

// Carrega arquivo na memória
void carregar_memoria(const char* nome_arquivo) {
    printf("Carregando arquivo...\n");
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }

    char linha[154];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        decodificaStringEGuardaNaMemoria(linha, memoria);
    }

    fclose(arquivo);
}

int main()
{
    // Carregando a memória
    carregar_memoria("entrada.txt");

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
                reg[ro0] = ro1;
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
                printf("MBR: %08X\n", MBR);
                MBR = reg[ro0];
                // printf("MBR RECEBENDO O VALOR DO ACUMULADOR: %x\n", MBR);
                memoria[MAR] = MBR;
                // printf("INDICE DA MEMORIA: %x\n", MAR);
                // printf("MEMORIA COM O VALOR ARMAZENADO: %x\n", memoria[MAR]);
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
