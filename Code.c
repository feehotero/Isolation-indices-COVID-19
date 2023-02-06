#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define phi 1.618033989
#define sqrt5 2.236067977
#define TAM_vetor 24000

#define NOME_ARQUIVO "ordenar.csv"

int pesquisa1(int, int, int, int, char[*][*], int[*], int[*], char[*][*], char[*][*], int[*]);
float pesquisa2(int, int, int, int, char[*][*], int[*], int[*], char[*][*], char[*][*], int[*]);
float pesquisa3(int, int, int[*], int[*], int[*]);
int salvar();
int exibirmenu(void);
int exibirmenu2(void);
void linha_esq();
void linha_esq2();
char nome_cidade(int, int, int, char[*][*], int[*]);

typedef struct {
	FILE* fp;
	int g;
	char* n;
} Arq;

typedef struct {
	char municipio[30];
	char codigo[10];
	char populacao[10];
	char estado[5];
	char data[20];
	unsigned long media;
} linha;

/* numero de fibonacci na posicao n */
int fib(int n) {
	return (pow(phi, n)) / sqrt5;
}

/* fecha e abre arquivo com w+ */
void limpaArquivo(Arq* a) {
	if ((void*) - 1 == a) return;
	fclose(a->fp);
	a->fp = fopen(a->n, "w+");

}

Arq* limpaArquivo2(Arq* a, Arq** wr) {
	if ((void*)-1 == a) return NULL;
	fclose(a->fp);
	a->fp = fopen(a->n, "w+");
	(*wr) = a;
	return a;
}

/* printa arquivo desde 0 at� EOF */
void salva_ordenacao(Arq* a) {

	if (a < 0) {
		return;
	}

	linha p;
	int counter = 0;

	char saida_linha[150];
    FILE* saida;
	FILE* f = a->fp;
	int pos = ftell(f);
	rewind(f);
	saida = fopen("ordenado.csv", "w");
	fputs("Munic�pio1;C�digo Munic�pio IBGE;Popula��o estimada (2020);UF1;Data;M�dia de �ndice De Isolamento\n", saida);
	while (!feof(f)) {
		for (int i = 0; i < a->g; i++) {
			fread(&p, sizeof(linha), 1, f);
			if (feof(f)) break;
			fprintf(saida, "%s;%s;%s;%s;%s;%ld%%\n", p.municipio, p.codigo, p.populacao, p.estado, p.data, p.media);
			counter++;
		}
	}
    fclose(saida);
	fseek(f, pos, SEEK_SET);
}

/* L� uma linha do csv, incrementa seu contador e retorna se foi lido com sucesso */
int readFile(linha * lin, int* fReads, FILE* f) {

	fread(lin, sizeof(linha), 1, f);

	(*fReads)++;

	return feof(f) ? NULL : 1;
}

/* retorna a quantidade de linhas csv do arquivo */
int quantidade_info(FILE* a) {

	int pos = ftell(a);
	rewind(a);
	fseek(a, 0, SEEK_END);
	int dist = ftell(a);
	fseek(a, pos, SEEK_SET);

	//printf("Tamanho do arquivo: %d bytes\n", dist);

	return dist / sizeof(linha);
}

int copia_csv(FILE* src, FILE* dst, int n) {
	int b = 0;

	while (b != n) {

		linha lin;
		char buffer[1024];
		fscanf(src, "\n%[^;];%[^;];%[^;];%[^;];%[^;];%u\n", &lin.municipio, &lin.codigo, &lin.populacao , &lin.estado , &lin.data , &lin.media);
		//lin.quantidade = atoi(buffer);
		fwrite(&lin, sizeof(linha), 1, dst);
		if (feof(src)) break;
		b++;
	}

	return b;

}

/* Copia src -> dst e retorna quantidade de linhas escritas
	n = -1 copia at� o final do arquivo */
int copia(FILE* src, FILE* dst, int n) {

	int b = 0;

	while (b != n) {

		linha lin;
		fread(&lin, sizeof(linha), 1, src);
		if (feof(src)) break;
		fwrite(&lin, sizeof(linha), 1, dst);
		b++;
	}

	return b;

}

