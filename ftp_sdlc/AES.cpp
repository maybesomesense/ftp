#include "AES.h"
using namespace std;

AES::AES() {};

void AES::generateRandomKey(unsigned char* key, int keyLength) {
    std::random_device rd; //Cоздается объект rd типа std::random_device, который представляет собой устройство генерации случайных чисел.
    std::mt19937 gen(rd()); //Генератор псевдослучайных чисел Mersenne Twister с 32-битным состоянием.
    std::uniform_int_distribution<int> dis(0, 255); //Определяет равномерное распределение целых чисел от 0 до 255.

    for (int i = 0; i < keyLength; ++i) {
        key[i] = static_cast<unsigned char>(dis(gen));
    }
}

void AES::SubWord(unsigned char* a) {
    int i;
    for (i = 0; i < 4; i++) {
        a[i] = sbox[a[i] / 16][a[i] % 16];
    }
}
void AES::RotWord(unsigned char* a) {
    unsigned char c = a[0];
    a[0] = a[1];
    a[1] = a[2];
    a[2] = a[3];
    a[3] = c;
}
void AES::XorWords(unsigned char* a, unsigned char* b, unsigned char* c) {
    int i;
    for (i = 0; i < 4; i++) {
        c[i] = a[i] ^ b[i];
    }
}
unsigned char AES::xtime(unsigned char b)  // multiply on x
{
    return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}
void AES::Rcon(unsigned char* a, unsigned int n) {
    unsigned int i;
    unsigned char c = 1;
    for (i = 0; i < n - 1; i++) {
        c = xtime(c);
    }

    a[0] = c;
    a[1] = a[2] = a[3] = 0;
}
void AES::KeyExpansion(unsigned char w[]) {
    unsigned char temp[4];
    unsigned char rcon[4];

    unsigned int i = 0;
    while (i < 4 * lengthKey) {
        w[i] = key[i];
        i++;
    }

    i = 4 * lengthKey;
    while (i < 4 * numberBytes * (numberRoundes + 1)) {
        temp[0] = w[i - 4 + 0];
        temp[1] = w[i - 4 + 1];
        temp[2] = w[i - 4 + 2];
        temp[3] = w[i - 4 + 3];

        if (i / 4 % lengthKey == 0) {
            RotWord(temp);
            SubWord(temp);
            Rcon(rcon, i / (lengthKey * 4));
            XorWords(temp, rcon, temp);
        }
        else if (lengthKey > 6 && i / 4 % lengthKey == 4) {
            SubWord(temp);
        }

        w[i + 0] = w[i - 4 * lengthKey] ^ temp[0];
        w[i + 1] = w[i + 1 - 4 * lengthKey] ^ temp[1];
        w[i + 2] = w[i + 2 - 4 * lengthKey] ^ temp[2];
        w[i + 3] = w[i + 3 - 4 * lengthKey] ^ temp[3];
        i += 4;
    }
}

void AES::AddRoundKey(unsigned char state[4][numberBytes], unsigned char* key) {
    unsigned int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberBytes; j++) {
            state[i][j] = state[i][j] ^ key[i + 4 * j];
        }
    }
}
void AES::SubBytes(unsigned char state[4][numberBytes]) {
    unsigned int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberBytes; j++) {
            t = state[i][j];
            state[i][j] = sbox[t / 16][t % 16];
        }
    }
}
void AES::ShiftRow(unsigned char state[4][numberBytes], unsigned int i, unsigned int n)  // shift row i on n positions 
{
    unsigned char tmp[numberBytes];
    for (unsigned int j = 0; j < numberBytes; j++) {
        tmp[j] = state[i][(j + n) % numberBytes];
    }
    memcpy(state[i], tmp, numberBytes * sizeof(unsigned char));
}
void AES::ShiftRows(unsigned char state[4][numberBytes]) {
    ShiftRow(state, 1, 1);
    ShiftRow(state, 2, 2);
    ShiftRow(state, 3, 3);
}
void AES::MixColumns(unsigned char state[4][numberBytes]) {
    unsigned char temp_state[4][numberBytes];

    for (size_t i = 0; i < 4; ++i) {
        memset(temp_state[i], 0, 4);
    }

    for (size_t i = 0; i < 4; ++i) {
        for (size_t k = 0; k < 4; ++k) {
            for (size_t j = 0; j < 4; ++j) {
                if (CMDS[i][k] == 1)
                    temp_state[i][j] ^= state[k][j];
                else
                    temp_state[i][j] ^= GF_MUL_TABLE[CMDS[i][k]][state[k][j]];
            }
        }
    }

    for (size_t i = 0; i < 4; ++i) {
        memcpy(state[i], temp_state[i], 4);
    }
}

