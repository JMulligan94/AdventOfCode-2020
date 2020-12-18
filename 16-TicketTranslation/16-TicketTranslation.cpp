// 16-TicketTranslation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <stdint.h>
#include <sstream>

using namespace std;

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test2.txt";

// Class representing a field on a ticket
class TicketField
{
public:
	TicketField() {}

	// Parse string line into the ticket field info
	TicketField(string line)
	{
		// Get field name
		int colonIndex = (int)line.find(':');
		m_name = line.substr(0, colonIndex);


		// Get field ranges
		string ranges = line.substr(colonIndex + 2);

		string token;
		istringstream tokenStream(ranges);
		while (getline(tokenStream, token, ' '))
		{
			int rangeDelimiterIndex = (int)token.find('-');
			if (rangeDelimiterIndex == -1)
				continue;

			int lowerLimit = stoi(token.substr(0, rangeDelimiterIndex));
			int upperLimit = stoi(token.substr(rangeDelimiterIndex+1));

			m_ranges.push_back(pair<int, int>(lowerLimit, upperLimit));
		}
	};

	// Is the given value valid for this field
	bool IsValueValid(int value) const
	{
		for (int i = 0; i < (int)m_ranges.size(); ++i)
		{
			if (value >= m_ranges[i].first
				&& value <= m_ranges[i].second)
				return true;
		}
		return false;
	}

	// Are all values in a given list valid for this field?
	bool ValidForValues(const vector<int>& values)
	{
		for (int value : values)
		{
			if (!IsValueValid(value))
				return false;
		}
		return true;
	}

	string m_name;
	vector<pair<int, int>> m_ranges;
};

// Class representing a Ticket
class Ticket
{
public:
	Ticket() {}
	
	// Parse string into ticket information
	Ticket(string line)
	{
		// Get all ticket values
		string token;
		istringstream tokenStream(line);
		while (getline(tokenStream, token, ','))
		{
			m_values.push_back(stoi(token));
		}
	}

	// Is this ticket valid for any of the known fields?
	// Return the incorrect value on the ticket
	int IsValid(const vector<TicketField>& fields) const
	{
		vector<int> valuesToCheck = m_values;
		for (TicketField field : fields)
		{
			for (int i = valuesToCheck.size() - 1; i >= 0; --i)
			{
				int value = valuesToCheck[i];
				if (field.IsValueValid(value))
				{
					valuesToCheck.erase(valuesToCheck.begin() + i);
				}
			}
		}

		return valuesToCheck.empty() ? -1 : valuesToCheck.front();
	}

	vector<int> m_values;
};

vector<TicketField> m_fields;
Ticket m_myTicket;
vector<Ticket> m_nearbyTickets;

int main()
{
	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;

		// Get fields
		while (getline(inputStream, line))
		{
			if (!line.empty())
			{
				m_fields.push_back(TicketField(line));
			}
			else 
				break;
		}

		// Get your ticket
		getline(inputStream, line);
		if (line == "your ticket:")
		{
			getline(inputStream, line);
			m_myTicket = Ticket(line);
		}

		// Empty line
		getline(inputStream, line);

		// Get nearby tickets
		getline(inputStream, line);
		if (line == "nearby tickets:")
		{
			while (getline(inputStream, line))
			{
				m_nearbyTickets.push_back(Ticket(line));
			}
		}
	}

	// Determine the valid tickets
	vector<Ticket> validTickets;
	if (m_myTicket.IsValid(m_fields))
		validTickets.push_back(m_myTicket);

	{
		// Part One
		cout << endl << "=== Part 1 ===" << endl;
		int sumErrorRate = 0;
		for (int i = 0; i < m_nearbyTickets.size(); ++i)
		{
			Ticket nearbyTicket = m_nearbyTickets[i];
			int errorRate = nearbyTicket.IsValid(m_fields);
			if (errorRate == -1)
			{
				validTickets.push_back(nearbyTicket);
			}
			else
			{
				sumErrorRate += errorRate;
				cout << "INVALID Ticket " << i << ": " << errorRate << " (" << sumErrorRate << ")" << endl;
			}
		}

		cout << "The ticket scanning error rate for nearby tickets is: " << sumErrorRate << endl << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;
		// Create a list for each column containing all values in the column
		vector<vector<int>> columns;
		for (int column = 0; column < m_fields.size(); ++column)
		{
			vector<int> columnValues;
			for (int i = 0; i < validTickets.size(); ++i)
			{
				columnValues.push_back(validTickets[i].m_values[column]);
			}
			columns.push_back(columnValues);
		}

		// Iterate columns trying to find matching fields
		vector<TicketField> columnFields(m_fields.size());
		vector<bool> ignoreColumns(m_fields.size()); // Columns to ignore - we've already found the field for this column
		vector<bool> ignoreFieldIndices(m_fields.size()); // Fields to ignore - we've already found the column for this field
		int determinedFieldCount = 0; // How many fields have we worked out so far
		
		// Iterate until we determine all fields
		while (determinedFieldCount != m_fields.size())
		{
			for (int column = 0; column < m_fields.size(); ++column)
			{
				// Skip column if already found matching field
				if (ignoreColumns[column])
					continue;

				// Get field indices which the column is valid for
				vector<int> matchingFieldIndices;
				for (int fieldIndex = 0; fieldIndex < m_fields.size(); ++fieldIndex)
				{
					// Skip field if already found matching column
					if (ignoreFieldIndices[fieldIndex])
						continue;

					// Is this field possible when considering the column values given
					TicketField field = m_fields[fieldIndex];
					if (field.ValidForValues(columns[column]))
						matchingFieldIndices.push_back(fieldIndex);
				}

				//cout << "Found " << matchingFieldIndices.size() << " matching fields for column " << column << endl;

				// If we've only found a single matching field, it has to be that one!
				if (matchingFieldIndices.size() == 1)
				{
					// Discard the column and field from the future checks so nothing else can
					// match to this field again
					ignoreFieldIndices[matchingFieldIndices.front()] = true;
					ignoreColumns[column] = true;

					determinedFieldCount++;
					cout << "Column " << column << " must be field \"" << m_fields[matchingFieldIndices.front()].m_name << "\"!" << endl;

					// Store field-column relationship
					columnFields[column] = m_fields[matchingFieldIndices.front()];
				}
			}
		}

		cout << endl;

		// Determin departure product for fields beginning with "departure"
		uint64_t departureProduct = 1;
		for (int i = 0; i < columnFields.size(); ++i)
		{
			cout << "Column: " << i << " is \"" << columnFields[i].m_name << "\"" << endl;
			if (columnFields[i].m_name.find("departure") != -1)
			{
				departureProduct *= m_myTicket.m_values[i];
			}
		}

		cout << endl << "The departure product is: " << departureProduct << endl;
	}
}