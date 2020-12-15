// 14-DockingData.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <bitset>
#include <stdint.h>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

const char* c_maskLinePrefix = "mask = ";
const char* c_memLinePrefix = "mem[";

string m_maskString;

map<uint64_t, uint64_t> m_valueMaskMemoryMap;
map<uint64_t, uint64_t> m_addressMaskMemoryMap;

void WriteToMemoryWithValueMask(uint64_t address, uint64_t value, bool print)
{
	// Add the element to the map for the address if it doesn't already exist
	auto findResult = m_valueMaskMemoryMap.find(address);
	if (findResult == m_valueMaskMemoryMap.end())
		m_valueMaskMemoryMap.emplace(address, 0);

	bitset<36> binaryVal = bitset<36>(value);
	// Iterate over each value bit
	for (int i = 0; i < binaryVal.size(); ++i)
	{
		char maskChar = m_maskString[binaryVal.size() - (i + 1)];

		// If '1' - bit will always be 1
		if (maskChar == '1')
			binaryVal[i] = true;

		// If '0' - bit will always be 0
		if (maskChar == '0')
			binaryVal[i] = false;
	}

	// Get result after running through the mask
	uint64_t maskedValue = binaryVal.to_ullong();

	if (print)
	{
		cout << "===================== MEM ADDRESS " << address << " =====================" << endl;
		cout << "value: \t\t" << bitset<36>(value) << "\t(decimal " << value << ")" << endl;
		cout << "mask: \t\t" << m_maskString << endl;
		cout << "result: \t" << binaryVal << "\t(decimal " << maskedValue << ")" << endl << endl;
	}

	// Set the value in memory
	m_valueMaskMemoryMap[address] = maskedValue;
}

// Retrieve total of all values in memory for those set with masked values
uint64_t GetSumOfValueMaskMemory()
{
	uint64_t sum = 0;
	for (auto iter = m_valueMaskMemoryMap.begin(); iter != m_valueMaskMemoryMap.end(); ++iter)
	{
		sum += iter->second;
	}
	return sum;
}

void WriteToMemoryWithAddressMask(uint64_t address, uint64_t value, bool print)
{
	vector<int> floatingBits; // Stores which place bits are "floating" - meaning they exist as both 0 and 1
	bitset<36> binaryAddress = bitset<36>(address);
	bitset<36> binaryVal = bitset<36>(value);
	// Iterate over the address bits
	for (int i = 0; i < binaryAddress.size(); ++i)
	{
		char maskChar = m_maskString[binaryAddress.size() - (i + 1)];
		if (maskChar == '1')
		{
			// If '1' - bit will always be 1
			binaryAddress[i] = true;
		}
		else if (maskChar == 'X')
		{
			// If 'X' - bit is considered "floating" - it exists as both 0 and 1
			// For now, set it to false - we'll add all the different combinations on top of this
			binaryAddress[i] = false;
			floatingBits.push_back(i);
		}
	}

	// Address with all floating bits set to 0
	uint64_t baseResultAddress = binaryAddress.to_ullong();

	// N = number of floating bits
	// Amount of combinations = N^2
	vector<uint64_t> results((size_t)pow(2, floatingBits.size()));
	for (int i = 0; i < results.size(); ++i)
		results[i] = baseResultAddress;

	// pass 0 - 1 on, 1 off
	// pass 1 - 2 on, 2 off
	// pass 2 - 4 on, 4 off
	// pass 3 - 8 on, 8 off
	// pass N - 2^N on, 2^N off

	// Calculate all possible combinations for floating bits
	for (int i = 0; i < floatingBits.size(); ++i)
	{
		int pattern = (int)pow(2, i); // Calc amount off, amount on
		uint64_t bit = (uint64_t)1 << floatingBits[i]; // What is the value of the floating bit

		for (int j = 0; j < results.size(); ++j)
		{
			int bitFactor = ((j / pattern) % 2) == 1; // Should this bit be 0 or 1
			uint64_t bitValue = bit * bitFactor;
			results[j] += bitValue;
		}
	}

	if (print)
	{
		cout << "===================== VALUE " << value << " =====================" << endl;
		cout << "address: \t" << bitset<36>(address) << "\t(decimal " << address << ")" << endl;
		cout << "mask: \t\t" << m_maskString << endl;
		cout << "b. result: \t" << bitset<36>(baseResultAddress) << "\t(decimal " << baseResultAddress << ")" << endl << endl;
		for (int i = 0; i < results.size(); ++i)
			cout << "results[" << i << "]: \t" << bitset<36>(results[i]) << "\t(decimal " << results[i] << ")" << endl;
	}

	// Write results back to address memory map
	for (uint64_t result : results)
	{
		if (m_addressMaskMemoryMap.find(result) == m_addressMaskMemoryMap.end())
			m_addressMaskMemoryMap.emplace(result, 0);
		
		m_addressMaskMemoryMap[result] = value;
	}
}

// Retrieve total of all values in memory for those set with masked addresses
uint64_t GetSumOfAddressMaskMemory()
{
	uint64_t sum = 0;
	for (auto iter = m_addressMaskMemoryMap.begin(); iter != m_addressMaskMemoryMap.end(); ++iter)
	{
		sum += iter->second;
	}
	return sum;
}

int main()
{
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			if (line.find(c_maskLinePrefix) == 0)
			{
				// mask line
				m_maskString = line.substr(strlen(c_maskLinePrefix));
			}
			else if (line.find(c_memLinePrefix) == 0)
			{
				int memAddress = 0;
				int memValue = 0;

				// mem set line
				int startMemAddress = (int)line.find('[') + 1;
				int endMemAddress = (int)line.find(']');
				memAddress = stoi(line.substr(startMemAddress, (size_t)(endMemAddress - startMemAddress)));

				int startMemValue = (int)line.find('=') + 2;
				memValue = stoi(line.substr(startMemValue));

				// For Part One
				// Set memory value using the mask on the value
				WriteToMemoryWithValueMask(memAddress, memValue, false);

				// For Part Two
				// Set memory address using the mask on the value
				//WriteToMemoryWithAddressMask(memAddress, memValue, false);
			}
		}
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		uint64_t memorySum = GetSumOfValueMaskMemory();
		cout << "The sum of all memory addresses when using the mask for the value is: " << endl << memorySum << endl << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		uint64_t memorySum = GetSumOfAddressMaskMemory();
		cout << "The sum of all memory addresses when using the mask for the memory address is: " << endl << memorySum << endl << endl;
	}
}