/* escreve 2 linhas ordenadamente */
void escreve2(linha a1_, linha a2_, FILE* a3) {
	if (a1_.media <= a2_.media) {
		fwrite(&a1_, sizeof(linha), 1, a3);
		fwrite(&a2_, sizeof(linha), 1, a3);
	}
	else {
		fwrite(&a2_, sizeof(linha), 1, a3);
		fwrite(&a1_, sizeof(linha), 1, a3);
	}
}

/* copiar� a1 em a3 at� que a2_ seja maior */
linha copiaAteMenor_(linha a1_, linha a2_, int* a1reads, int* a1g, Arq* a1, Arq* a3) {
	while (a1_.media <= a2_.media && *a1reads < *a1g && !feof(a1->fp)) {
		//escrever conteudo de a1 at� a2 ser menor
		fwrite(&a1_, sizeof(linha), 1, a3->fp);
		readFile(&a1_, a1reads, a1->fp);
	}

	return a1_;
}

/* fun��o principal que ordena tudo */
int ordena(Arq* a1, Arq* a2, Arq* a3, linha a1_, linha a2_, int* a1reads, int* a2reads, int* a1g, int* a2g) {
	//o else indica que a2 � menor que a
	if (*a1reads == *a1g) {
		a2_ = copiaAteMenor_(a2_, a1_, a2reads, a2g, a2, a3);
		escreve2(a1_, a2_, a3->fp);
		copia(a2->fp, a3->fp, *a2g - *a2reads);
		return 1;
	}
	else {
		//faltam ler coisas do a1

		while (*a1reads != *a1g && *a2reads != *a2g && !feof(a1->fp) && !feof(a2->fp)) {

			a2_ = copiaAteMenor_(a2_, a1_, a2reads, a2g, a2, a3);
			a1_ = copiaAteMenor_(a1_, a2_, a1reads, a1g, a1, a3);

			if (*a2reads == *a2g || feof(a2->fp)) {
				a1_ = copiaAteMenor_(a1_, a2_, a1reads, a1g, a1, a3);
				if (feof(a2->fp)) {
					fwrite(&a1_, sizeof(linha), 1, a3->fp);
				}
				else escreve2(a1_, a2_, a3->fp);
				copia(a1->fp, a3->fp, *a1g - *a1reads);
				*a1reads = *a1g;
				return 1;
			}

			if (*a1reads == *a1g || feof(a1->fp)) {
				a2_ = copiaAteMenor_(a2_, a1_, a2reads, a2g, a2, a3);
				if (feof(a2->fp)) {
					fwrite(&a2_, sizeof(linha), 1, a3->fp);
				}
				else escreve2(a1_, a2_, a3->fp);
				copia(a2->fp, a3->fp, *a2g - *a2reads);
				*a2reads = *a2g;
				return 1;
			}
		}
	}
	return 0;
}

/* retorna o ponteiro para arquivo que est� em EOF */
Arq* poliphaseSort(Arq* a1, Arq* a2, Arq* a3, int tam, int max) {

	int* a1g = &a1->g;
	int* a2g = &a2->g;
	int* a3g = &a3->g;

	*a3g = *a1g + *a2g;

	linha a1_, a2_;

	while (!feof(a1->fp) && !feof(a2->fp)) {
		// Quando o A1 acaba mas n�o tem EOF
		if (ftell(a1->fp) / sizeof(linha) == tam && a1->g == 1) break;

		int a1reads = 0, a2reads = 0;

		while (1) {

			//LEITURA
			if (!readFile(&a1_, &a1reads, a1->fp))
				break;
			if (!readFile(&a2_, &a2reads, a2->fp)) {
				fseek(a1->fp, ftell(a1->fp) - sizeof(linha), SEEK_SET);
				break;
			}

			if (*a1g != 1 || *a2g != 1) {
				if (ordena(a1, a2, a3, a1_, a2_, &a1reads, &a2reads, a1g, a2g))
					break;
			}
			else {
				//estamos com gr�o 1 nos 2, entao � uma opera��o simples
				escreve2(a1_, a2_, a3->fp);
			}

			if (a1reads == *a1g && a2reads == *a2g || (feof(a1->fp) || feof(a2->fp)))
				break;
		}
	}

	//Hora de terminar a execu��o
	int tam_arq = ftell(a3->fp);
	if (round(tam_arq / sizeof(linha)) >= max)
		return (void*)-1;

	// Retorna o arquivo que acabou
	if (feof(a1->fp)) { return a1; }
	if (feof(a2->fp)) { return a2; }
	if (feof(a3->fp)) { return a3; }

	// Quando o A1 acaba mas n�o tem EOF
	if (round(ftell(a1->fp) / sizeof(linha)) == tam)
		return a1;

	return 0;
}

