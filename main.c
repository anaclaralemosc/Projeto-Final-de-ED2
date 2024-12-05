#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO_HASH 100
#define MAX_VEICULOS 100

        // Estruturas, Tabela Hash e Dados Iniciais
// Estrutura para representar um veículo
typedef struct Veiculo {
    char placa[100];               // placa do veículo
    char modelo[100];          // modelo do veículo
    char local[100];       // localização do veículo
    struct Veiculo *proximo; // Ponteiro para o próxima veículo (lista ligada)
} Veiculo;

// Estrutura para análise de congestionamento
typedef struct {
    char local[100];
    int quantidade;
} Congestionamento;

// Estrutura da Tabela Hash
typedef struct {
    Veiculo *tabela[TAMANHO_HASH];  // Array de ponteiros para o início da lista ligada
} TabelaHash;

// Função Hash: Retorna o índice baseado na placa do veículo
int funcao_hash (char *str) {
    int soma =0;
    for (int i=0; str[i] != '\0'; i++){
        soma += str[i];
    }
    
    return soma % TAMANHO_HASH;  // Usa módulo para determinar a posição na tabela
}

// Função para carregar dados string de um arquivo para um vetor
int carregarDados(const char *nomeArquivo, char dados[MAX_VEICULOS][100]) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    int i = 0;
    char linha[100 + 1];  // Buffer para ler a linha com segurança
    while (i < MAX_VEICULOS && fgets(linha, sizeof(linha), arquivo) != NULL) {
        // Remove o caractere de nova linha, se presente
        linha[strcspn(linha, "\n")] = '\0';

        // Ignora linhas em branco
        if (strlen(linha) > 0) {
            strncpy(dados[i], linha, 100);
            dados[i][100 - 1] = '\0';  // Garante que a string termina corretamente
            i++;
        }
    }

    fclose(arquivo);
    return i;  // Retorna o número de dados carregadas
}


        // Funções para análise de congestionamento
// Função para criar um local ou encontrar um já existente quando fazer a contagem de veículos em cada local
int encontrar_ou_adicionar_local(Congestionamento *contagem, int *totalLocais, const char *local) {
    for (int i = 0; i < *totalLocais; i++) {
        if (strcmp(contagem[i].local, local) == 0) {
            return i; // Retorna o índice do local já existente
        }
    }

    // Adiciona um novo local se não foi encontrado
    if (*totalLocais < MAX_VEICULOS) {
        strcpy(contagem[*totalLocais].local, local);
        contagem[*totalLocais].quantidade = 0;
        (*totalLocais)++;
        return *totalLocais - 1; // Retorna o índice do novo local
    }

    printf("Erro: Número de locais registrados ultrapassa número de veículos.\n");
    return -1;
}

// Função para comparar a quantidade de veículos nos locais e realizar o qsort
int comparar_contagem(const void *a, const void *b) {
    // Compara as quantidades de veículos de forma decrescente
    Congestionamento *contagemA = (Congestionamento *)a;
    Congestionamento *contagemB = (Congestionamento *)b;

    // Retorna a diferença de contagens de forma decrescente
    return (contagemB->quantidade - contagemA->quantidade);
}

// Atualiza a estrutura de congestionamento de acordo com a presença de veículos em dado local
void atualizar_contagem(Congestionamento *contagem, int *totalLocais, const char *local, int incremento) {
    int indice = encontrar_ou_adicionar_local(contagem, totalLocais, local);
    if (indice != -1) {
        contagem[indice].quantidade += incremento;
        if (contagem[indice].quantidade < 0) {
            contagem[indice].quantidade = 0; // Garante que a contagem não seja negativa
        }
    }
}


        // Operações do Menu //
// Função para criar um novo veículo
Veiculo *criar_veiculo(const char *placa, const char *modelo, const char *local) {
    Veiculo *novo_veiculo = (Veiculo *)malloc(sizeof(Veiculo));
    if (novo_veiculo == NULL) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }
    strcpy(novo_veiculo->placa, placa);
    strcpy(novo_veiculo->modelo, modelo);
    strcpy(novo_veiculo->local, local);
    novo_veiculo->proximo = NULL;
    return novo_veiculo;
}

// Função para inserir um veículo na tabela hash (usando listas ligadas para colisões)
void inserir(TabelaHash *tabela, Veiculo *veiculo, Congestionamento *contagem, int *totalLocais) {
    int indice = funcao_hash(veiculo->placa);  // Calcula o índice baseado na placa
    veiculo->proximo = tabela->tabela[indice];  // Insere no início da lista ligada
    tabela->tabela[indice] = veiculo;
    
    atualizar_contagem(contagem, totalLocais, veiculo->local, 1); // Soma 1 à contagem do local
}

