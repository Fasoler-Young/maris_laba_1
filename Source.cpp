#include<stdio.h>



#define CMP_LT(a, b) (strcmp((a), (b)) < 0)
#define CMP_EQ(a, b) (strcmp((a), (b)) == 0)

typedef struct Entry {
    char* key;
    char* value;
} Entry;

void freeEntry(Entry** e) {
    free((*e)->key);
    free((*e)->value);
    free(*e);
    /**e = NULL;*/
}
#define FREE_ENTRY(e) freeEntry(e)

static const size_t INIT_SIZE = 100;
static const float MULTIPLIER = 2.0f;



typedef struct Dictionary {
    size_t limit;
    size_t size;
    float multiplier;
    Entry** data;
} Dictionary;

Dictionary * createDictionary(size_t initSize) {
    Dictionary* dict = (Dictionary*)malloc(sizeof(Dictionary));
    dict->limit = initSize > INIT_SIZE ? initSize : INIT_SIZE;
    dict->size = 0;
    dict->data = malloc(sizeof(Entry*) * dict->limit);
    dict->multiplier = MULTIPLIER;
    return dict;
}

__inline static int find(char* key, Entry** data, size_t size, size_t * index) {
    size_t i = 0;
    size_t j = size - 1;
    size_t med;
    int result = 0;
    while (i <= j) {
        //чтобы избежать переполнени€ складываем именно так
        med = i + (j - i) / 2;
        if (CMP_LT(key, data[med]->key)) {
            //ѕроверка, чтобы число не стало меньше нул€
            if (med != 0) {
                j = med - 1;
            }
            else {
                break;
            }
        }
        else if (CMP_EQ(key, data[med]->key)) {
            result = 1;
            break;
        }
        else {
            i = med + 1;
        }
    }
    *index = med;
    return result;
}

int raw_put(Dictionary * dict, Entry * e) {
    size_t i = 0;
    size_t j = dict->size;
    size_t index;
    int contains;

    if (dict->size == 0) {
        dict->data[0] = e;
        dict->size++;
        return;
    }

    contains = find(e->key, dict->data, dict->size, &index);
    //≈сли такой элемент уже содержитс€, то выходим
    if (contains) {
        return 0;
    }
    //ѕровер€ем размер массива
    if (dict->size >= dict->limit) {
        dict->limit = (size_t)(dict->multiplier * dict->limit);
        dict->data = (Entry**)realloc(dict->data, dict->limit * sizeof(Entry*));
        if (dict == NULL) {
            exit(-5);
        }
    }

    if (index >= (dict->size - 1)) {
        index = dict->size;
        if (CMP_LT(e->key, dict->data[dict->size - 1]->key)) {
            dict->data[dict->size] = dict->data[dict->size - 1];
            dict->data[dict->size - 1] = e;
        }
        else {
            dict->data[index] = e;
        }
    }
    else {
        memcpy(&(dict->data[index + 1]), &(dict->data[index]), sizeof(Entry*) * (dict->size - index));
        if (index == 0 && CMP_LT(e->key, dict->data[0]->key)) {
            dict->data[index] = e;
        }
        else {
            dict->data[index + 1] = e;
        }
    }

    dict->size++;
    return 1;
}

int put(Dictionary * *dict, char* key, char* value) {
    Entry* e = (Entry*)malloc(sizeof(Entry));
    int result;
    e->key = key;
    e->value = value;
    result = raw_put(dict, e);
    if (result == 0) {
        FREE_ENTRY(&e);
    }
    return result;
}

Entry * xremove(Dictionary * dict, char* key) {
    int contains;
    size_t index;
    Entry* retVal = NULL;

    if (dict->size == 0) {
        return NULL;
    }

    contains = find(key, dict->data, dict->size, &index);
    if (contains) {
        printf("[[%d]]", index);
        retVal = dict->data[index];
        //≈сли размер блока равен нулю, то поведение не определено,
        //поэтому нужно провер€ть, чтобы мы не скопировали ноль байт
        if (index != dict->size) {
            memcpy(&(dict->data[index]), &(dict->data[index + 1]), (dict->size - index) * sizeof(Entry*));
        }
        dict->size--;
    }
    return retVal;
}

char* get(Dictionary * dict, char* key, int* wasFound) {
    size_t index;
    char* retVal = NULL;

    if (dict->size == 0) {
        *wasFound = 0;
    }
    else {
        *wasFound = find(key, dict->data, dict->size, &index);
        if (*wasFound) {
            retVal = dict->data[index]->value;
        }
    }

    return retVal;
}

void destroyDictionary(Dictionary * *dict) {
    size_t i;
    for (i = 0; i < (*dict)->size; i++) {
        FREE_ENTRY(&((*dict)->data[i]));
    }
    free((*dict)->data);
    free(*dict);
    *dict = NULL;
}

Entry * getMax(Dictionary * dict) {
    if (dict->size > 0) {
        return dict->data[dict->size - 1];
    }
    else {
        return NULL;
    }
}

Entry* getMin(Dictionary * dict) {
    if (dict->size > 0) {
        return dict->data[0];
    }
    else {
        return NULL;
    }
}

int main()
{
    Dictionary* d = createDictionary(0);
    char a, b;
    a = 'a';
    char *b = &"abc";
    put(d, &a, &b);
    int c = 1;
    char* res = get(d, &a, &c);
    printf("%s\n", res);
    return 0;
}

