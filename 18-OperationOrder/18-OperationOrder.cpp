// 18-OperationOrder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <string>
#include <stdint.h>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";

class Operand
{
public:
	virtual uint64_t ChronologicalResolve() const = 0;
	virtual uint64_t PrioritiseAdditionResolve() const = 0;
};

class IntOperand : public Operand
{
public:
	IntOperand(uint64_t value)
		: m_value(value)
	{
	}

	// Inherited via Operand
	virtual uint64_t ChronologicalResolve() const override
	{
		return m_value;
	}

	// Inherited via Operand
	virtual uint64_t PrioritiseAdditionResolve() const override
	{
		return m_value;
	}

private:
	uint64_t m_value;
};

class Expression : public Operand
{
public:
	Expression() = default;
	Expression(string line)
	{
		m_expression = line;

		int bracketStart;
		int bracketIdx = 0;
		for (int i = 0; i < line.size(); ++i)
		{
			char lineChar = line[i];
			switch (lineChar)
			{
				case ' ':
				{
					// Discard spaces
				}
				break;
				case '+':
				{
					if (bracketIdx == 0)
						m_operators.push_back(ADDITION);
				}
				break;
				case '*':
				{
					if (bracketIdx == 0)
						m_operators.push_back(MULTIPLICATION);
				}
				break;
				case '(':
				{
					if (bracketIdx == 0)
						bracketStart = i + 1;
					bracketIdx++;
				}
				break;
				case ')':
				{
					bracketIdx--;
					
					if (bracketIdx == 0)
					{
						string subExpression = line.substr(bracketStart, i - bracketStart);
						m_operands.push_back(new Expression(subExpression));
					}
				}
				break;
				default:
				{
					if (bracketIdx == 0)
					{
						// Should be a number...
						m_operands.push_back(new IntOperand(lineChar - '0'));
					}
				}
				break;
			}
		}
	}

	~Expression()
	{
		for (int i = 0 - 1; i < m_operands.size(); ++i)
		{
			delete m_operands[i];
			m_operands[i] = nullptr;
		}
		m_operands.clear();
	}

	// Inherited via Operand
	virtual uint64_t ChronologicalResolve() const override
	{
		uint64_t result = m_operands.front()->ChronologicalResolve();
		for (int i = 1; i < m_operands.size(); ++i)
		{
			uint64_t resolvedValue = m_operands[i]->ChronologicalResolve();

			switch (m_operators[i-1])
			{
				case ADDITION:
				{
					result += resolvedValue;
				}
				break;
				case MULTIPLICATION:
				{
					result *= resolvedValue;
				}
				break;
			}
			
		}
		return result;
	}

	// Inherited via Operand
	virtual uint64_t PrioritiseAdditionResolve() const override
	{
		vector<Operand*> localOperands = m_operands;
		vector<OperatorType> localOperatorType = m_operators;
		vector<int> createdOperators;

		// Resolve all additions
		for (int i = 0; i < localOperands.size()-1; ++i)
		{
			if (i < localOperatorType.size())
			{
				if (localOperatorType[i] == ADDITION)
				{
					uint64_t operandA = localOperands[i]->PrioritiseAdditionResolve();
					uint64_t operandB = localOperands[i + 1]->PrioritiseAdditionResolve();

					localOperatorType.erase(localOperatorType.begin() + i);

					localOperands.erase(localOperands.begin() + i);
					localOperands.erase(localOperands.begin() + i);
					localOperands.insert(localOperands.begin() + i, new IntOperand(operandA + operandB));
					createdOperators.push_back(i);
					
					i--;
					continue;
				}
			}
		}

		uint64_t result = localOperands.front()->PrioritiseAdditionResolve();
		// Resolve multiplications
		for (int i = 1; i < localOperands.size(); ++i)
		{
			result *= localOperands[i]->PrioritiseAdditionResolve();
		}

		return result;
	}

	string GetString() const
	{
		return m_expression;
	}

private:
	
	enum OperatorType
	{
		ADDITION,
		MULTIPLICATION
	};

	string m_expression;
	vector<Operand*> m_operands;
	vector<OperatorType> m_operators;
};

int main()
{
	vector<Expression*> expressions;
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;

		// Get fields
		while (getline(inputStream, line))
		{
			expressions.push_back(new Expression(line));
		}
	}

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		uint64_t answerSum = 0;
		for (int i = 0; i < expressions.size(); ++i)
		{
			//cout << "Expression: " << expressions[i]->GetString() << endl;
			uint64_t answer = expressions[i]->ChronologicalResolve();
			//cout << "Answer: " << answer << endl << endl;
			answerSum += answer;
		}

		cout << endl << "Sum of all answers = " << answerSum << endl << endl;
	}


	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		uint64_t answerSum = 0;
		for (int i = 0; i < expressions.size(); ++i)
		{
			cout << "Expression: " << expressions[i]->GetString() << endl;
			uint64_t answer = expressions[i]->PrioritiseAdditionResolve();
			cout << "Answer: " << answer << endl << endl;
			answerSum += answer;
		}

		cout << endl << "Sum of all answers = " << answerSum << endl << endl;
	}

	for (int i = 0; i < expressions.size(); ++i)
	{
		delete expressions[i];
		expressions[i] = nullptr;
	}
	expressions.clear();
}