// 04-PassportProcessing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>

using namespace std;

enum PassportInfoType
{
	BIRTH_YEAR,
	ISSUE_YEAR,
	EXP_YEAR,
	HEIGHT,
	HAIR_COLOUR,
	EYE_COLOUR,
	PASSPORT_ID,
	COUNTRY_ID
};

struct chr_cmp
{
	bool operator()(const char* a, const char* b) const
	{
		return strcmp(a, b) < 0;
	}
};

map<const char*, PassportInfoType, chr_cmp> m_idToTypeMap;

struct Passport
{
	Passport()
		: m_byr(-1)
		, m_eyr(-1)
		, m_iyr(-1)
	{
	}

	void SetPassportInfo(string keyValuePair)
	{
		int splitter = (int)keyValuePair.find(':');
		string key = keyValuePair.substr(0, splitter);
		string value = keyValuePair.substr(splitter + 1, keyValuePair.size() - (splitter + 1));

		PassportInfoType infoType = m_idToTypeMap[key.c_str()];

		switch (infoType)
		{
			case BIRTH_YEAR:
			{
				m_byr = stoi(value);
			}
			break;
			case ISSUE_YEAR:
			{
				m_iyr = stoi(value);
			}
			break;
			case EXP_YEAR:
			{
				m_eyr = stoi(value);
			}
			break;
			case HEIGHT:
			{
				m_hgt = value;
			}
			break;
			case HAIR_COLOUR:
			{
				m_hcl = value;
			}
			break;
			case EYE_COLOUR:
			{
				m_ecl = value;
			}
			break;
			case PASSPORT_ID:
			{
				m_pid = value;
			}
			break;
			case COUNTRY_ID:
			{
				m_cid = value;
			}
			break;
		}
	}

	bool IsYearValid(int year, int min, int max) const
	{
		// Must be within given range
		return year >= min && year <= max;
	}

	bool IsHeightValid() const
	{
		// Has to be at least 2 letters for "in" or "cm"
		if ((int)m_hgt.size() <= 2)
			return false;

		string measurement = m_hgt.substr((int)m_hgt.size() - 2);
		int value = stoi(m_hgt.substr(0, (int)m_hgt.size() - 2));

		// If "cm", must be between 150 and 193
		if (strcmp(measurement.c_str(), "cm") == 0)
		{
			return value >= 150 && value <= 193;
		}
		// If "in", must be between 59 and 76
		else if (strcmp(measurement.c_str(), "in") == 0)
		{
			return value >= 59 && value <= 76;
		}
		return false;
	}

	bool IsHairColourValid() const
	{
		if (m_hcl.empty() || (int)m_hcl.size() != 7)
			return false;

		if (m_hcl.at(0) != '#')
			return false;

		for (int i = 1; i < (int)m_hcl.size(); ++i)
		{
			char value = m_hcl.at(i);
			if (value < '0' 
				|| value > 'f'
				|| (value > '9' && value < 'a'))
				return false;
		}

		return true;
	}

	bool IsEyeColourValid() const
	{
		if ((int)m_ecl.size() != 3)
			return false;

		const char* ecl_chr = m_ecl.c_str();
		if (strcmp(ecl_chr, "amb") != 0
			&& strcmp(ecl_chr, "blu") != 0
			&& strcmp(ecl_chr, "brn") != 0
			&& strcmp(ecl_chr, "gry") != 0
			&& strcmp(ecl_chr, "grn") != 0
			&& strcmp(ecl_chr, "hzl") != 0
			&& strcmp(ecl_chr, "oth") != 0)
			return false;

		return true;
	}

	bool IsPassportIDValid() const
	{
		if ((int)m_pid.size() != 9)
			return false;

		for (int i = 1; i < (int)m_pid.size(); ++i)
		{
			char value = m_pid.at(i);
			if (value < '0' || value > '9')
				return false;
		}

		return true;
	}

	bool IsValidPartOne() const
	{
		return m_byr != -1
			&& m_iyr != -1
			&& m_eyr != -1
			&& !m_hgt.empty()
			&& !m_pid.empty()
			&& !m_hcl.empty()
			&& !m_ecl.empty();
	}

	bool IsValidPartTwo() const
	{
		return IsYearValid(m_byr, 1920, 2002)
			&& IsYearValid(m_iyr, 2010, 2020)
			&& IsYearValid(m_eyr, 2020, 2030)
			&& IsHeightValid()
			&& IsHairColourValid()
			&& IsEyeColourValid()
			&& IsPassportIDValid();
	}


	int m_byr; // Birth year
	int m_iyr; // Issue year
	int m_eyr; // Expiration Year
	string m_hgt; // Height
	string m_hcl; // Hair Colour
	string m_ecl; // Eye Colour
	string m_pid; // Passport ID
	string m_cid; // Country ID
};


vector<Passport> m_passports;

void InitTokenToTypeMap()
{
	m_idToTypeMap.emplace("byr", BIRTH_YEAR);
	m_idToTypeMap.emplace("iyr", ISSUE_YEAR);
	m_idToTypeMap.emplace("eyr", EXP_YEAR);
	m_idToTypeMap.emplace("hgt", HEIGHT);
	m_idToTypeMap.emplace("hcl", HAIR_COLOUR);
	m_idToTypeMap.emplace("ecl", EYE_COLOUR);
	m_idToTypeMap.emplace("pid", PASSPORT_ID);
	m_idToTypeMap.emplace("cid", COUNTRY_ID);
}

int main()
{
	InitTokenToTypeMap();

	ifstream inputStream("input.txt");
	if (inputStream.is_open())
	{
		Passport currentPassport;
		string line;
		while (getline(inputStream, line))
		{
			if (!line.empty())
			{
				// Iterate over key-value pairs on line
				int index = (int)line.rfind(' ');
				vector<string> keyValuePairs;
				string keyValue = line;
				while (index != -1)
				{
					keyValue = line.substr(index+1);
					currentPassport.SetPassportInfo(keyValue);
					line = line.substr(0, index);
					index = (int)line.rfind(' ');
				}
				currentPassport.SetPassportInfo(line);
			}
			else
			{
				// End of passport entry
				m_passports.push_back(currentPassport);
				currentPassport = Passport();
			}
		}
		m_passports.push_back(currentPassport);
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		int numValidPassports = 0;
		for (int i = 0; i < (int)m_passports.size(); ++i)
		{
			if (m_passports[i].IsValidPartOne())
				numValidPassports++;
		}

		cout << "The number of valid passports is: " << numValidPassports << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		int numValidPassports = 0;
		for (int i = 0; i < (int)m_passports.size(); ++i)
		{
			if (m_passports[i].IsValidPartTwo())
				numValidPassports++;
		}

		cout << "The number of valid passports is: " << numValidPassports << endl;
	}
}
