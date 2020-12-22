// 19-MonsterMessages.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdint.h>
#include <sstream>
#include <regex>

using namespace std;

class Rule;
vector<Rule*> m_rules;
vector<string> m_messages;

class Rule
{
public:
	Rule(string line)
	{
		// Parse line into Rule info
		int colonIndex = (int)line.find(':');
		m_id = stoi(line.substr(0, colonIndex));

		string token;
		istringstream stream(line.substr(colonIndex + 2));
		m_messageChar = '#';

		int critIdx = 0;
		bool fillCritA = true;

		while (getline(stream, token, ' '))
		{
			if (token[0] == '\"')
			{
				m_messageChar = token[1];
				continue;
			}

			if (token == "|")
			{
				critIdx++;
				continue;
			}

			if (critIdx >= (int)m_criterias.size())
				m_criterias.push_back(new Criteria());

			m_criterias[critIdx]->m_criteriaIds.push_back(stoi(token));
		}
	}

	~Rule()
	{
		// Delete all Criteria ptrs
		for (int i = 0; i < (int)m_criterias.size(); ++i)
		{
			delete m_criterias[i];
			m_criterias[i] = nullptr;
		}
		m_criterias.clear();
	}

	void Fixup()
	{
		// Rule is just a char - no need to fixup
		if (m_messageChar != '#')
		{
			return;
		}

		// Find the Rule ptrs by cached IDs for the criteria
		for (int i = 0; i < (int)m_criterias.size(); ++i)
		{
			m_criterias[i]->m_criteriaRules.clear();
			for (int id : m_criterias[i]->m_criteriaIds)
			{
				m_criterias[i]->m_criteriaRules.push_back(m_rules[id]);
			}
		}
	}

	void BuildRegexString()
	{
		// Regex expression has already been created - early out
		if (!m_cachedRegex.empty())
			return;

		if (m_messageChar != '#')
		{
			// Terminate here with char data
			m_cachedRegex = m_messageChar;
			return;
		}

		// Get sub-rule regex expressions
		vector<string> regexParts;
		for (Criteria* criteria : m_criterias)
		{
			string regexPart;
			for (Rule* rule : criteria->m_criteriaRules)
			{
				if (rule != nullptr)
				{
					rule->BuildRegexString();
					regexPart.append(rule->GetRegex());
				}
			}
			regexParts.push_back(regexPart);
		}


		// Build up regex string from sub-rules
		// (SubRule1 | SubRule2 | SubRule3 etc.)
		string fullRegex;
		fullRegex.append("(");
		for (int i = 0; i < (int)regexParts.size(); ++i)
		{
			fullRegex.append(regexParts[i]);

			if (i != regexParts.size() - 1)
				fullRegex.append("|");
		}
		fullRegex.append(")");

		m_cachedRegex = fullRegex;
	}

	// Does message fully match the rule's regex expression
	bool Matches(string message)
	{
		if (m_cachedRegex.empty())
			BuildRegexString();

		const regex expression(m_cachedRegex);
		return regex_match(message, expression);
	}

	// Does message start with the rule's regex expression
	bool StartsWithMatch(string message)
	{
		string startRegex = "^"; ;
		startRegex.append(m_cachedRegex);
		const regex expression(startRegex);
		return regex_search(message, expression);
	}

	// Does message end with the rule's regex expression
	bool EndsWithMatch(string message)
	{
		string endRegex = m_cachedRegex;
		endRegex.append("$");
		const regex expression(endRegex);
		return regex_search(message, expression);
	}

	int GetID() const { return m_id; }

	void OverwriteRegex(const string& regexStr) { m_cachedRegex = regexStr; }

	string GetRegex() const { return m_cachedRegex; }

private:

	struct Criteria
	{
		vector<int> m_criteriaIds;
		vector<Rule*> m_criteriaRules;
	};

	int m_id;
	char m_messageChar;

	vector<Criteria*> m_criterias;

