#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

struct Enj_Hashmap_Node_{
    char *key
    void *value

    size_t hash;
};

int Enj_InitHashmap(Enj_Hashmap *m){
    m->data = malloc((1<<3)*sizeof(struct Enj_Hashmap_Node_));
    m->size = 0;
    m->logcap = 3;

    if(!m->data) return 1;
    for(size_t i = 0; i < (1<<m->logcap); i++){
        ((struct Enj_Hashmap_Node_ *)m->data)[i].key = NULL;
    }

    return 0;
}
void Enj_FreeHashmap(Enj_Hashmap *m){
    for(size_t i = 0; i < (1<<m->logcap); i++){
        //free silently ignores NULL already
        free(((struct Enj_Hashmap_Node_ *)m->data)[i].key);
    }
    free(m->data);
}

size_t hash_str(const char *s){
    size_t res = 7;
    while(*s){
        res = 31 * res + *(s++);
    }
    return res;
}

int Enj_HashmapHas(Enj_Hashmap *m, const char *k){
    size_t hash = hash_str(k);
    size_t idx = hash & ((1<<m->logcap)-1);

    for(size_t i = 0; i < 1<<m->logcap; i++){
        size_t ibuf = (i+idx) & ((1<<m->logcap)-1);
        char *ki = ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].key;
        size_t hi = ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].hash;

        if(!ki){
            return 0;
        }
        if(hi == hash && strcmp(ki, k) == 0){
            return 1;
        }
    }
    return 0;
}
void * Enj_HashmapGet(Enj_Hashmap *m, const char *k){
    size_t hash = hash_str(k);
    size_t idx = hash & ((1<<m->logcap)-1);

    for(size_t i = 0; i < 1<<m->logcap; i++){
        size_t ibuf = (i+idx) & ((1<<m->logcap)-1);
        char *ki = ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].key;
        size_t hi = ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].hash;

        if(!ki){
            return NULL;
        }
        if((hi == hash && strcmp(ki, k) == 0){
            return ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].value;
        }
    }
    return NULL;
}

void hashmap_rawput(struct Enj_Hashmap_Node_ *buf,
                    size_t logcap,
                    const char *k,
                    void *v){

    size_t hash = hash_str(k);
    size_t idx = hash & ((1<<logcap)-1);

    for(size_t i = 0; i < 1<<logcap; i++){
        size_t ibuf = (i+idx) & ((1<<logcap)-1);

        if(!buf[ibuf].key){
            buf[ibuf].key = k;
            buf[ibuf].value = v;
            buf[ibuf].hash = hash;

            return;
        }
        else if(hash == buf[ibuf].hash && strcmp(k, buf[ibuf].key) == 0){
            free(buf[ibuf].key);
            buf[ibuf].key = k;
            buf[ibuf].value = v;
            buf[ibuf].hash = hash;

            return;
        }
    }
}

int Enj_HashmapPut(Enj_Hashmap *m, const char *k, void *v){
    if(m->size == (1<<m->logcap) / 4 * 3){
        void *newbuf =
            malloc(2 * (1<<m->logcap) * sizeof(struct Enj_Hashmap_Node_));
        if(!newbuf) return 1;

        for(size_t i = 0; i < 2*(1<<m->logcap); i++){
            ((struct Enj_Hashmap_Node_ *)newbuf)[i].key = NULL;
        }

        //Rehash elements
        for(size_t i = 0; i < 1<<m->logcap; i++){
            char *ki = ((struct Enj_Hashmap_Node_ *)m->data)[i].key;
            if(!ki) continue;

            void *vi = ((struct Enj_Hashmap_Node_ *)m->data)[i].value;
            hashmap_rawput(
                (struct Enj_Hashmap_Node_ *)newbuf,
                m->logcap+1,
                ki,
                vi
            );
        }
        free(m->data);
        m->data = newbuf;
        m->logcap++;
    }


    size_t slen = strlen(k);
    char *sbuf = (char *)malloc(slen+1);
    if(!sbuf) return 1;

    memcpy(sbuf, k, slen+1);

    hashmap_rawput((struct Enj_Hashmap_Node_ *)m->data, m->logcap, sbuf, v);
    m->size++;
    return 0;
}


void Enj_HashmapRemove(Enj_Hashmap *m, const char *k){
    size_t hash = hash_str(k);
    size_t idx = hash & ((1<<m->logcap)-1);

    //Target index to remove
    size_t ilast;

    size_t i;
    for(i = 0; i < 1<<m->logcap; i++){
        size_t ibuf = (i+idx) & ((1<<m->logcap)-1);
        char *ki = ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].key;
        if(!ki) return;
        size_t hi = ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].hash;


        if(hi == hash && strcmp(ki, k) == 0){
            ilast = ibuf;
            i++;
            goto find_removable;
        }
    }
    return;

find_removable:

    for(; i < 1<<m->logcap; i++){
        size_t ibuf = (i+idx) & ((1<<m->logcap)-1);
        char *ki = ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].key;

        //Found empty node, end of hash chain
        if(!ki) break;
        size_t hi = ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].hash;

        if(hi == hash){
            ilast = ibuf;
        }
    }
    //Replace entry with last hash match in chain
    free(((struct Enj_Hashmap_Node_ *)m->data)[ibuf].key);

    ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].key =
        ((struct Enj_Hashmap_Node_ *)m->data)[ilast].key;
    ((struct Enj_Hashmap_Node_ *)m->data)[ibuf].value =
        ((struct Enj_Hashmap_Node_ *)m->data)[ilast].value;

    ((struct Enj_Hashmap_Node_ *)m->data)[ilast].key = NULL;
}