/* faz itera��o entre o proprio arquivo*/
void  ordenaParcial(Arq* read1, Arq* read2, Arq* wr, int tam, int max) {

	Arq* k;
	limpaArquivo(read2);
	read1->g = wr->g;

	while (!feof(wr->fp)) {
		//copiar g numeros para read1 e armazenar em r1
		limpaArquivo(read1);
		copia(wr->fp, read1->fp, wr->g);
		rewind(read1->fp);
		if (feof(wr->fp)) break;

		//iterar read1 com o restante do arquivo da escrita
		k = poliphaseSort(read1, wr, read2, tam, max);

		if (k == wr) break;
	}

	//caso tam/gr�o for impar, ent�o copia o restante que est� em r1
	//para o arquivo final
	if ((int)ceil(tam / read1->g) % 2 != 0 || tam % read1->g != 0)
		copia(read1->fp, read2->fp, read1->g);

	//rewind pra proxima leitura
	rewind(read2->fp);
}

/* prepara os arquivos para leitura */
void initiateValues(Arq* a1, Arq* a2, Arq* a3) {
	FILE* arquivo_para_ordenar = fopen(NOME_ARQUIVO, "r");

	//Criamos nosso a1 com os valores de input
	a1->fp = fopen("a1", "w+");


	//Pula o cabe�alho
	char c[1024];
	fgets(c, 1024, arquivo_para_ordenar);


	copia_csv(arquivo_para_ordenar, a1->fp, -1);
	rewind(a1->fp);
	fclose(arquivo_para_ordenar);


	a2->fp = fopen("a2", "w+");
	a3->fp = fopen("a3", "w+");
	//fopen com w+ faz com que os arquivos usados saiam vazios
	a1->g = a2->g = a3->g = 1;

	a1->n = "a1";
	a2->n = "a2";
	a3->n = "a3";
}


