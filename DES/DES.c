/*
Date: Second semester of 2021.
Algorithm: Data Encryption Standard (DES)
https://csrc.nist.gov/csrc/media/publications/fips/46/3/archive/1999-10-25/documents/fips46-3.pdf
This c file is initiated with command line arguments. After compiling, for example, type ./DES -file to be read (and encrypted or decrypted) by the program- -mode, which can be "-c" for cyphering, or "-d" for decoding-
-key, that must have 8 characters- -name of the file that will contain the generated output-
Examples:
For encryption:
./DES in.txt -c abcdefgh out.txt
For decryption (of the result generated previously):
./DES out.txt -d abcdefgh outd.txt
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void ascii_para_bin(unsigned const char *, char *);
void permutacoes(const int *, const char *, char*, int);
void chaves(const char *, const char *, const char *, char[16][29], char [16][29], char [16][57], char [16][49]); 
char XOR(const char, const char);
void Processamento_S( char *, const char *, int);
int bin_para_dec(int);
int dec_para_bin(int);
int char_para_int(const char);

const int Initial_Permutation[64] = { 
                                      58, 50, 42, 34, 26, 18, 10, 2,
                                      60, 52, 44, 36, 28, 20, 12, 4,
                                      62, 54, 46, 38, 30, 22, 14, 6,
                                      64, 56, 48, 40, 32, 24, 16, 8,
                                      57, 49, 41, 33, 25, 17, 9,  1,
                                      59, 51, 43, 35, 27, 19, 11, 3,
                                      61, 53, 45, 37, 29, 21, 13, 5,
                                      63, 55, 47, 39,31, 23, 15,  7
                                                                    };

const int final_permutation[64] = {
                                    40, 8, 48, 16, 56, 24, 64, 32,
                                    39, 7, 47, 15, 55, 23, 63, 31,
                                    38, 6, 46, 14, 54, 22, 62, 30,
                                    37, 5, 45, 13, 53, 21, 61, 29,
                                    36, 4, 44, 12, 52, 20, 60, 28,
                                    35, 3, 43, 11, 51, 19, 59, 27,
                                    34, 2, 42, 10, 50, 18, 58, 26,
                                    33, 1, 41,  9, 49, 17, 57, 25
                                                                  };

const int E_bit_selection[48] = {
                                  32, 1, 2, 3, 4, 5,
                                  4, 5, 6, 7, 8, 9,
                                  8, 9, 10, 11, 12, 13,
                                  12, 13, 14, 15, 16, 17,
                                  16, 17, 18, 19, 20, 21,
                                  20, 21, 22, 23, 24, 25,
                                  24, 25, 26, 27, 28, 29,
                                  28, 29, 30, 31, 32, 1
                                                          };

const int P[32] = {
                    16, 7, 20, 21,
                    29, 12, 28, 17,
                    1, 15, 23, 26,
                    5, 18, 31, 10,
                    2, 8, 24, 14,
                    32, 27, 3, 9,
                    19, 13, 30, 6,
                    22, 11, 4, 25
                                  };

const int S_box[8][4][16] = {
                      //S1
                      {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
                       {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
                       {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
                       {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},

                      //S2
                      {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
                       {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
                       {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
                       {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
                      
                      //S3
                      {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
                       {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
                       {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
                       {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},

                      //S4
                      {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
                       {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
                       {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
                       {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},

                      //S5
                      {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
                       {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
                       {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
                       {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},

                      //S6
                      {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
                       {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
                       {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
                       {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},

                      //S7
                      {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
                       {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
                       {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
                       {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
                       
                      //S8
                      {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
                       {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
                       {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
                       {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}
                      }; 

const int permuted_choice_1[56] = {
                                    57, 49, 41, 33, 25, 17, 9,
                                    1, 58, 50, 42, 34, 26, 18,
                                    10, 2, 59, 51, 43, 35, 27,
                                    19, 11, 3, 60, 52, 44, 36,
                                    63, 55, 47, 39, 31, 23, 15,
                                    7, 62, 54, 46, 38, 30, 22,
                                    14, 6, 61, 53, 45, 37, 29,
                                    21, 13, 5, 28, 20, 12, 4
                                                              };

const int left_shifts_table[28] = {
                                    2, 3, 4, 5, 6, 7, 8,
                                    9, 10, 11, 12, 13, 14, 15,
                                    16, 17, 18, 19, 20, 21, 22,
                                    23, 24, 25, 26, 27, 28, 1
                                                              };

const int permuted_choice_2[48] = {  
                                    14, 17, 11, 24, 1, 5,
                                    3, 28, 15, 6, 21, 10,
                                    23, 19, 12, 4, 26, 8,
                                    16, 7, 27, 20, 13, 2,
                                    41, 52, 31, 37, 47, 55,
                                    30, 40, 51, 45, 33, 48,
                                    44, 49, 39, 56, 34, 53,
                                    46, 42, 50, 36, 29, 32
                                                            };
                                                            

int main(int argc, char * argv[]) {

  FILE *in, *out;
  char chave_ascii[9], texto_plano_bin[65], chave_bin[65], pc_1_chave[57], bloco_c[29], bloco_d[29], blocos_c_permutados[16][29], blocos_d_permutados[16][29], CD[16][57], chaves_finais[16][49], texto_IP[65], esquerda[33], direita[33], direito_expandido[49], xor_chave[49], blocos_B[8][7], output_S[5], output_S_total[33], S_permutado[33], aux_dir[33], total[65], total_permutado[65], total_permutado_bytes[9];
  unsigned char texto_plano_ascii[9];
  static unsigned char texto_arquivo[500000];
  int S, len, bin_rep;

  //VALIDAÇÃO DA INICIALIZAÇÃO E LEITURA DO ARQUIVO INICIAL
  if (argc != 5) {
    printf("Erro de sintaxe!\nDigite: %s <arquivo-entrada> <-c|-d> <chave de 8 caracteres> <arquivo-saida>\n", argv[0]);
    return 1;
  }
  if(strcmp(argv[2], "-c") != 0 &&  strcmp(argv[2], "-d") != 0){
    printf("Erro no modo de criptografia escolhido.\nDigite: %s <arquivo-entrada> <-c|-d> <chave de 8 caracteres> <arquivo-saida>\n", argv[0]);
    return 2;
  }
  if(argv[3][8] != '\0'){
    printf("A chave nao possui 64 bits (8 caracteres)\n");
    return 3;
  }
  if ((in = fopen(argv[1], "rb")) == NULL) {
    printf("Erro: arquivo nao encontrado (%s)!\n", argv[1]);
    return 4;
  }
  if ((out = fopen(argv[4], "wb")) == NULL) {
    printf("Erro: nao foi possivel criar o arquivo \"%s\"!\n", argv[4]);
    fclose(in);
    return 5;
  }
  len = 0;
  strcpy(chave_ascii, argv[3]);
  unsigned char b;
  while (1) {
    b = fgetc(in);
    //printf("%d\n", b);
    if (feof(in)) {
      break;
    }
    texto_arquivo[len] = b;
    len++;
  }
  fclose(in);
  texto_arquivo[len] = '\0';
  //printf("Len antes da expansao: %d\n", len);

  //CONVERSÃO DA CHAVE EM ASCII PARA UMA STRING COM SEUS REPRESENTANTES EM BINÁRIO
  printf("Texto de entrada completo:\n%s\n", texto_arquivo);
  ascii_para_bin((unsigned char*)chave_ascii, chave_bin);
  printf("\nChave digitada em binario:\n%s\n", chave_bin);
  printf("modo: %s\n", argv[2]);

  //LOOP PARA EXPANDIR O ÚLTIMO BLOCO COM ESPAÇOS EM BRANCO, CASO O NÚMERO DE CARACTERES NÃO SEJA DIVISÍVEL POR OITO
  while(len % 8 != 0){
    texto_arquivo[len] = ' ';
    len++;
  }
  texto_arquivo[len] = '\0';
  //printf("Len apos a expansao: %d\n", len);

  //PERMUTAÇÃO PC-1 NOS BITS DA CHAVE
  permutacoes(permuted_choice_1, chave_bin, pc_1_chave, 57);
  //printf("\nPC_1: %s\n", pc_1_chave);

  //DIVISÃO DA CHAVE PERMUTADA EM BLOCO C E D
  for (int i = 0; i < 28; i++){
    bloco_c[i]= pc_1_chave[i];
    bloco_d[i]= pc_1_chave[i+28];
  }
  bloco_c[28]='\0';
  bloco_d[28]='\0';
  //printf("Bloco C: %s\nBloco D: %s\n", bloco_c, bloco_d);

  //GERAÇÃO DAS 16 "SUBCHAVES" K
  chaves(bloco_c, bloco_d, argv[2], blocos_c_permutados, blocos_d_permutados, CD, chaves_finais);
  
  //LOOP PARA QUE TODA A INFORMAÇÃO DO ARQUIVO INICIAL SEJA CRIPTOGRAFADA
  for(int x = 0; x < len/8; x++){
    //printf("LEN: %d\n", len);
    for(int y = 0; y < 8; y++){
      texto_plano_ascii[y] = texto_arquivo[x*8 + y];
      //printf("%c\n", texto_plano_ascii[y]);
    }
    texto_plano_ascii[8] = '\0';
    printf("\nparte %d:\n%s\n", x + 1, texto_plano_ascii);

    //CRIAÇÃO DA N-ÉSIMA STRING COM OS RESPECTIVOS BYTES DO ARQUIVO INICIAL, EM BINÁRIO
    ascii_para_bin(texto_plano_ascii, texto_plano_bin);
    texto_plano_bin[64] = '\0';
    printf("\nTexto de entrada em binario:\n%s\n", texto_plano_bin);
  
    //PERMUTAÇÃO INICIAL DO TEXTO PLANO E DIVISÃO EM DOIS BLOCOS
    permutacoes(Initial_Permutation, texto_plano_bin, texto_IP, 65);
    //printf("\nBloco de texto em binário após IP:\n%s\n", texto_IP);
    for (int i = 0; i < 32; i++){
      esquerda[i]= texto_IP[i];
      direita[i]= texto_IP[i+32];
    }
    esquerda[32] = '\0';
    direita[32] = '\0';
    //printf("\nLADO ESQUERDO ANTES DOS ROUNDS:%s\nLADO DIREITO ANTES DOS ROUNDS:%s\n", esquerda, direita); 

    //RODADAS DA CRIPTOGRAFIA GERAL
    for(int i = 0; i < 16; i++){
      S = 0;
      //printf("\nESQUERDA inicio %d: %s\nDIREITA inicio %d: %s\n", i + 1, esquerda, i + 1, direita);

      //FUNÇÃO F 
      permutacoes(E_bit_selection, direita, direito_expandido, 49);
      output_S[0] = '\0';
      output_S_total[0] = '\0';
      S_permutado[0] = '\0';
      //printf("\nDireito expandido %d: %s\n", i + 1, direito_expandido);

      //XOR ENTRE A SUBCHAVE DO ROUND E O LADO DIREITO EXPANDIDO DO ROUND
      for(int j = 0; j < 48; j++){
        xor_chave[j] = XOR(direito_expandido[j], chaves_finais[i][j]);
      }
      xor_chave[48] = '\0';
      //printf("\nChave %d:\n%s\nF:\n%s\nXOR entre chave e F:\n%s\n", i + 1, chaves_finais[i], direito_expandido, xor_chave);

      //DIVISÃO DO CONTEÚDO EM BLOCOS MENORES PARA A ENTRADA NO PROCESSAMENTO S
      for(int k = 0; k < 8; k++){
        for(int l = 0; l < 6; l++){
          blocos_B[k][l] = xor_chave[l + k*6];
        }
        blocos_B[k][6] = '\0';
        //printf("\n%s\n", blocos_B[k]);
        Processamento_S(output_S, blocos_B[k], S);
        strcat(output_S_total,output_S);
        S++;
      }
      output_S_total[32] = '\0';
      //printf("\nOUTPUT S: %s\n", output_S_total);

      //PERMUTANDO A SAÍDA DO PROCESSAMENTO S
      permutacoes(P, output_S_total, S_permutado, 33);
      //printf("\nOUTPUT S PERMUTADO: %s\n", S_permutado);

      //XOR ENTRE LADO DIREITO MODIFICADO E LADO ESQUERDO ATUAL E TROCA DOS LADOS PARA A PRÓXIMA ITERAÇÃO
      strcpy(aux_dir, direita);
      for(int j = 0; j < 32; j++){
        direita[j] = XOR(S_permutado[j], esquerda[j]);
      }
      strcpy(esquerda, aux_dir);
      //printf("\nESQUERDA fim %d: %s\nDIREITA fim %d: %s\n", i + 1, esquerda, i + 1, direita);
    }

    //CONCATENANDO AS DUAS METADES E FINALIZANDO A CRIPTOGRAFIA
    for(int i = 0; i < 32; i++){
      total[i] = direita[i];
      total[i + 32] = esquerda[i];
    }
    total[64] = '\0';
    //printf("\n%s\n", total);
    permutacoes(final_permutation, total, total_permutado, 65);
    printf("\nTexto final em binario:\n%s\n", total_permutado);

    //COLOCANDO O RESULTADO NO ARQUIVO FINAL
    for(int i = 0; i < 8; i++){
      for(int j = 0; j < 8; j++){
        total_permutado_bytes[j] = total_permutado[i*8 + j];
      }
      total_permutado_bytes[8] = '\0';
      bin_rep = atoi(total_permutado_bytes);
      bin_rep = bin_para_dec(bin_rep);
      //printf("colocado: %d\n", bin_rep);
      fputc((bin_rep), out);
    }
  }
  fclose(out);

  return 0;
}

//FUNÇÃO PARA CONVERTER AS ENTRADAS PARA BINÁRIO
void ascii_para_bin(unsigned const char * cod_ascii, char * cod_bin){
  int pos, bin;
  pos = 0;
  char bin8[9];
  //printf("codigo ascci: %s\n", cod_ascii);
  cod_bin[0] = '\0';
  while(pos < 8){
    //printf("int do cod ascii: %d\n", (int)cod_ascii[pos]);
    bin = dec_para_bin((int)(cod_ascii[pos]));
    //printf("%d\n", bin);
    sprintf(bin8, "%08d", bin);
    //printf("%s\n", bin8);
    strcat(cod_bin, bin8);
    pos++;
  }
  cod_bin[64] = '\0';
  //printf("%s\n", cod_bin);
}

//FUNÇÃO GERAL QUE REALIZA AS PERMUTAÇÕES DO DES
void permutacoes(const int * tabela, const char * bloco_anterior, char* bloco_final, int tamanho){
  bloco_final[0] = '\0';
  for(int i = 0; i < tamanho - 1; i++){
    bloco_final[i] = bloco_anterior[tabela[i] - 1];
  }
  bloco_final[tamanho - 1] = '\0';
}

//GERAÇÃO DAS CHAVES (KEYSCHEDULE)
void chaves(const char * c, const char * d, const char * argv, char blocos_c[16][29], char blocos_d[16][29], char blocos_CD[16][57], char chaves_permutadas[16][49]){
  char aux_blocos[29];
  
  //DESLOCAMENTO DE BITS À ESQUERDA PARA A CRIAÇÃO DOS 16 BLOCOS C E D
  permutacoes(left_shifts_table, c, blocos_c[0], 29);
  permutacoes(left_shifts_table, d, blocos_d[0], 29);
  //printf("\n\nBLOCOS C E D:\n%s\n%s\n", blocos_c[0], blocos_d[0]);
  for(int i = 1; i < 16; i++){
    if(i + 1 == 2 || i + 1 == 9 || i + 1 == 16){
      permutacoes(left_shifts_table, blocos_c[i - 1], blocos_c[i], 29);
      permutacoes(left_shifts_table, blocos_d[i - 1], blocos_d[i], 29);
    }else{
      permutacoes(left_shifts_table, blocos_c[i - 1], aux_blocos, 29);
      permutacoes(left_shifts_table, aux_blocos, blocos_c[i], 29);

      permutacoes(left_shifts_table, blocos_d[i - 1], aux_blocos, 29);
      permutacoes(left_shifts_table, aux_blocos, blocos_d[i], 29);
    }
    //printf("\n%s\n%s\n", blocos_c[i], blocos_d[i]);
  }

  //CONCATENANDO OS BLOCOS C E D
  //printf("\nBlocos concatenados: ");
  for(int i = 0; i < 16; i++){
    for(int j = 0; j < 56; j++){
      if(j < 28){
        blocos_CD[i][j] = blocos_c[i][j];
      }else{
        blocos_CD[i][j] = blocos_d[i][j - 28];
      }
      blocos_CD[i][56] = '\0';
    }
    //printf("\n\n%s\n", blocos_CD[i]); 
  }

  //APLICANDO A PC-2 E FINALIZANDO A CRIAÇÃO DAS 16 SUBCHAVES
  //printf("\n\nCHAVES FINAIS:\n");
  for(int i = 0; i < 16; i++){
    if(strcmp(argv, "-c") == 0){
      permutacoes(permuted_choice_2, blocos_CD[i], chaves_permutadas[i], 49);
    }
    if(strcmp(argv, "-d") == 0){
      permutacoes(permuted_choice_2, blocos_CD[i], chaves_permutadas[15 - i], 49);
    }
    //printf("%s\n", chaves_permutadas[i]);
  }
}

//FUNÇÃO XOR
char XOR(const char a, const char b){

  if((b == '1' || a == '1') && (b != a)){
    return '1';
  }
  return '0';
}

//FUNÇÃO PARA O PROCESSAMENTO S QUE OCORRE NA FUNÇÃO F DO DES
void Processamento_S(char * output, const char *bloco, int dim){

  int linhas, col, resultado;
  linhas = char_para_int(bloco[0])*10 + char_para_int(bloco[5]);
  linhas = bin_para_dec(linhas);

  col = char_para_int(bloco[1])*1000 + char_para_int(bloco[2])*100 + char_para_int(bloco[3])*10 + char_para_int(bloco[4]);
  col = bin_para_dec(col);
  //printf("%d\n", linhas);
  //printf("%d\n", col);
  resultado = S_box[dim][linhas][col];
  int bin = (dec_para_bin(resultado));
  //printf("binario %d", bin);
  sprintf(output, "%04d", bin);
  //printf("\n%s\n", output);
}

//FUNÇÃO QUE CONVERTE BINÁRIO PARA DECIMAL
int bin_para_dec(int binario){
  int dec, pot2;
  pot2 = 1, dec = 0;

  while(binario > 0){
    dec += (binario % 10)*pot2;
    binario /= 10;
    pot2 *= 2;
  }
  return dec;
}

//FUNÇÃO QUE CONVERTE DECIMAL PARA BINÁRIO
int dec_para_bin(int decimal){
  int resto, bin, pot10 = 1;
  bin = 0;

  while(decimal > 0){
    resto = decimal % 2;
    bin += resto*pot10;
    decimal /= 2;
    pot10 *= 10;
  }
  return bin;
}

//FUNÇÃO QUE CONVERTE UM NUMERO CHAR PARA INT
int char_para_int(const char c){
  int resposta = c - '0';
  return resposta;
}