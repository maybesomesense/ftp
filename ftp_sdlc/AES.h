#pragma once
//Advanced Encryption Standard//

class AES {
	private:
		static constexpr unsigned int numberBytes = 4;
		static constexpr unsigned int numberRows = 4;
		unsigned int blockBytesLen = numberBytes * numberRows * sizeof(unsigned char);

		unsigned int lengthKey = 4;
		unsigned int numberRoundes = 10;
		
		//Useful functions
		void CheckLength(unsigned int len);
		void KeyExpansion(const unsigned char key[], unsigned char w[]);
		void XorBlocks(const unsigned char* a, const unsigned char* b, unsigned char* c, unsigned int len);

		//Function for encryption
		void SubBytes(unsigned char state[numberBytes][numberRows]);
		void MixColumns(unsigned char state[numberBytes][numberRows]);
		void ShiftRow(unsigned char state[numberBytes][numberRows], unsigned int i, unsigned int n);  
		void ShiftRows(unsigned char state[numberBytes][numberRows]);
		void AddRoundKey(unsigned char state[numberBytes][numberRows], unsigned char* key);
		void EncryptBlock(const unsigned char in[], unsigned char out[], unsigned char* roundKeys);

		//Function for decryption
		void InversionSubBytes(unsigned char state[numberBytes][numberRows]);
		void InversionMixColumns(unsigned char state[numberBytes][numberRows]);
		void InversionShiftRows(unsigned char state[numberBytes][numberRows]);
		void DecryptBlock(const unsigned char in[], unsigned char out[], unsigned char* roundKeys);
		
		unsigned char* EncryptECB(const unsigned char message[], unsigned int inLength, const unsigned char key[]);
		unsigned char* DecryptECB(const unsigned char message[], unsigned int inLength, const unsigned char key[]);

	public:
		AES(const int keyLength = 128);

		unsigned char* Encrypt(const unsigned char message[]);
		unsigned char* Decrypt(const unsigned char message[]);

		void printHexArray(unsigned char a[], unsigned int n);
};