int main() {

	Arq a1;
	Arq a3;
	Arq a2;

	//o main recebe o nome do arquivo de numeros aleatorios a ser aberto em argv[1]
	initiateValues(&a1, &a2, &a3);

	int tam = quantidade_info(a1.fp);

	int i;
	for (i = 0; fib(i+1) < tam; i++);
	int max = fib(i);

	//dividir a1 no maior peda�o de fib
	fseek(a1.fp, max * sizeof(linha), SEEK_SET);
	copia(a1.fp, a2.fp, -1);

	rewind(a1.fp); rewind(a2.fp);

	//valores iniciais
	Arq* read1 = &a1, * read2 = &a2, * wr = &a3;
	Arq* k;

	do {

		k = poliphaseSort(read1, read2, wr, max, tam);

		/*  k � o arquivo que deu EOF. ent�o rewind e ele ser� nosso arquivo
			para escrever. read2 ser� rewind */
		if (k == read1) {
			limpaArquivo(read1);
			read1 = read2;
		}

		rewind(wr->fp);
		read2 = wr;

	} while (limpaArquivo2(k, &wr));

	if (wr->g != tam) {
		/* copiando primeira parte do wr para read1 */
		rewind(wr->fp);
		limpaArquivo(read1);

		while (wr->g + 1 < tam) {
			rewind(read2->fp);
			ordenaParcial(read1, read2, wr, tam, max);

			k = wr;
			wr = read2;
			read2 = k;
		}
	}

    salva_ordenacao(wr);

    FILE *arquivo;
    FILE *arquivo_final;

    int opcao = 1;
    char cidade[TAM_vetor][25];
    int codigo_IBGE[TAM_vetor];
    int populacao[TAM_vetor];
    char estado[TAM_vetor][2];
    char data[TAM_vetor][30];
    int isolamento[TAM_vetor];

    arquivo = fopen("20220211_isolamento.csv", "r");

    if(!arquivo) {
        printf("Erro! Nao consegui abrir o arquivo de leitura");
        return 1;
    }

    char linha_arq[150];
    fgets(linha_arq,150, arquivo);

    char *token;
    int n_linhas = 0;
    while(n_linhas < TAM_vetor) {
        fgets(linha_arq,150, arquivo);
        token = strtok(linha_arq, ";");
        strcpy(cidade[n_linhas], token);
        token = strtok(NULL, ";");
        codigo_IBGE[n_linhas] = atoi(token);
        token = strtok(NULL,";");
        populacao[n_linhas] = atoi(token);
        token = strtok(NULL,";");
        strcpy(estado[n_linhas],token);
        token = strtok(NULL,";");
        strcpy(data[n_linhas], token);
        token = strtok(NULL,"%%");
        isolamento[n_linhas] = atoi(token);
        n_linhas++;
    }

    fclose(arquivo);

    arquivo_final = fopen("saida.csv", "w");

    if(!arquivo_final) {
            printf("Erro ao tentar abrir o arquivo de saida !\n");
            return 1;
    }

    int extra = 0, codigo, opcao_salvar;
    char codigo_IBGE_STRING[TAM_vetor][7];
    char pop_STRING[TAM_vetor][8];
    char isol_STRING[TAM_vetor][2];

    while(opcao!=3) {
    printf("\n");
    if(extra == 0){
        opcao = exibirmenu();
        switch(opcao) {
            case 1: codigo = pesquisa1(TAM_vetor, 25, 2, 30, cidade, codigo_IBGE, populacao, estado, data, isolamento);
                    opcao_salvar = salvar();
                    extra++;
                    if(opcao_salvar == 1){
                        printf("\nArquivo salvo com sucesso!");
                        fputs("Munic�pio1;C�digo Munic�pio IBGE;Popula��o estimada (2020);UF1;Data;M�dia de �ndice De Isolamento\n", arquivo_final);
                        for(i=0; i<TAM_vetor; i++){
                            if(codigo == codigo_IBGE[i]){
                                strcpy(linha_arq, cidade[i]);
                                strcat(linha_arq, ";");
                                sprintf(codigo_IBGE_STRING[i],"%d", codigo_IBGE[i]);
                                strcat(linha_arq, codigo_IBGE_STRING[i]);
                                strcat(linha_arq, ";");
                                sprintf(pop_STRING[i],"%d", populacao[i]);
                                strcat(linha_arq, pop_STRING[i]);
                                strcat(linha_arq, ";");
                                strcat(linha_arq, "SP");
                                strcat(linha_arq, ";");
                                strcat(linha_arq, data[i]);
                                strcat(linha_arq, ";");
                                sprintf(isol_STRING[i],"%d", isolamento[i]);
                                strcat(linha_arq, isol_STRING[i]);
                                strcat(linha_arq, "%");
                                fprintf(arquivo_final, "%s\n", linha_arq);
                            }
                        }
                    }
                    break;
            case 2: printf("\nMedia = %.2f", pesquisa2(TAM_vetor, 25, 2, 30, cidade, codigo_IBGE, populacao, estado, data, isolamento));
                    break;
            case 3: printf("\nObrigado por utilizar nosso programa.");
                    break;
            default:
                    printf("Opcao invalida!");
        }
    }
    else{
        opcao = exibirmenu2();
        switch(opcao) {
            case 1: codigo = pesquisa1(TAM_vetor, 25, 2, 30, cidade, codigo_IBGE, populacao, estado, data, isolamento);
                    opcao_salvar = salvar();
                    if(opcao_salvar == 1){
                        printf("\nArquivo salvo com sucesso!");
                        fputs("Munic�pio1;C�digo Munic�pio IBGE;Popula��o estimada (2020);UF1;Data;M�dia de �ndice De Isolamento\n", arquivo_final);
                        for(i=0; i<TAM_vetor; i++){
                            if(codigo == codigo_IBGE[i]){
                                strcpy(linha_arq, cidade[i]);
                                strcat(linha_arq, ";");
                                sprintf(codigo_IBGE_STRING[i],"%d", codigo_IBGE[i]);
                                strcat(linha_arq, codigo_IBGE_STRING[i]);
                                strcat(linha_arq, ";");
                                sprintf(pop_STRING[i],"%d", populacao[i]);
                                strcat(linha_arq, pop_STRING[i]);
                                strcat(linha_arq, ";");
                                strcat(linha_arq, "SP");
                                strcat(linha_arq, ";");
                                strcat(linha_arq, data[i]);
                                strcat(linha_arq, ";");
                                sprintf(isol_STRING[i],"%d", isolamento[i]);
                                strcat(linha_arq, isol_STRING[i]);
                                strcat(linha_arq, "%");
                                fprintf(arquivo_final, "%s\n", linha_arq);
                            }
                        }
                    }
                    break;
            case 2: printf("\nMedia = %.2f", pesquisa2(TAM_vetor, 25, 2, 30, cidade, codigo_IBGE, populacao, estado, data, isolamento));
                    break;
            case 3: printf("\nObrigado por utilizar nosso programa.");
                    break;
            case 4: nome_cidade(codigo, TAM_vetor, 25, cidade, codigo_IBGE);
                    printf("\nMedia da cidade = %.2f", pesquisa3(codigo, TAM_vetor, codigo_IBGE, populacao, isolamento));
                    break;
            default:
                    printf("Opcao invalida!");
        }
    }
    printf("\n");
 }
    fclose(arquivo);

	return 0;
}