	string m_cachedRegex;
};


const char* c_inputFile = "Input.txt";
const char* c_testFile = "TestA.txt";

const char* c_test2 = "Test2.txt";

int main()
{
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;

		// Get rules
		while (getline(inputStream, line))
		{
			if (line.empty())
				break;

			Rule* newRule = new Rule(line);
			if (newRule->GetID() >= (int)m_rules.size())
			{
				m_rules.resize(newRule->GetID() + 1);
			}
			m_rules[newRule->GetID()] = newRule;
		}

		// Get messages
		while (getline(inputStream, line))
		{
			m_messages.push_back(line);
		}

		// Fixup all rules
		for (Rule* rule : m_rules)
		{
			if (rule != nullptr)
				rule->Fixup();
		}


		{
			// Part One
			cout << "=== Part 1 ===" << endl;
			
			m_rules[0]->BuildRegexString();
			cout << "Rule[0] regex = " << m_rules[0]->GetRegex() << endl << endl;

			int numMatch = 0;
			for (string message : m_messages)
			{
				if (m_rules[0]->Matches(message))
				{
					//cout << "MATCH: " << message << endl;;
					numMatch++;
				}
			}
			cout << endl << "Found " << numMatch << " matches" << endl;
		}


		{
			// Part Two
			cout << endl << "=== Part 2 ===" << endl;

			// ============= Change rules 8 and 11 =================
			delete m_rules[8];
			m_rules[8] = new Rule("8: 42 | 42 8");

			delete m_rules[11];
			//m_rules[11] = new Rule("11: 42 31 | 42 11 31");
			// Manually expanded out for recursive patterns - messy but it works!
			m_rules[11] = new Rule("11: 42 31 | 42 42 31 31 | 42 42 42 31 31 31 | 42 42 42 42 31 31 31 31 | 42 42 42 42 42 31 31 31 31 31");

			for (Rule* rule : m_rules)
			{
				// Reset cached expressions
				if (rule != nullptr)
					rule->OverwriteRegex("");
			}

			m_rules[8]->Fixup();
			m_rules[11]->Fixup();
			m_rules[0]->Fixup();

			// Rule 0: 8 11

			string rule8Regex;
			// Rule 8: 42 | 42 8
			// Which means the message has to start with rule 42's regex one or more times

			m_rules[42]->BuildRegexString();
			string regex42 = m_rules[42]->GetRegex();

			rule8Regex = regex42;
			rule8Regex.append("+");
			m_rules[8]->OverwriteRegex(rule8Regex);
			//cout << "Manually setting Rule 8 regex to: \"" << rule8Regex << "\"" << endl << endl;

			// Rule 11: 42 31 | 42 11 31
			// Which means it must be immediately followed by a series of 42's regex, followed by a series of 31's regex
			// where the number of regexs are the same
			// i.e. 42, 42, 31, 31 
			// or 42, 42, 42, 31, 31, 31 etc...
			m_rules[31]->BuildRegexString();
			string regex31 = m_rules[31]->GetRegex();

			m_rules[11]->BuildRegexString();
			//cout << "Manually setting Rule 11 regex to: \"" << m_rules[11]->GetRegex() << "\"" << endl << endl;

			// =====================================================

			int numMatch = 0;
			for (string message : m_messages)
			{
				bool startsWith42 = m_rules[42]->StartsWithMatch(message);
				if (!startsWith42)
					continue;

				bool endsWith31 = m_rules[31]->EndsWithMatch(message);
				if (!endsWith31)
					continue;

				if (m_rules[0]->Matches(message))
				{
					cout << "MATCH: " << message << endl;;
					numMatch++;
				}
			}
			cout << endl << "Found " << numMatch << " matches" << endl;
		}

		for (int i = 0; i < (int)m_rules.size(); ++i)
		{
			delete m_rules[i];
			m_rules[i] = nullptr;
		}
		m_rules.clear();
	}
}