void AES::AESEncryptBlock(const unsigned char message[], unsigned char cipher[]) {
    unsigned char* roundKeys = new unsigned char[4 * lengthKey * (numberRoundes + 1)];

    KeyExpansion(roundKeys);

    unsigned char state[4][numberBytes];
    unsigned int i, j, round;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberBytes; j++) {
            state[i][j] = message[i + 4 * j];
        }
    }

    AddRoundKey(state, roundKeys);

    for (round = 1; round <= numberRoundes - 1; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, roundKeys + round * 4 * numberBytes);
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, roundKeys + numberRoundes * 4 * numberBytes);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberBytes; j++) {
            cipher[i + 4 * j] = state[i][j];
        }
    }

    delete[] roundKeys;
}
void AES::Encrypt(const unsigned char message[], unsigned char ciphertext[]) {
    generateRandomKey(this->key, 16);
    for (int i = 0; i < 16; i++)
        ciphertext[i] = this->key[i];
    for (int i = 16; i < 1024; i += 16) 
        AESEncryptBlock(message + i - 16, ciphertext + i);
}

void AES::InvSubBytes(unsigned char state[4][numberBytes]) {
    unsigned int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberBytes; j++) {
            t = state[i][j];
            state[i][j] = inv_sbox[t / 16][t % 16];
        }
    }
}
void AES::InvShiftRows(unsigned char state[4][numberBytes]) {
    ShiftRow(state, 1, numberBytes - 1);
    ShiftRow(state, 2, numberBytes - 2);
    ShiftRow(state, 3, numberBytes - 3);
}
void AES::InvMixColumns(unsigned char state[4][numberBytes]) {
    unsigned char temp_state[4][numberBytes];

    for (size_t i = 0; i < 4; ++i) {
        memset(temp_state[i], 0, 4);
    }

    for (size_t i = 0; i < 4; ++i) {
        for (size_t k = 0; k < 4; ++k) {
            for (size_t j = 0; j < 4; ++j) {
                temp_state[i][j] ^= GF_MUL_TABLE[INV_CMDS[i][k]][state[k][j]];
            }
        }
    }

    for (size_t i = 0; i < 4; ++i) {
        memcpy(state[i], temp_state[i], 4);
    }
}

void AES::AESDecryptBlock(const unsigned char in[], unsigned char out[]) {
    unsigned char* roundKeys = new unsigned char[4 * lengthKey * (numberRoundes + 1)];

    KeyExpansion(roundKeys);

    unsigned char state[4][numberBytes];
    unsigned int i, j, round;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberBytes; j++) {
            state[i][j] = in[i + 4 * j];
        }
    }

    AddRoundKey(state, roundKeys + numberRoundes * 4 * numberBytes);

    for (round = numberRoundes - 1; round >= 1; round--) {
        InvSubBytes(state);
        InvShiftRows(state);
        AddRoundKey(state, roundKeys + round * 4 * numberBytes);
        InvMixColumns(state);
    }

    InvSubBytes(state);
    InvShiftRows(state);
    AddRoundKey(state, roundKeys);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberBytes; j++) {
            out[i + 4 * j] = state[i][j];
        }
    }
}
void AES::Decrypt(const unsigned char message[], unsigned char result[]) {
    for (int i = 0; i < 16; i++)
        this->key[i] = message[i];
    for (int i = 16; i < 1024; i += 16)
        AESDecryptBlock(message + i, result + i-16);
}



int test() {
    AES aes;
    unsigned char key[1024] = "Hello world, you are so beautiful\nTHx<3\n";
    unsigned char cipher_text[1024];
    unsigned char result[1024];
    aes.Encrypt(key, cipher_text);
    aes.Decrypt(cipher_text, result);
    for (int i = 0; i < 1024; i++)
        cout << key[i];
    cout << endl;
    for (int i = 16; i < 1024; i++)
        cout << cipher_text[i];
    cout << endl << endl;
    for (int i = 16; i < 1024; i++)
        cout << result[i];
    
    return 0;
}
