// 21-AllergenAssessment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm> //std::sort
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>

using namespace std;

map<string, vector<string>> m_potentialAllergenMap;

map<string, string> m_matchedAllergens;
map<string, string> m_matchedIngredients;

class Food
{
public:

	// Parse a line from input into Food class
	Food(string line)
	{
		string token;
		istringstream ingredientsStream(line);
		
		// Get ingredients (all tokens before '(')
		while (getline(ingredientsStream, token, ' '))
		{
			if (token[0] == '(')
			{
				break;
			}
			m_ingredients.push_back(token);
		}

		// Get marked allergens
		string containsStr = "(contains ";
		int startAllergensIndex = (int)line.find(containsStr) + containsStr.length();
		string allergensStr = line.substr(startAllergensIndex, line.length() - startAllergensIndex - 1);

		istringstream allergensStream(allergensStr);
		while (getline(allergensStream, token, ' '))
		{
			if (token.back() == ',')
				token = token.substr(0, token.length() - 1);

			m_knownAllergens.push_back(token);
		}
	}

	// Function for returning the intersection of two vectors
	std::vector<std::string> intersection(std::vector<std::string>& v1,
		std::vector<std::string>& v2) {
		std::vector<std::string> v3;

		std::sort(v1.begin(), v1.end());
		std::sort(v2.begin(), v2.end());

		std::set_intersection(v1.begin(), v1.end(),
			v2.begin(), v2.end(),
			back_inserter(v3));
		return v3;
	}

	void AddAllergenInfoToMap()
	{
		for (string knownAllergen : m_knownAllergens)
		{
			bool addedAllergen = false;

			// Add allergen to map if not seen before
			if (m_potentialAllergenMap.find(knownAllergen) == m_potentialAllergenMap.end())
			{
				m_potentialAllergenMap.emplace(knownAllergen, vector<string>());
				addedAllergen = true;
			}

			// If the allergen has just been added, all ingredients are potential matches
			if (addedAllergen)
			{
				m_potentialAllergenMap[knownAllergen] = m_ingredients;
			}
			else
			{
				// Find the intersection between the two vectors
				m_potentialAllergenMap[knownAllergen] = intersection(m_ingredients, m_potentialAllergenMap[knownAllergen]);
			}
		}
	}

	int GetNumberOfSafeIngredients() const
	{
		int numSafeIngredients = 0;
		for (string ingredient : m_ingredients)
		{
			// Ingredient is safe if thre are no matching allergens for it in map
			numSafeIngredients += m_matchedIngredients.find(ingredient) == m_matchedIngredients.end() ? 1 : 0;
		}
		return numSafeIngredients;
	}

private:
	vector<string> m_ingredients;
	vector<string> m_knownAllergens;
};

const char* c_inputFile = "Input.txt";
const char* c_testFile = "Test.txt";


int main()
{
	vector<Food*> foods;

	ifstream inputStream(c_inputFile);
	if (inputStream.is_open())
	{
		string line;

		// Get fields
		while (getline(inputStream, line))
		{
			foods.push_back(new Food(line));
		}
	}

	// Build allergen map
	for (Food* food : foods)
	{
		food->AddAllergenInfoToMap();
	}


	// Iterate until we have found the ingredient match for all allergens
	int iteration = 0;
	while (m_matchedAllergens.size() != m_potentialAllergenMap.size())
	{
		cout << "==================================" << endl;
		cout << "Iteration " << iteration << endl;

		// Iterate over allergen map - find any allergens with just ONE potential ingredient
		// If found, this ingredient has to be the match and we can remove from all other lists
		// Since an ingredient cannot have more than one allergen
		for (auto allergenIter = m_potentialAllergenMap.begin(); allergenIter != m_potentialAllergenMap.end(); ++allergenIter)
		{
			if (m_matchedAllergens.find(allergenIter->first) != m_matchedAllergens.end())
				continue;
			
			cout << endl << "=== Allergen (" << allergenIter->first << ") ===" << endl;
			for (string potentialIngred : allergenIter->second)
			{
				cout << "\t- " << potentialIngred << endl;
			}

			if (allergenIter->second.size() == 1)
			{
				string allergen = allergenIter->first;
				string ingredient = allergenIter->second.front();
				cout << endl << "Found a match for " << allergen << " -> " << ingredient << "!" << endl;

				m_matchedAllergens.emplace(allergen, ingredient); // Map allergens to ingredients
				m_matchedIngredients.emplace(ingredient, allergen); // And ingredients to allergens

				// Remove the ingredient from allergen map
				for (auto allergenIter2 = m_potentialAllergenMap.begin(); allergenIter2 != m_potentialAllergenMap.end(); ++allergenIter2)
				{
					if (m_matchedAllergens.find(allergenIter2->first) != m_matchedAllergens.end())
						continue;

					for (int i = allergenIter2->second.size() - 1; i >= 0; --i)
					{
						if (ingredient == allergenIter2->second[i])
						{
							cout << endl << "Removed ingredient " << allergenIter2->second[i] << " from potential candidates for allergen " << allergenIter2->first << endl;
							allergenIter2->second.erase(allergenIter2->second.begin() + i);
							break;
						}
					}
				}
			}
		}
		iteration++;
	}

	// Print found pairs
	cout << endl << "=============================================" << endl;
	for (auto matchedIter = m_matchedAllergens.begin(); matchedIter != m_matchedAllergens.end(); ++matchedIter)
	{
		cout << "Allergen: \"" << matchedIter->first << "\", Ingredient: \"" << matchedIter->second << "\"" << endl;
	}
	cout << "=============================================" << endl;

	{
		// Part One
		cout << "=== Part 1 ===" << endl;
		
		// Get the number of non-allergen ingredients
		int numSafeIngredients = 0;
		for (Food* food : foods)
		{
			numSafeIngredients += food->GetNumberOfSafeIngredients();
		}

		cout << "The total number of safe ingredients in all foods is: " << numSafeIngredients << endl << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		// Get the canonical dangerous ingredient list
		vector<string> dangerousList;
		for (auto allergenIter = m_matchedAllergens.begin(); allergenIter != m_matchedAllergens.end(); ++allergenIter)
		{
			dangerousList.push_back(allergenIter->first);
		}

		// Sort allergens alphabetically
		sort(dangerousList.begin(), dangerousList.end());

		// Create ingredients string from alphabetically sorted allergens
		string dangerousStr;
		for (string allergen : dangerousList)
		{
			dangerousStr.append(m_matchedAllergens[allergen]);
			dangerousStr.append(",");
		}
		dangerousStr = dangerousStr.substr(0, dangerousStr.length() - 1);
		cout << "The canonical dangerous ingredient list is:" << endl << dangerousStr << endl;
	}

	// Delete any foods
	for (int i = 0; i < (int)foods.size(); ++i)
	{
		delete foods[i];
		foods[i] = nullptr;
	}
	foods.clear();
}