// Função para buscar um veículo na tabela hash pela placa
Veiculo *buscar_placa(TabelaHash *tabela, char *placa) {
    int indice = funcao_hash(placa);  // Calcula o índice com base na placa
    Veiculo *atual = tabela->tabela[indice];
    
    while (atual != NULL) {
        if (strcmp(atual->placa, placa) == 0) {  // Compara as placas
            return atual;  // Retorna o veículo encontrado
        }
        atual = atual->proximo;
    }
    return NULL;  // Retorna NULL se não encontrar
}

// Função para buscar veículos pelo modelo
void buscar_modelo(TabelaHash *tabela, char *modelo) {
    int encontrado = 0;
    for (int i = 0; i < TAMANHO_HASH; i++) {
        Veiculo *atual = tabela->tabela[i];
        while (atual != NULL) {
            if (strcmp(atual->modelo, modelo) == 0) {  // Compara os modelos
                printf("Placa: %s       Modelo: %s\nLocalização: %s     (Índice: %d)\n\n",
                       atual->placa, atual->modelo, atual->local, i);
                encontrado = 1;
            }
            atual = atual->proximo;
        }
    }
    if (!encontrado) {
        printf("Nenhum veículo com o modelo '%s' foi encontrado.\n", modelo);
    }
}

// Função para exibir todos os veículos e seus dados
void exibir_veiculos(TabelaHash *tabela) {
    for (int i = 0; i < TAMANHO_HASH; i++) { 
        Veiculo *atual = tabela->tabela[i];
        while (atual != NULL) {
            printf("Placa: %s       Modelo: %s\nLocalização: %s     (Índice: %d)\n\n",
                   atual->placa, atual->modelo, atual->local, i);
            atual = atual->proximo;  // Avança para o próximo veículo
        }
    }
}

// Função para exibir todos os veículos em um local
void buscar_local(TabelaHash *tabela, char *local) {
    int encontrado = 0;
    for (int i = 0; i < TAMANHO_HASH; i++) {
        Veiculo *atual = tabela->tabela[i];
        while (atual != NULL) {
            if (strcmp(atual->local, local) == 0) {  // Compara as localizações
                printf("Placa: %s       Modelo: %s\nLocalização: %s     (Índice: %d)\n\n",
                       atual->placa, atual->modelo, atual->local, i);
                encontrado = 1;
            }
            atual = atual->proximo; // Avança para o próximo veículo
        }
    }
    if (!encontrado) {
        printf("Nenhum veículo na localização '%s' foi encontrado.\n", local);
    }
}

// Função para exibir o congestionamento em cada local
void exibir_congestionamento(Congestionamento *contagem, int totalLocais) {
    // Ordena a contagem de veículos por local em ordem decrescente
    qsort(contagem, totalLocais, sizeof(Congestionamento), comparar_contagem);

    for (int i = 0; i < totalLocais; i++) {
        if(i==0){
            printf("O local mais congestionado é: %s, com %d veículos.\n\n", contagem[i].local, contagem[i].quantidade);
        } else {
            printf("Local: %s\nQuantidade de veículos: %d\n\n", contagem[i].local, contagem[i].quantidade);
        }
    }
}

// Função para excluir um veículo na tabela hash pela placa
int excluir(TabelaHash *tabela, char *placa, Congestionamento *contagem, int *totalLocais) {
    int indice = funcao_hash(placa);  // Calcula o índice da tabela hash
    Veiculo *atual = tabela->tabela[indice];
    Veiculo *anterior = NULL;

    // Percorre a lista no índice da tabela hash
    while (atual != NULL) {
        if (strcmp(atual->placa, placa) == 0) {  // Compara as placas
            atualizar_contagem(contagem, totalLocais, atual->local, -1); // Subtrai um da contagem do local
            
            if (anterior == NULL) {
                // Caso seja o primeiro da lista
                tabela->tabela[indice] = atual->proximo;
            } else {
                // Caso seja um elemento no meio ou no fim da lista
                anterior->proximo = atual->proximo;
            }
            free(atual);  // Libera a memória do veículo
            return 1;  // Excluiu com sucesso
        }
        // Avança para o próximo elemento
        anterior = atual;
        atual = atual->proximo;
    }

    return 0;  // Não excluiu
}



        // Função principal para demonstrar o uso da tabela hash para análise de dados de tráfego urbano 
