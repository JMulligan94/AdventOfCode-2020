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
const char* c_testFile = "Test2.txt";

const char* c_maskLinePrefix = "mask = ";
const char* c_memLinePrefix = "mem[";

string m_maskString;

map<uint64_t, uint64_t> m_valueMaskMemoryMap;
map<uint64_t, uint64_t> m_addressMaskMemoryMap;

void WriteToMemoryWithValueMask(uint64_t address, uint64_t value, bool print)
{
	auto findResult = m_valueMaskMemoryMap.find(address);
	if (findResult == m_valueMaskMemoryMap.end())
		m_valueMaskMemoryMap.emplace(address, 0);

	uint64_t addressVal = m_valueMaskMemoryMap[address];
	bitset<36> binaryVal = bitset<36>(value);
	bitset<36> binaryResult = bitset<36>(addressVal);
	for (int i = 0; i < binaryResult.size(); ++i)
	{
		binaryResult[i] = binaryVal[i];

		char maskChar = m_maskString[binaryResult.size() - (i + 1)];
		if (maskChar == '1')
			binaryResult[i] = true;

		if (maskChar == '0')
			binaryResult[i] = false;
	}

	uint64_t result = binaryResult.to_ullong();

	if (print)
	{
		cout << "===================== MEM ADDRESS " << address << " =====================" << endl;
		cout << "value: \t\t" << binaryVal << "\t(decimal " << value << ")" << endl;
		cout << "mask: \t\t" << m_maskString << endl;
		cout << "result: \t" << binaryResult << "\t(decimal " << result << ")" << endl << endl;
	}

	m_valueMaskMemoryMap[address] = result;
}

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
	vector<int> floatingBits;
	bitset<36> binaryAddress = bitset<36>(address);
	bitset<36> binaryVal = bitset<36>(value);
	for (int i = 0; i < binaryAddress.size(); ++i)
	{
		char maskChar = m_maskString[binaryAddress.size() - (i + 1)];
		if (maskChar == '1')
			binaryAddress[i] = true;
		else if (maskChar == 'X')
		{
			binaryAddress[i] = false;
			floatingBits.push_back(i);
		}
	}

	uint64_t baseResultAddress = binaryAddress.to_ullong();

	vector<uint64_t> results((size_t)pow(2, floatingBits.size()));
	for (int i = 0; i < results.size(); ++i)
		results[i] = baseResultAddress;

	// 0 - 1 on, 1 off
	// 1 - 2 on, 2 off
	// 2 - 4 on, 4 off
	// 3 - 8 on, 8 off
	// N - pow(2^N) on, pow(2^N) off

	// pass 0
	// 0 - 0 0
	// 1 - 0 1
	// 2 - 0 0
	// 3 - 0 1

	// pass 1
	// 0 - 0 0
	// 1 - 0 1
	// 2 - 1 0
	// 3 - 1 1

	for (int i = 0; i < floatingBits.size(); ++i)
	{
		int interval = (int)pow(2, i);
		uint64_t bit = (uint64_t)1 << floatingBits[i];
		int bitFactor = 0;
		int intervalCount = interval;
		for (int j = 0; j < results.size(); ++j)
		{
			uint64_t bitValue = bit * bitFactor;
			results[j] += bitValue;

			intervalCount--;
			if (intervalCount == 0)
			{
				bitFactor = bitFactor == 0 ? 1 : 0;
				intervalCount = interval;
			}
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

	for (uint64_t result : results)
	{
		if (m_addressMaskMemoryMap.find(result) == m_addressMaskMemoryMap.end())
			m_addressMaskMemoryMap.emplace(result, 0);
		
		m_addressMaskMemoryMap[result] = value;
	}
}

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

				WriteToMemoryWithValueMask(memAddress, memValue, false);

				WriteToMemoryWithAddressMask(memAddress, memValue, false);
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