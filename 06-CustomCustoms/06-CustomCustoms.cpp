// 06-CustomCustoms.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <set>

using namespace std;

struct Group
{
	void AddAnswers(string answers, bool first)
	{
		m_individualAnswers.push_back(answers);
		for (int i = 0; i < (int)answers.size(); ++i)
		{
			char answer = answers.at(i);
			m_uniqueAnswers.insert(answer);

			if (first)
			{
				m_commonAnswers.insert(answer);
			}
		}

		if (!first)
		{
			vector<char> commonVec(m_commonAnswers.size());
			copy(m_commonAnswers.begin(), m_commonAnswers.end(), commonVec.begin());
			// Remove all answers from common answers set that don't appear in this person's answers
			for (int i = 0; i < (int)commonVec.size(); ++i)
			{
				char answer = commonVec[i];
				int findResult = (int)answers.find(answer);

				// If not found in answers, remove from common set
				if (findResult == -1)
					m_commonAnswers.erase(answer);
			}
		}
	}

	int GetNumUniqueAnswers() const { return (int)m_uniqueAnswers.size(); }
	int GetNumCommonAnswers() const { return (int)m_commonAnswers.size(); }

	set<char> m_uniqueAnswers;
	set<char> m_commonAnswers;
	vector<string> m_individualAnswers;
};

vector<Group> m_groups;

int main()
{
	ifstream inputStream("input.txt");
	if (inputStream.is_open())
	{
		Group currentGroup;
		string line;
		bool firstPersonOfGroup = true;
		while (getline(inputStream, line))
		{
			if (!line.empty())
			{
				currentGroup.AddAnswers(line, firstPersonOfGroup);
				firstPersonOfGroup = false;
			}
			else
			{
				// End of passport entry
				m_groups.push_back(currentGroup);
				currentGroup = Group();
				firstPersonOfGroup = true;
			}
		}
		m_groups.push_back(currentGroup);
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		int numUniqueAnswers = 0;
		for (int i = 0; i < (int)m_groups.size(); ++i)
		{
			numUniqueAnswers += m_groups[i].GetNumUniqueAnswers();
		}

		cout << "The number of uniquely answered questions is: " << numUniqueAnswers << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		int numCommonAnswers = 0;
		for (int i = 0; i < (int)m_groups.size(); ++i)
		{
			numCommonAnswers += m_groups[i].GetNumCommonAnswers();
		}

		cout << "The number of common answered questions is: " << numCommonAnswers << endl;
	}

}
