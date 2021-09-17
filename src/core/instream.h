#pragma once
#ifdef __cplusplus
extern "C" {
#endif
//On read - args are: out buffer, max # bytes to read, # bytes left, userdata
// returns # bytes actually read
typedef struct Enj_Instream{
    void (*onfree)(void *);

    size_t (*onread)(void *, size_t, size_t *, void *);
    void *data;
    size_t bytes;
} Enj_Instream;

size_t Enj_ReadBytes(Enj_Instream *is, void *buf, size_t n);

int Enj_InitInstreamFromFile(Enj_Instream *is, const char *file);

int Enj_InitInstreamFromMemory(Enj_Instream *is, const void *buf, size_t n);

void Enj_FreeInstream(Enj_Instream *is);

#ifdef __cplusplus
}
#endif
