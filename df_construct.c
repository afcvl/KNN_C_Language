#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    float *lineValues;
    int linePosition;
} line;

typedef struct {
    line *line;
    int numLines;
    int numCol;
} DataFrame;

int* df_size(char* path, int shape[2]) {

    FILE* data = fopen(path, "r");
    char c;
    int numLinhas = 0;
    int numCols = 1;
    while ((c= fgetc(data))!=-1){
        if(c=='\n'){
            numLinhas++;
        }
        else if(c==','&&numLinhas==0){
            numCols++;
        }
        else if(c==-1) break;
    }
    shape[0] = numLinhas;
    shape[1] = numCols;

    return shape;
}

line lineConstructor(char* linha, int n_elements) {

    line new_line;
    new_line.lineValues = (float*) malloc(sizeof(float)*n_elements);
    
    float token_lf;
    int pos = 0;
    int i = 0;
    
    char* token = strtok(linha, ",");
    while (token != NULL) {

        token_lf = strtof(token, NULL);

        new_line.lineValues[i] = token_lf;

        token = strtok(NULL, ",");

        i++;
    }

    return new_line;
}   

DataFrame *DataFrameConstructor(char *path) {

    int shape_df[2];  // (n_linhas, n_colunas)

    
    df_size(path, shape_df);
    
    DataFrame *dataFrame = (DataFrame*) malloc(sizeof(DataFrame));
    dataFrame->numLines = shape_df[0];
    dataFrame->numCol = shape_df[1];
    dataFrame->line = (line*) malloc(sizeof(line)*shape_df[0]);

    FILE* data = fopen(path, "r");
    
    char buffer[200];

    int n_line = 0; 
    while(fgets(buffer, sizeof(buffer), data)) {

        dataFrame->line[n_line] = lineConstructor(buffer, dataFrame->numCol);

        n_line++;
    }
    
    return dataFrame;
}

void imprime_linha(line* linha, int size) {

    for(int i = 0; i < size; i++) {
        printf("%f, ",linha->lineValues[i]);
    }
}

void impirme_df(DataFrame* DataFrame, int size) {
    
    for(int i = 0; i < size; i++) {

        printf("n_linha: %d ", i);
        imprime_linha(&DataFrame->line[i], DataFrame->numCol);
        printf("\n");

    }

}