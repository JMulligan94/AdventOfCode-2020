// 09-EncodingError.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

const int c_preambleLength = 25;
const int c_numPreviousConsidered = 25;

int main()
{
	vector<long long> m_numbers;
	// Read input.txt and grab the relevant info
	ifstream inputStream("input.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			m_numbers.push_back(stoull(line));
		}
	}

	int invalidIndex = -1;
	long long invalidNumber = -1;
	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		// Find invalid index and number
		for (int i = c_preambleLength; i < m_numbers.size(); ++i)
		{
			bool foundMatch = false;
			long long num_i = m_numbers[i];
			for (int j = i - c_numPreviousConsidered; j < i; ++j)
			{
				long long num_j = m_numbers[j];
				for (int k = i - c_numPreviousConsidered; k < i; ++k)
				{
					if (j == k)
						continue;

					long long num_k = m_numbers[k];
					if ((num_j + num_k) == num_i)
					{
						foundMatch = true;
					}
				}
			}

			if (!foundMatch)
			{
				invalidIndex = i;
				invalidNumber = m_numbers[invalidIndex];
				break;
			}
		}
		cout << "Found no matches for index " << invalidIndex << " - " << invalidNumber << endl << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		
		// Find encryption weakness contiguous set adding up to the invalidNumber
		int startIndex = -1;
		int endIndex = -1;
		bool foundMatch = false;
		for (int i = 1; i < m_numbers.size(); ++i)
		{
			vector<long long> runningTotals(m_numbers.size());
			// Keep vector of running totals for sum of each index range of j - i
			// i.e. runningTotals[2] = 2+3+4+5+...+i
			// i.e. runningTotals[5] = 5+6+7+...+i
			for (int j = 0; j < i; ++j)
			{
				long long num_j = m_numbers[j];

				// Add this number running backwards through the running totals and see if we hit the invalid number
				int backwards = j;
				while (backwards >= 0)
				{
					runningTotals[backwards] += num_j;
					// Check if this is the contiguous set we're looking for yet
					if (runningTotals[backwards] == invalidNumber)
					{
						// It IS the contiguous set we're looking for
						// Cache the start and end indices of the set!
						startIndex = backwards;
						endIndex = j;
						cout << "Found contiguous set!" << endl;
						cout << "Numbers at indices " << backwards << " - " << j << " sum up to " << invalidNumber << endl << endl;
						foundMatch = true;
						break;
					}
					backwards--;
				}
			}

			if (foundMatch)
				break;
		}

		if (foundMatch)
		{
			// Double check that this range adds up to the invalid number
			// while caching the smallest and largest number
			long long sum = 0;
			long long smallestNum = LLONG_MAX;
			long long largestNum = LLONG_MIN;
			for (int i = startIndex; i < endIndex + 1; ++i)
			{
				long long currentNum = m_numbers[i];
				sum += currentNum;

				if (currentNum > largestNum)
					largestNum = currentNum;

				if (currentNum < smallestNum)
					smallestNum = currentNum;
			}

			if (sum == invalidNumber)
			{
				// It does add up!
				cout << "The smallest number is: " << smallestNum << endl;
				cout << "The largest number is: " << largestNum << endl;
				cout << "The sum of these numbers is: " << (smallestNum + largestNum) << endl << endl;
			}
		}
	}
}