int main() {
    // Inicializa a semente para geração de números aleatórios
    srand(time(NULL)); 
    
    // Inicializa a tabela hash
    TabelaHash tabela = {NULL};
    
    // Inicializa os dados de análise de congestionamento
    Congestionamento contagem[MAX_VEICULOS];
    int totalLocais = 0;
    
    // Inserção de dados iniciais de 'x' veículos dos arquivos.
    char placas[MAX_VEICULOS][100], modelos[MAX_VEICULOS][100], locais[MAX_VEICULOS][100];
    const char *arqPlacas = "placas100.txt";    //
    const char *arqModelos = "modelos100.txt";  // Editar arquivos para mudar quantidade de dados
    const char *arqLocais = "locais100.txt";    //
    
    int numPlacas = carregarDados(arqPlacas, placas);
    int numModelos = carregarDados(arqModelos, modelos);
    int numLocais = carregarDados(arqLocais, locais);
    
    int i;
    for(i=0;i<MAX_VEICULOS;i++){
        inserir(&tabela, criar_veiculo(placas[i], modelos[i], locais[i]), contagem, &totalLocais);
    }
    
    // Menu para o usuário
    int op;
    
    while(1){
        printf("\nEscolha uma opção:\n");
        printf("1. Inserir veículo\n");
        printf("2. Buscar veículo pela placa\n");
        printf("3. Buscar veículos pelo modelo (Com letras maiúsculas e acentos adequados.)\n");
        printf("4. Exibir todos os veículos registrados\n");
        printf("5. Exibir todos os veículos registrados em determinada região (Com letras maiúsculas e acentos adequados.)\n");
        printf("6. Exibir o congestionamento das regiões\n");
        printf("7. Excluir um veículo pela placa\n");
        printf("8. Sair\n");
        scanf("%d", &op);
        
        switch (op) {
            case 1: {
                char placa[100], modelo[100], local[100];
                printf("\nDigite os dados do veículo que deseja inserir:\nPlaca: ");
                scanf("%s", placa);
                getchar();
                printf("Modelo: ");
                fgets(modelo, sizeof(modelo), stdin);
                modelo[strcspn(modelo, "\n")] = '\0';
                
                printf("Local: ");
                fgets(local, sizeof(local), stdin);
                local[strcspn(local, "\n")] = '\0';
                
                inserir(&tabela, criar_veiculo(placa, modelo, local), contagem, &totalLocais);
                
                printf("O veículo foi inserido com sucesso!\n");
                break;
            }

            case 2: {
                char placa[100];
                printf("\nDigite a placa que deseja buscar: ");
                scanf("%s", placa);
                
                clock_t start = clock();
                Veiculo *veiculo = buscar_placa(&tabela, placa);
                if (veiculo != NULL) {
                    printf("Veículo encontrado:\n");
                    printf("Placa: %s       Modelo: %s\nLocalização: %s\n\n",
                            veiculo->placa, veiculo->modelo, veiculo->local);
                } else {
                    printf("Veículo com placa '%s' não encontrado.\n", placa);
                }
                clock_t end = clock();
                
                double calc = ((double)(end - start)) / CLOCKS_PER_SEC;
                printf("\nExecutado em %f segundos.\n", calc);
                break;
            }

            case 3: {
                char modelo[100];
                printf("\nDigite o modelo que deseja buscar: ");
                getchar();
                fgets(modelo, sizeof(modelo), stdin);
                modelo[strcspn(modelo, "\n")] = '\0';
                
                clock_t start = clock();
                buscar_modelo(&tabela, modelo);
                clock_t end = clock();
                
                double calc = ((double)(end - start)) / CLOCKS_PER_SEC;
                printf("\nExecutado em %f segundos.\n", calc);
                break;
            }

            case 4: {
                printf("\nAqui estão todos os veículos registrados:\n");
                
                clock_t start = clock();
                exibir_veiculos(&tabela);
                clock_t end = clock();
                
                double calc = ((double)(end - start)) / CLOCKS_PER_SEC;
                printf("\nExecutado em %f segundos.\n", calc);
                break;
            }
            
            case 5: {
                char local[100];
                printf("\nDigite em qual localização deseja buscar: ");
                getchar();
                fgets(local, sizeof(local), stdin);
                local[strcspn(local, "\n")] = '\0';
                
                clock_t start = clock();
                buscar_local(&tabela, local);
                clock_t end = clock();
                
                double calc = ((double)(end - start)) / CLOCKS_PER_SEC;
                printf("\nExecutado em %f segundos.\n", calc);
                break;
            }
            
            case 6: {
                printf("\nA quantidade de veículos por região, em ordem decrescente, é a seguinte:\n\n");
                
                clock_t start = clock();
                exibir_congestionamento(contagem, totalLocais);
                clock_t end = clock();
                
                double calc = ((double)(end - start)) / CLOCKS_PER_SEC;
                printf("\nExecutado em %f segundos.\n", calc);
                break;
            }
            
            case 7: {
                char placa[100];
                printf("\nDigite a placa do veículo que deseja excluir: ");
                scanf("%s", placa);
                
                clock_t start = clock();
                if (excluir(&tabela, placa, contagem, &totalLocais)) {
                    printf("Veículo excluído com sucesso.\n");
                } else {
                    printf("Veículo com a placa '%s' não foi encontrado.\n", placa);
                }
                clock_t end = clock();
                
                double calc = ((double)(end - start)) / CLOCKS_PER_SEC;
                printf("\nExecutado em %f segundos.\n", calc);
                break;
            }
            
            case 8: {
                printf("Saindo do programa...\n");
                return 0;
            }
            
            default:
                printf("Opção inválida! Escolha novamente.\n");
                break;
        }
    }

    return 0;
}
