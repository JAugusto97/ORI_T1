/* Autores: Caio Luiggy Riyoichi Sawada Ueno - 743516
 *          Gabriel Cheban do Prado Mendes - 743535
 *          João Augusto Leite - 743551
 *
 * Proposito: Trabalho 1 de Organização e Recuperação da Informação (UFSCar - DC). Este programa deve gerar um arquivo .dat com dados
 *            estruturados da seguinte maneira:
 *              * Blocos de 512 bytes, sendo os 2 últimos bytes o tamanho válido do bloco em formato little-endian;
 *              * Registros de tamanho variável separados pelo caractere 10 da tabela ASCII;
 *              * Campos de tamanho variável separados pelo caractere 13 da tabela ASCII;
 *              * Os campos são: RA, Nome, Sigla do Curso e Ano de Ingresso;
 *            Além disso, o programa permite realizar buscas em arquivos de estrutura semelhante, passando o RA do aluno como chave.
 *
 * Link da especificacao detalhada: https://drive.ufscar.br/d/2947e699b2/files/?p=/trabalhos/t1/trabalho_1.pdf
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define K_RA 7
#define K_NOME 201
#define K_SIGLA 201
#define K_ANO 5
#define K_ARQUIVO 201

//Struct Registro contendo os campos a serem lidos
typedef struct{
    char RA[K_RA], nome[K_NOME], curso[K_SIGLA], ano[K_ANO];
} Registro;

//Função para contar o número de bytes em um registro (incluindo separadores)
int contaRegistro(Registro *r){
    return (strlen(r->nome) + strlen(r->curso) + strlen(r->RA) + strlen(r->ano) + 6);
}

//Função para converter um número (short unsigned) para o formato Little-Endian
short unsigned int inverte(short unsigned int numero){
    numero = ( ((numero%16) << 4) + (numero/16) );
    return numero;
}

//Função para preencher um bloco com os registros lidos
void populaBloco(char *titulo){
    char lixo[2], separaCampo = 13, separaRegistro = 10;
    char bloco[511];
    int numeroRegistros, i, j;
    int bytesLivres = 510;
    short unsigned int  bytesUsados;
    Registro aluno;

    bloco[0] = '\0'; //Evitar lixo

    scanf("%s", titulo); //Nome do arquivo
    strcat(titulo, ".dat"); //Nome do arquivo com extensão
    scanf("%d", &numeroRegistros); //Quantidade de registros
    gets(lixo); //Ler o "enter" deixado pela função scanf

    FILE *arquivo = fopen (titulo, "wb"); //Abertura do arquivo

    if (arquivo == NULL)
        printf("Não foi possível abrir o arquivo\n");
    else{
        for(j=0; j<numeroRegistros; j++){ //Laço de leitura
            gets(aluno.RA);
            gets(aluno.nome);
            gets(aluno.curso);
            gets(aluno.ano);

            if(contaRegistro(&aluno) <= bytesLivres){ //Se há espaço no bloco, concatene o registro no bloco
                strcat(bloco, aluno.RA);
                strcat(bloco, &separaCampo);
                strcat(bloco, aluno.nome);
                strcat(bloco, &separaCampo);
                strcat(bloco, aluno.curso);
                strcat(bloco, &separaCampo);
                strcat(bloco, aluno.ano);
                strcat(bloco, &separaCampo);
                strcat(bloco, &separaRegistro);
                bytesLivres -= contaRegistro(&aluno); //Decremento da quantidade de bytes livres
            }
            else{ //Se não há espaço, preencha o restante do bloco com *
                for (i = 511 - bytesLivres; i < 510; i++)
                    bloco[i] = '*';

                fwrite(bloco, sizeof(char),510,arquivo); //Salva o bloco na memória secundária (510 bytes)
                bytesUsados = 510 - bytesLivres; //Tamanho do bloco (bytes livres)
                bytesUsados = inverte(bytesUsados); //Conversão para Little-Endian
                fwrite(&bytesUsados, sizeof(short unsigned int),1,arquivo); //Salva a quantidade de bytes válidos do bloco (2 bytes)

                bytesLivres = 510; //Novo bloco
                bloco[0] = '\0'; //Não há nada no bloco

                //Caso o último registro não caiba no bloco, concatene o registro no bloco novo
                strcat(bloco, aluno.RA);
                strcat(bloco, &separaCampo);
                strcat(bloco, aluno.nome);
                strcat(bloco, &separaCampo);
                strcat(bloco, aluno.curso);
                strcat(bloco, &separaCampo);
                strcat(bloco, aluno.ano);
                strcat(bloco, &separaCampo);
                strcat(bloco, &separaRegistro);
                bytesLivres -= contaRegistro(&aluno); //Decremento da quantidade de bytes livres
            }
        }
        if(bytesLivres < 510) //Preencher o último bloco com *
            for (i = 511 - bytesLivres; i < 510; i++)
                bloco[i] = '*';

        fwrite(bloco, sizeof(char),510,arquivo); //Salva o bloco na memória secundária (510 bytes)
        bytesUsados = 510 - bytesLivres; //Tamanho do bloco (bytes livres)
        bytesUsados = inverte(bytesUsados); //Conversão para Little-Endian
        fwrite(&bytesUsados, sizeof(short unsigned int),1,arquivo); //Salva a quantidade de bytes válidos do bloco (2 bytes)

    }
    fclose(arquivo);
    arquivo = NULL;
}

//Função para buscar o registro com base no RA
int buscaRA(char *bloco, char *procuraRA, Registro *aluno){
    int i, j=0, k, flag;
    char recuperaRA[K_RA];

    do{ //Procure o RA enquanto não o encontrar ou não terminar o bloco
        k = 0;
        for(i=0; i<K_RA; i++,j++) //Copie o RA do bloco para o vetor recuperaRA
            recuperaRA[i] = bloco[j];
        recuperaRA[K_RA] = '\0';

        flag = strncmp(recuperaRA,procuraRA,6); //Retorna 0 se encontrar o RA solicitado


        if(!flag){ //Salva as informações no registro se encontrar o RA
            strcpy(aluno->RA, procuraRA);
            while(bloco[j] != 13)
                aluno->nome[k++] = bloco[j++];
            aluno->nome[k] = '\0';
            k = 0;
            j++;
            while(bloco[j] != 13)
                aluno->curso[k++] = bloco[j++];
            aluno->curso[k] = '\0';
            k = 0;
            j++;
            while(bloco[j] != 13)
                aluno->ano[k++] = bloco[j++];
            aluno->ano[k] = '\0';

        }

        while(bloco[j] != 10 ){ //Percorre o vetor bloco até o final de um registro
            j++;
        }
        j+=2; //j é o índice do próximo RA
    }while(flag && j <= 512);

    return !flag;
}

//Função para recuperar os blocos e solicitar a comparação do RA
void buscaBloco(char* procuraRA, char* titulo){
    char bloco[513];
    int blocoNumero = 1, flagFound, flagEOF;
    Registro aluno;

    FILE *arquivo = fopen(titulo, "rb"); //Abertura do arquivo

    if (arquivo == NULL)
        printf("Não foi possível abrir o arquivo\n");

    else{
        do{ //Recupera os blocos enquanto não encontrar ou não terminar o arquivo
            flagEOF = fseek(arquivo,((blocoNumero-1)*512),SEEK_SET); //Retorna 0 sucesso

            fread(bloco,sizeof(char),512,arquivo);

            flagFound = buscaRA(bloco, procuraRA, &aluno); //Retorna 1 caso ache o registro
            flagEOF = strlen(bloco); //Retorna o tamanho do bloco

            blocoNumero++;

            bloco[0] = '\0';

        }while(!flagFound && flagEOF > 0);

        if(flagFound) //Caso enconte pelo RA
            printf("%s:%s:%s:%s\n", aluno.RA, aluno.nome, aluno.curso, aluno.ano);
        else //Senão
            printf("*\n");
    }

    fclose(arquivo);
    arquivo = NULL;
}

int main(){
    char titulo[K_ARQUIVO],procuraRA[K_RA];

    populaBloco(titulo);

    scanf("%s", procuraRA);
    while(procuraRA[0] != '0' || procuraRA[1] != '\0'){
        buscaBloco(procuraRA, titulo);
        scanf("%s", procuraRA);
    }

    return 0;
}
