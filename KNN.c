#include <math.h>
#include <omp.h>
#include "df_construct.c"
#include <time.h> 

float distancia_vetores(line vet1, line vet2, int vet_size)
{

    float *vect1 = vet1.lineValues;
    float *vect2 = vet2.lineValues;

    float distancia_quadratica = 0;

    for (int i = 0; i < vet_size; i++)
    {
        distancia_quadratica += pow((vect1[i] - vect2[i]), 2);
    };

    return sqrt(distancia_quadratica);
}

// descobre o maior valor de um array e retorna seu indice
void maior(int *array, int size, int *idxMaior, int *maior)
{
    *idxMaior = 0;
    *maior = 0;

    for (int i = 0; i < size; i++)
    {
        if (array[i] > *maior)
        {
            *maior = array[i];
            *idxMaior = i;
        }
    }
}

void idx_k_menores(float* array, int array_size, int k, int* idxMenores) {
    float* menores = malloc(sizeof(float)*k);
    int indice_maior = 0;
    float valor_maior = INFINITY;

    for(int i=0;i<k;i++){
        menores[i] = INFINITY;
    }
    for(int i=0; i < array_size; i++) {

        if(array[i]<valor_maior){
            menores[indice_maior] = array[i];
            idxMenores[indice_maior] = i;
            valor_maior = 0;
            for(int i = 0;i<k ;i++){
                if(menores[i]>valor_maior){
                    valor_maior = menores[i];
                    indice_maior = i;
                }
            }
        }
    }
}

int classe_prevista(DataFrame* y_train, int* indice_menores, int k) {
    int previsao = 0;
    for(int i = 0; i < k; i++) {
        int temp = indice_menores[i];
        previsao = previsao + (int) y_train->line[temp].lineValues[0]; 
    }


    if(previsao> k/2){
        return 1;
    }
    else return 0;
}

// x_train = features / y_train = labels / x_test = to_predict / k = n_neighbors
DataFrame *knn(DataFrame *x_train, DataFrame *y_train, DataFrame *x_test, int k)
{
    if (k % 2 == 1) {
        k++;
    }

    float *distancias = (float*) malloc(sizeof(float) * x_train->numLines);
    FILE* arquivo = fopen("predicoes.txt", "w");
    float dist;

    for (int i = 0; i <= x_test->numLines; i++)   // percorre cada linha do x_test
    { 

        for (int j = 0; j < x_train->numLines; j++)   // percorre cada linha do x_train (é onde devemos aplicar paralelismo)
        { 

            dist = distancia_vetores(x_test->line[i], x_train->line[j], x_train->numCol);

            distancias[j] = dist;
        }

    int* indice_menores = malloc(sizeof(int)*k);
        
    idx_k_menores(distancias, x_train->numLines, k, indice_menores);

    char caracter = classe_prevista(y_train, indice_menores, k) + 48;

    fputc(caracter, arquivo);
    fputc('\n', arquivo);

    }

    free(distancias);
    return x_train;
}

DataFrame *knn_thread(DataFrame *x_train, DataFrame *y_train, DataFrame *x_test, int k, int n_threads)
{

    if (k % 2 == 1) {
        k++;
    }

    float *distancias = (float*) malloc(sizeof(float) * x_train->numLines);
    FILE* arquivo = fopen("predicoes.txt", "w");

    for (int i = 0; i <= x_test->numLines; i++)   // percorre cada linha do x_test
    { 
        omp_set_num_threads(n_threads);
        #pragma omp parallel
        {       
            int thread_id = omp_get_thread_num();
            float dist;
            for(int k = thread_id; k < x_train->numLines; k += n_threads){

                dist = distancia_vetores(x_test->line[i], x_train->line[k], x_train->numCol);

            distancias[k] = dist;
            }

        }

    int* indice_menores = malloc(sizeof(int)*k);
        
    idx_k_menores(distancias, x_train->numLines, k, indice_menores);

    char caracter = classe_prevista(y_train, indice_menores, k) + 48;

    fputc(caracter, arquivo);
    fputc('\n', arquivo);

    }

    free(distancias);
    return x_train;
}

int main()
{
    DataFrame *x_train = DataFrameConstructor("Dados\\xtrain500000.txt");

    DataFrame *y_train = DataFrameConstructor("Dados\\ytrain500000.txt");

    DataFrame *x_test = DataFrameConstructor("Dados\\xtest.txt");

    clock_t t; 
    t = clock(); 

    //DataFrame *test = knn(x_train, y_train, x_test, 3); 

    DataFrame *test = knn_thread(x_train, y_train, x_test, 3, 4); 

    
    t = clock() - t; 
    double time_taken = (((double)t)/CLOCKS_PER_SEC); // in seconds 
 
    printf("knn demorou %f segundos para executar \n", time_taken); 
    
    return 0;
}