char nome_cidade(int cod, int lim, int lim2, char v[lim][lim2], int j[lim]){
    int i;
    for(i=0; i<lim; i++) {
        if(cod == j[i]){
            printf("\n*%s*", v[i]);
            break;
        }
    }
}

int salvar(){
    int option;
    printf("Deseja salvar o arquivo?\n\n");
    printf("[1] SIM\n");
    printf("[2] NAO\n\n");
    printf("Digite uma opcao: ");
    scanf("%d", &option);
    return option;
}

int pesquisa1(int lim, int lim2, int lim3, int lim4, char v[lim][lim2], int j[lim], int k[lim], char l[lim][lim3], char m[lim][lim4], int n[lim]) {
    int i;
    int cod;

    printf("Digite o codigo do municipio: ");
    scanf("%i", &cod);

    for(i=0; i<lim; i++) {
        if(cod == j[i]){
            printf("%s ", v[i]);
            printf("%i ", j[i]);
            printf("%i ", k[i]);
            printf("%.2s ", l[i]);
            printf("%s ", m[i]);
            printf("%i%% ", n[i]);
            printf("\n\n");
        }
    }
    return cod;
}

float pesquisa2(int lim, int lim2, int lim3, int lim4, char v[lim][lim2], int j[lim], int k[lim], char l[lim][lim3], char m[lim][lim4], int n[lim]){

    float media = 0;
    int i;

    for(i=0; i<lim; i++){
        media = media + (k[i] + n[i]);
    }

    media = media / lim;

    return media;

}

float pesquisa3(int cod, int lim, int j[lim], int k[lim], int n[lim]){

    float media = 0;
    int i, x = 0;

    for(i=0; i<lim; i++){
        if(cod == j[i]){
        media = media + (k[i] + n[i]);
        x++;
        }
    }

    media = media / x;

    return media;
}

int exibirmenu() {
    int op;
    linha_esq();
    linha_esq2();
    printf("*  Pesquisa sobre os dados de Isolamento  *\n");
    linha_esq2();
    linha_esq();
    printf("\n[1] Pesquisa por municipio\n");
    printf("[2] Media geral\n");
    printf("[3] Sair\n");
    printf("\nDigite uma opcao: ");
    scanf("%d", &op);
    return op;
}

int exibirmenu2() {
    int op;
    linha_esq();
    linha_esq2();
    printf("*  Pesquisa sobre os dados de Isolamento  *\n");
    linha_esq2();
    linha_esq();
    printf("\n[1] Pesquisa por municipio\n");
    printf("[2] Media geral\n");
    printf("[3] Sair\n");
    printf("[4] Media da cidade\n");
    printf("\nDigite uma opcao: ");
    scanf("%d", &op);
    return op;
}

void linha_esq(){
    int i;
    for(i=0;i<=20;i++){
    printf("*.");
    }
    printf("*");
    printf("\n");
}

void linha_esq2(){
    int i;
    printf("|");
    for(i=0;i<=40;i++){
    printf(" ");
    }
    printf("|");
    printf("\n");
}
