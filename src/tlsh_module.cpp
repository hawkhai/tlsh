#include "tlsh.h"
#include "tlsh_module.h"

namespace tlsh {

    std::string eval_tlsh(unsigned char* pBuffer, unsigned int len, int showvers)
    {
        Tlsh tlsh;
        tlsh.update(pBuffer, len);
        tlsh.final();
        const char* s = tlsh.getHash(showvers);
        if (*s == '\0')
            return "TNULL";
        return s;
    }

    std::string hash_py(unsigned char* pBuffer, unsigned int len)
    {
        return (eval_tlsh(pBuffer, len, SHOWVERSION));
    }

    std::string oldhash_py(unsigned char* pBuffer, unsigned int len) {
        return (eval_tlsh(pBuffer, len, 0));
    }

    std::string forcehash_py(unsigned char* pBuffer, unsigned int len)
    {
        return (hash_py(pBuffer, len));
    }

    std::string eval_cons_tlsh(unsigned char* pBuffer, unsigned int len, int showvers)
    {
        Tlsh tlsh;
        tlsh.update(pBuffer, len);
        tlsh.final(NULL, 0, 2);
        const char* s = tlsh.getHash(showvers);
        if (*s == '\0')
            return "TNULL";
        return s;
    }

    std::string conservativehash_py(unsigned char* pBuffer, unsigned int len)
    {
        return (eval_cons_tlsh(pBuffer, len, SHOWVERSION));
    }

    std::string oldconservativehash_py(unsigned char* pBuffer, unsigned int len)
    {
        return (eval_cons_tlsh(pBuffer, len, 0));
    }

    int diff_py(const char* hash1, const char* hash2) {
        Tlsh tlsh1, tlsh2;
        if (tlsh1.fromTlshStr(hash1) != 0) {
            throw "argument %s is not a TLSH hex string";
        }
        if (tlsh2.fromTlshStr(hash2) != 0) {
            throw "argument %s is not a TLSH hex string";
        }

        int score = tlsh1.totalDiff(&tlsh2);
        return score;
    }

    int diffxlen_py(const char* hash1, const char* hash2) {
        Tlsh tlsh1, tlsh2;
        if (tlsh1.fromTlshStr(hash1) != 0) {
            throw "argument %s is not a TLSH hex string";
        }
        if (tlsh2.fromTlshStr(hash2) != 0) {
            throw "argument %s is not a TLSH hex string";
        }
        int score = tlsh1.totalDiff(&tlsh2, false);
        return score;
    }

    tlsh_TlshObject* Tlsh_create(const char* str, unsigned int len)
    {
        tlsh_TlshObject* tlsh_object = new tlsh_TlshObject();

        /* Call Tlsh() constructor. */
        // new (&tlsh_object->tlsh) Tlsh();
        tlsh_object->required_data = 0;
        tlsh_object->finalized = false;

        if (str && len) {
            Tlsh_update(tlsh_object, str, len);
        }
        return tlsh_object;
    }

    void Tlsh_release(tlsh_TlshObject* self) {
        delete self;
    }

    tlsh_TlshObject* Tlsh_fromTlshStr(const char* str, unsigned int len)
    {
        if ((len != TLSH_STRING_LEN_REQ) && (len != TLSH_STRING_LEN_REQ - 2)) {
            throw "argument length incorrect: not a TLSH hex string";
            return NULL;
        }

        tlsh_TlshObject* self = Tlsh_create(str, len);

        if (self->tlsh.fromTlshStr(str) != 0) {
            delete self;
            throw "argument value incorrect: not a TLSH hex string";
            return NULL;
        }
        self->finalized = true;
        return self;
    }

    void Tlsh_update(tlsh_TlshObject* self, const char* str, unsigned int len)
    {
        if (self->finalized) {
            throw "final() has already been called";
            return;
        }
        if (self->required_data < MIN_DATA_LENGTH) {
            self->required_data += len > MIN_DATA_LENGTH ? MIN_DATA_LENGTH : len;
        }
        self->tlsh.update((unsigned char*)str, (unsigned int)len);
    }

    void Tlsh_final(tlsh_TlshObject* self)
    {
        if (self->finalized) {
            throw "final() has already been called";
            return;
        }
        if (self->required_data < MIN_DATA_LENGTH) {
            throw "less than %u of input";
            return;
        }
        self->finalized = true;
        self->tlsh.final();
    }

    std::string Tlsh_hexdigest(tlsh_TlshObject* self)
    {
        char hash[TLSH_STRING_LEN_REQ + 1];
        if (!self->finalized) {
            throw "final() has not been called";
            return NULL;
        }
        self->tlsh.getHash(hash, TLSH_STRING_LEN_REQ + 1, SHOWVERSION);
        if (hash[0] == '\0') {
            throw "error while getting hash (not enough variation in input?)";
            return NULL;
        }
        return hash;
    }

    int Tlsh_diff(tlsh_TlshObject* self, char* str, unsigned int len)
    {
        int score;

        Tlsh other;
        if ((len != TLSH_STRING_LEN_REQ) && (len != TLSH_STRING_LEN_REQ - 2)) {
            throw "argument is not a TLSH hex string";
            return NULL;
        }
        if (other.fromTlshStr(str) != 0) {
            throw "argument is not a TLSH hex string";
            return NULL;
        }
        score = self->tlsh.totalDiff(&other);
        return score;
    }

    int Tlsh_diff(tlsh_TlshObject* self, tlsh_TlshObject* other_tlsh)
    {
        int score;
        score = self->tlsh.totalDiff(&other_tlsh->tlsh);
        return score;
    }

    int Tlsh_lvalue(tlsh_TlshObject* self)
    {
        if (!self->finalized) {
            throw "final() has not been called";
            return NULL;
        }
        return self->tlsh.Lvalue();
    }

    int Tlsh_q1ratio(tlsh_TlshObject* self)
    {
        if (!self->finalized) {
            throw "final() has not been called";
            return NULL;
        }
        return self->tlsh.Q1ratio();
    }

    int Tlsh_q2ratio(tlsh_TlshObject* self)
    {
        if (!self->finalized) {
            throw "final() has not been called";
            return NULL;
        }
        return self->tlsh.Q2ratio();
    }

    bool Tlsh_is_valid(tlsh_TlshObject* self)
    {
        return self->tlsh.isValid();
    }

    int Tlsh_checksum(tlsh_TlshObject* self, int id)
    {
        if (!self->finalized) {
            throw "final() has not been called";
            return NULL;
        }
        return self->tlsh.Checksum(id);
    }

    int Tlsh_bucket_value(tlsh_TlshObject* self, int id)
    {
        if (!self->finalized) {
            throw "final() has not been called";
            return NULL;
        }
        return self->tlsh.BucketValue(id);
    }

}
