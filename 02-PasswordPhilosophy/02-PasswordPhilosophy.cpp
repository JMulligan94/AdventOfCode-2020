// 02-PasswordPhilosophy.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

struct PasswordEntry
{
	PasswordEntry(int a, int b, char token, string password)
	{
		m_a = a;
		m_b = b;
		m_token = token;
		m_password = password;
	};
	
	int m_a;
	int m_b;
	char m_token;
	string m_password;
};

int GetNumberValidPasswordsOld(const vector<PasswordEntry>& entries)
{
	// Password must contain a number of specified char inside the given range A-B
	int numValid = 0;
	for (auto iter = entries.begin(); iter != entries.end(); ++iter)
	{
		PasswordEntry pwEntry = *iter;
		string password = pwEntry.m_password;

		int numInstances = 0;
		for (int i = 0; i < (int)password.size(); ++i)
		{
			if (password.at(i) == pwEntry.m_token)
			{
				numInstances++;
			}
		}

		if (numInstances >= pwEntry.m_a
			&& numInstances <= pwEntry.m_b)
		{
			numValid++;
		}
	}
	return numValid;
}

int GetNumberValidPasswordsNew(const vector<PasswordEntry>& entries)
{
	// Password must contain the letter in either index A or B - but not both!
	int numValid = 0;
	for (auto iter = entries.begin(); iter != entries.end(); ++iter)
	{
		PasswordEntry pwEntry = *iter;
		string password = pwEntry.m_password;

		if ((password.at(pwEntry.m_a - 1) == pwEntry.m_token)
			^ (password.at(pwEntry.m_b - 1) == pwEntry.m_token)) // XOR
		{
			numValid++;
		}
	}
	return numValid;
}

int main()
{
	vector<PasswordEntry> passwords;

	ifstream inputStream("input.txt");
	if (inputStream.is_open())
	{
		string line;
		while (getline(inputStream, line))
		{
			// Split line into range - char -  password
			int spaceIndex = line.find(' ');
			int passwordIndex = line.find(':') + 2;

			// Get two integers from range string
			string rangeString = line.substr(0, spaceIndex);
			int dashIndex = rangeString.find('-');
			int a = stoi(rangeString.substr(0, dashIndex));
			int b = stoi(rangeString.substr(dashIndex + 1, rangeString.size() - (dashIndex + 1)));

			// Get char token
			char token = line.at(spaceIndex + 1);

			// Get password string
			string pwString = line.substr(passwordIndex, line.size() - passwordIndex);

			// Add password entry
			PasswordEntry pwEntry(a, b, token, pwString);
			passwords.push_back(pwEntry);
		}
	}

	{
		// Part One
		int numValid = GetNumberValidPasswordsOld(passwords);

		cout << "=== Part 1 ===" << endl;
		cout << "The number of valid passwords is: " << numValid << endl << endl;
	}

	{
		// Part Two
		int numValid = GetNumberValidPasswordsNew(passwords);

		cout << "=== Part 2 ===" << endl;
		cout << "The number of valid passwords is: " << numValid << endl;
	}
	

}