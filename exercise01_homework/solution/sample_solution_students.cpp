#include <cstring>
#include <iostream>
#include "vv-aes.h"
unsigned char swamp[256];
unsigned char messages[16];
unsigned char tmp;
unsigned char tmp2;
unsigned char m[16];
inline void shift_rows() {
    messages[0] = swamp[messages[0]];
    messages[1] = swamp[messages[1]];
    messages[2] = swamp[messages[2]];
    messages[3] = swamp[messages[3]];
    tmp = swamp[messages[4]];
    messages[4] = swamp[messages[5]];
    messages[5] = swamp[messages[6]];
    messages[6] = swamp[messages[7]];
    messages[7] = tmp;
    tmp = swamp[messages[8]];
    tmp2 = swamp[messages[9]];
    messages[8] = swamp[messages[10]];
    messages[10] = tmp;
    messages[9] = swamp[messages[11]];
    messages[11] = tmp2;
    tmp = swamp[messages[15]];
    messages[15] = swamp[messages[14]];
    messages[14] = swamp[messages[13]];
    messages[13] = swamp[messages[12]];
    messages[12] = tmp;

}
inline void mix_columns() {
    m[4] = messages[4] * messages[4];
    m[5] = messages[5] * messages[5];
    m[6] = messages[6] * messages[6];
    m[7] = messages[7] * messages[7];
    m[8] = messages[8] * messages[8] * messages[8];
    m[9] = messages[9] * messages[9] * messages[9];
    m[10] = messages[10] * messages[10] * messages[10];
    m[11] = messages[11] * messages[11] * messages[11];
    m[12] = messages[12] * messages[12] * messages[12] * messages[12];
    m[13] = messages[13] * messages[13] * messages[13] * messages[13];
    m[14] = messages[14] * messages[14] * messages[14] * messages[14];
    m[15] = messages[15] * messages[15] * messages[15] * messages[15];
    messages[0] = 3* messages[0] + 2* m[4] + 3* m[8] + 2* m[12];
    messages[1] = 3* messages[1] + 2* m[5] + 3* m[9] + 2* m[13];
    messages[2] = 3* messages[2] + 2* m[6] + 3* m[10] + 2* m[14];
    messages[3] = 3* messages[3] + 2* m[7] + 3* m[11] + 2*m[15];
    messages[4] = 2* messages[0] + 2* m[4] + 2* m[8] + m[12];
    messages[5] = 2* messages[1] + 2* m[5] + 2* m[9] + m[13];
    messages[6] = 2* messages[2] + 2* m[6] + 2* m[10] +  m[14];
    messages[7] = 2* messages[3] + 2* m[7] + 2* m[11] +  m[15];
    messages[8] =  messages[0] + 2* messages[4] * messages[4] + 3* m[8] + 2* m[12];
    messages[9] =  messages[1] + 2* messages[5] * messages[5] + 3* m[9] + 2* m[13];
    messages[10] =  messages[2] + 2* messages[6] * messages[6] + 3* m[10] + 2* m[14];
    messages[11] =  messages[3] + 2* messages[7] * messages[7] + 3* m[11] + 2* m[15];
    messages[12] =  messages[0] + 2* messages[4] * messages[4] + 2* messages[8] * messages[8] * messages[8] + m[12];
    messages[13] =  messages[1] + 2* messages[5] * messages[5] + 2* messages[9] * messages[9] * messages[9] + m[13];
    messages[14] =  messages[2] + 2* messages[6] * messages[6] + 2* messages[10] * messages[10] * messages[10] + m[14];
    messages[15] =  messages[3] + 2* messages[7] * messages[7] + 2* messages[11] * messages[11] * messages[11] + m[15];
}

inline void add_key() {
    messages[0] = messages[0] ^ key[0][0];
    messages[1] = messages[1] ^ key[0][1];
    messages[2] = messages[2] ^ key[0][2];
    messages[3] = messages[3] ^ key[0][3];
    messages[4] = messages[4] ^ key[1][0];
    messages[5] = messages[5] ^ key[1][1];
    messages[6] = messages[6] ^ key[1][2];
    messages[7] = messages[7] ^ key[1][3];
    messages[8] = messages[8] ^ key[2][0];
    messages[9] = messages[9] ^ key[2][1];
    messages[10] = messages[10] ^ key[2][2];
    messages[11] = messages[11] ^ key[2][3];
    messages[12] = messages[12] ^ key[3][0];
    messages[13] = messages[13] ^ key[3][1];
    messages[14] = messages[14] ^ key[3][2];
    messages[15] = messages[15] ^ key[3][3];
}
int main() {
    for (int i = 0; i < 256; i++) {
        swamp[originalCharacter[i]] = substitutedCharacter[i];
    }
    for (int i = 0; i < 16; i++) {
        messages[i]=message[i/4][i%4];
    }
    readInput();
    for (int i = 0; i < 500000; i++) {
            set_next_key();
            add_key();
            set_next_key();
            shift_rows();
            mix_columns();
            add_key();
            set_next_key();

            shift_rows();
            mix_columns();
            add_key();
            set_next_key();

            shift_rows();
            mix_columns();
            add_key();
            set_next_key();

            shift_rows();
            mix_columns();
            add_key();
            set_next_key();

            shift_rows();
            mix_columns();
            add_key();
            set_next_key();

            shift_rows();
            mix_columns();
            add_key();
            set_next_key();

            shift_rows();
            mix_columns();
            add_key();
            set_next_key();

            shift_rows();
            mix_columns();
            add_key();
            set_next_key();

            shift_rows();
            mix_columns();
            add_key();

            shift_rows();
            add_key();
    }
    for (int i = 0; i < 16; i++) {
        message[i / 4][i % 4] = messages[i];
    }
    writeOutput();
    return 0;
}


