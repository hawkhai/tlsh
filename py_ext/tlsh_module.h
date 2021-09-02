#include <new>
#include <string>
#include "tlsh.h"

// to generate the "T1" hashes introduced in TLSH 4.0.0
// see 4.0.0 from 26/Mar/2020 at https://github.com/trendmicro/tlsh/blob/master/Change_History.md
#define SHOWVERSION	1

static std::string eval_tlsh(unsigned char* pBuffer, unsigned int len, int showvers);
static std::string hash_py(unsigned char* pBuffer, unsigned int len);
static std::string oldhash_py(unsigned char* pBuffer, unsigned int len);
static std::string forcehash_py(unsigned char* pBuffer, unsigned int len);
static std::string eval_cons_tlsh(unsigned char* pBuffer, unsigned int len, int showvers);
static std::string conservativehash_py(unsigned char* pBuffer, unsigned int len);
static std::string oldconservativehash_py(unsigned char* pBuffer, unsigned int len);

static int diff_py(char* hash1, char* hash2);
static int diffxlen_py(char* hash1, char* hash2);

typedef struct {
    unsigned short required_data;
    bool finalized;
    Tlsh tlsh;
} tlsh_TlshObject;

static tlsh_TlshObject* Tlsh_create(const char* str, unsigned int len);
static tlsh_TlshObject* Tlsh_fromTlshStr(const char* str = nullptr, unsigned int len = 0);
static void Tlsh_release(tlsh_TlshObject* self);

void Tlsh_update(tlsh_TlshObject* self, const char* str, unsigned int len);
void Tlsh_final(tlsh_TlshObject* self);
std::string Tlsh_hexdigest(tlsh_TlshObject* self);

int Tlsh_diff(tlsh_TlshObject* self, char* str, unsigned int len);
int Tlsh_diff(tlsh_TlshObject* self, tlsh_TlshObject* other_tlsh);
int Tlsh_lvalue(tlsh_TlshObject* self);
int Tlsh_q1ratio(tlsh_TlshObject* self);
int Tlsh_q2ratio(tlsh_TlshObject* self);
bool Tlsh_is_valid(tlsh_TlshObject* self);
int Tlsh_checksum(tlsh_TlshObject* self, int id);
int Tlsh_bucket_value(tlsh_TlshObject* self, int id);
