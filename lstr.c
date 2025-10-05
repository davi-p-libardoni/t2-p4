#include "lstr.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct no{
    struct no* ant;
    struct no* prox;
    str string;
} no;

struct lstr{
    no* primeiro;
    no* ultimo;
    no* corrente;
    int tam;
    int pos;
};

Lstr ls_cria(){
    Lstr new = malloc(sizeof(struct lstr));
    new->primeiro = NULL;
    new->ultimo = NULL;
    new->corrente = NULL;
    new->tam = 0;
    new->pos = -1;
    return new;
}

void ls_destroi(Lstr self){
    if(ls_vazia(self)){ 
        free(self);
        return;
    }
    ls_inicio(self);
    while(self->primeiro != NULL){
        no* primeiro = self->primeiro;
        self->primeiro = primeiro->prox;
        free(primeiro);
    }
    free(self);
}

bool ls_vazia(Lstr self){
    return (self->primeiro==NULL)?1:0;
}

int ls_tam(Lstr self){
    return self->tam;
}

bool ls_item_valido(Lstr self){
    return (ls_vazia(self) || self->pos <= -1 || self->pos >= self->tam)? 0 : 1;
}

str ls_item(Lstr self){
    assert(ls_item_valido(self));
    str item = s_sub(self->corrente->string,0,self->corrente->string.tamc);
    return item;
}

str *ls_item_prt(Lstr self){
    assert(ls_item_valido(self));
    return &self->corrente->string;
}

void ls_inicio(Lstr self){
    self->corrente = NULL;
    self->pos = -1;
}

void ls_final(Lstr self){
    self->corrente = NULL;
    self->pos = self->tam;
}

void ls_posiciona(Lstr self, int pos){
    if(pos<0) pos += self->tam;
    if(pos<0){
        ls_inicio(self);
        return;
    }
    if(pos>=self->tam){
        ls_final(self);
        return;
    }
    int begin = (pos > (self->tam - pos))?1:0;
    if(begin == 0){
        self->corrente = self->primeiro;
        self->pos = 0;
        while(self->pos != pos)
            ls_avanca(self);
    }else{
        self->corrente = self->ultimo;
        self->pos = self->tam-1;
        while(self->pos != pos)
            ls_recua(self);
    }
    self->pos = pos;
}

bool ls_avanca(Lstr self){
    if(self->pos == self->tam) return 0;
    self->pos += 1;
    self->corrente = (self->corrente != NULL)?self->corrente->prox:self->primeiro;
    return (self->corrente == NULL)?0:1;
}

bool ls_recua(Lstr self){
    if(self->pos == -1) return 0;
    self->pos -= 1;
    self->corrente = (self->corrente != NULL)?self->corrente->ant:self->ultimo;
    return (self->corrente == NULL)?0:1;
}

static no* cria_no(no* ant, no* prox, str cad){
    no* new = malloc(sizeof(no));
    new->ant = ant;
    new->prox = prox;
    new->string = s_copia(cad);
    return new;
}

static void insere_lista_vazia(Lstr self, str cad){
    no* novo = cria_no(NULL,NULL,cad);
    self->primeiro = novo;
    self->ultimo = novo;
    ls_avanca(self);
    self->pos = 0;
    self->tam += 1;
}

static void insere_inicio(Lstr self, str cad){
    no* novo = cria_no(NULL,self->primeiro,cad);
    self->primeiro->ant = novo;
    self->primeiro = novo;
    self->corrente = novo;
    self->pos = 0;
    self->tam += 1;
}

static void insere_final(Lstr self, str cad){
    no* novo = cria_no(self->ultimo,NULL,cad);
    self->ultimo->prox = novo;
    self->ultimo = novo;
    self->corrente = novo;
    self->pos = self->tam;
    self->tam += 1;
}

static void desloca_lista_inserir(Lstr self, no* new, no* anterior, no* proximo){
    anterior->prox = new;
    proximo->ant = new;
}

void ls_insere_antes(Lstr self, str cad){
    if(ls_vazia(self)){
        insere_lista_vazia(self,cad);
        return;
    }
    if(self->pos <= 0){
        insere_inicio(self,cad);
        return;
    }
    if(self->pos == self->tam){
        insere_final(self,cad);
        return;
    }
    no* proximo = self->corrente;
    no* anterior = self->corrente->ant;
    no* novo = cria_no(anterior,proximo,cad);
    desloca_lista_inserir(self,novo,anterior,proximo);
    self->corrente = novo;
    self->tam += 1;
}

void ls_insere_depois(Lstr self, str cad){
    if(ls_vazia(self)){
        insere_lista_vazia(self,cad);
        return;
    }
    if(self->pos < 0){
        insere_inicio(self,cad);
        return;
    }
    if(self->pos >= self->tam-1){
        insere_final(self,cad);
        return;
    }
    no* proximo = self->corrente->prox;
    no* anterior = self->corrente;
    no* novo = cria_no(anterior,proximo,cad);
    desloca_lista_inserir(self,novo,anterior,proximo);
    self->corrente = novo;
    self->tam += 1;
}

str ls_remove(Lstr self){
    assert(!ls_vazia(self) && ls_item_valido(self));
    str strRemovida = s_copia(self->corrente->string);
    no* remover = self->corrente;
    if(remover != self->primeiro) remover->ant->prox = self->corrente->prox;
    if(remover != self->ultimo) remover->prox->ant = self->corrente->ant;
    self->tam -= 1;
    s_destroi(remover->string);
    self->corrente = remover->prox;
    if(self->primeiro == remover) self->primeiro = remover->prox;
    if(self->ultimo == remover) self->ultimo = remover->ant;
    free(remover);
    return strRemovida;
}

Lstr ls_sublista(Lstr self, int tam){
    Lstr new = ls_cria();
    if(tam < 0 || self->pos >= self->tam) return new;
    int fim = (self->tam > (self->pos + tam - 1))?self->pos + tam - 1:self->tam - 1;
    for(int i = self->pos;i <= fim;i++,ls_avanca(self))
        ls_insere_depois(new,self->corrente->string);
    return new;
}

str ls_junta(Lstr self, str separador){
    if(self->tam == 0) return s_copia(s_(""));
    if(self->tam == 1) return s_copia(self->primeiro->string);
    str nova = s_copia(s_(""));
    for(ls_inicio(self);ls_avanca(self);){
        if(self->pos > 0) s_cat(&nova,separador);
        s_cat(&nova,self->corrente->string);
    }
    return nova;
}

void ls_imprime(Lstr self){
    if(ls_vazia(self)) return;
    for(ls_inicio(self);ls_avanca(self);){
        s_imprime(self->corrente->string);
        printf("\n");
    }
}

static void ls_info(Lstr self){
    printf("//   //\n");
    if(ls_vazia(self)){
        printf("Lista vazia.\n//   //\n");
        return;
    }
    printf("Primeiro - \"");
    s_imprime(self->primeiro->string);
    printf("\"\nÚltimo - \"");
    s_imprime(self->ultimo->string);

    ls_inicio(self);
    printf("\"\n-1 -> NULL\n");
    for(ls_inicio(self);ls_avanca(self);){
        printf("%d -> ",self->pos);
        s_imprime(self->corrente->string);
        printf("\n");
    }
    printf("%d -> NULL\n",self->tam);
    printf("//   //\n\n");
}