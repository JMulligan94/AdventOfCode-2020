// 25-ComboBreaker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdint.h>

//#define DEBUG_PRINT

using namespace std;


const int c_initialSubjectNumber = 7;
const int c_divisionValue = 20201227;

class HandshakeParticipant
{
public:
	HandshakeParticipant(uint64_t publicKey)
		: m_publicKey(publicKey)
	{
	}

	uint64_t GetPublicKey() const { return m_publicKey; }

	uint64_t FindLoopSize() const
	{
		/*
		The handshake used by the card and the door involves an operation that transforms a subject number. To transform a subject number, start with the value 1. Then, a number of times called the loop size, perform the following steps:

			- Set the value to itself multiplied by the subject number.
			- Set the value to the remainder after dividing the value by 20201227.
		*/

		uint64_t subjectNumber = c_initialSubjectNumber;

		uint64_t loopSize = 0;
		uint64_t value = 1;
		while (value != m_publicKey)
		{
			loopSize++;

			// Set the value to itself multiplied by the subject number.
			value *= subjectNumber;

			// Set the value to the remainder after dividing the value by 20201227.
			value = (value % c_divisionValue);

#ifdef DEBUG_PRINT
			cout << "Loop " << loopSize << ": " << value << endl;
#endif
		}
		return loopSize;
	}

	// Calculate encyrption key by transforming the public key by the other participant's loop size
	uint64_t GetEncryptionKey(uint64_t otherLoopSize) const
	{
		uint64_t subjectNumber = m_publicKey;
		uint64_t value = 1;
		for (uint64_t i = 0; i < otherLoopSize; ++i)
		{
			// Set the value to itself multiplied by the subject number.
			value *= subjectNumber;

			// Set the value to the remainder after dividing the value by 20201227.
			value = (value % c_divisionValue);
		}
		return value;
	}

private:
	uint64_t m_publicKey;
};

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

int main()
{
	HandshakeParticipant* card = nullptr;
	HandshakeParticipant* door = nullptr;

	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;

		// Get card public key
		getline(inputStream, line);
		card = new HandshakeParticipant(stoi(line));

		// Get door public key
		getline(inputStream, line);
		door = new HandshakeParticipant(stoi(line));
	}

	uint64_t cardLoopSize = card->FindLoopSize();
	cout << endl << "Card's secret loop size = " << cardLoopSize << endl << endl;

	uint64_t doorLoopSize = door->FindLoopSize();
	cout << endl << "Door's secret loop size = " << doorLoopSize << endl << endl;

	uint64_t cardEncryptionKey = card->GetEncryptionKey(doorLoopSize);
	cout << endl << "Card's encryption key = " << cardEncryptionKey << endl << endl;

	uint64_t doorEncryptionKey = door->GetEncryptionKey(cardLoopSize);
	cout << endl << "Door's encryption key = " << doorEncryptionKey << endl << endl;

	// Delete ptrs
	delete card;
	delete door;

	card = nullptr;
	door = nullptr;
}