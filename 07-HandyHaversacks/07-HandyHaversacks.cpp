// 07-HandyHaversacks.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>
#include <map>

using namespace std;

struct Bag;
map<unsigned int, Bag*> m_bagHashes;

// Given a colour string, find the corresponding Bag object
Bag* FindBag(const string& colour)
{
	hash<string> hasher;
	unsigned int id = (unsigned int)hasher(colour);
	auto findResult = m_bagHashes.find(id);
	if (findResult != m_bagHashes.end())
	{
		return findResult->second;
	}
	return nullptr;
}

struct Bag
{
	// Convert a string from input.txt into a child bag
	void AddSubBagFromString(string childBagStr)
	{
		// Disregard no other bag strings
		if (strcmp(childBagStr.c_str(), "no other bags") == 0)
			return;

		// Find space which separates quantity from bag colour
		int spaceIndex = (int)childBagStr.find(' ');
		if (spaceIndex == -1)
			return;

		// Get quantity
		string quantityStr = childBagStr.substr(0, spaceIndex);
		int childQuantity = stoi(quantityStr);

		// Get colour
		string childColour = childBagStr.substr(spaceIndex + 1);

		// Find terminate index to remove ' bag' or ' bags' (depending on quantity)
		int terminateIndex = (int)childColour.size() - 4;
		if (childQuantity > 1)
		{
			terminateIndex--; // remove 's'
		}

		childColour = childColour.substr(0, terminateIndex); 

		Bag* childBag = FindBag(childColour);
		if (childBag == nullptr)
		{
			cout << "Couldn't find a bag for: " << childColour << endl;
			return;
		}
		m_childBags.push_back(pair<Bag*, int>(childBag, childQuantity));
	}

	// Check if this bag contains the query bag (optional recursive)
	bool ContainsBag(Bag* queryBag, bool recursive)
	{
		for (int i = 0; i < (int)m_childBags.size(); ++i)
		{
			Bag* childBag = m_childBags[i].first;
			if (childBag == queryBag)
				return true;

			if (recursive && childBag->ContainsBag(queryBag, recursive))
				return true;
		}
		return false;
	}

	// Get the number of bags contained within this bag (optionally recursive)
	int GetQuantityChildBags(bool recursive)
	{
		int quantity = 0;
		for (int i = 0; i < (int)m_childBags.size(); ++i)
		{
			quantity += m_childBags[i].second;

			if (recursive)
				quantity += (m_childBags[i].first->GetQuantityChildBags(recursive) * m_childBags[i].second);
		}
		return quantity;
	}

	string m_colour;
	vector<pair<Bag*, int>> m_childBags;
};


int main()
{
	vector<pair<Bag*, string>> bagInfos;

	// Read input.txt and grab the relevant info
	ifstream inputStream("input.txt");
	if (inputStream.is_open())
	{
		string line;
		hash<string> hasher;

		// Store all types of bag first - with the remaining string from the line
		while (getline(inputStream, line))
		{
			Bag* currentBag = new Bag();
			int containIndex = (int)line.find(" contain");

			// Get bag colour from line
			int colourIndex = containIndex - 5; // remove " bags" from string
			string bagColour = line.substr(0, colourIndex);
			currentBag->m_colour = bagColour;

			unsigned int id = (unsigned int)hasher(bagColour);
			m_bagHashes.emplace(id, currentBag);

			// Grab rest of line and store alongside Bag object
			int remainingLineIndex = containIndex + 9;
			string remainingLine = line.substr(remainingLineIndex);
			bagInfos.push_back(pair<Bag*, string>(currentBag, remainingLine));
		}
	}
	
	// Parse the remaining line string and fill in child bag info
	for (int i = 0; i < (int)bagInfos.size(); ++i)
	{
		Bag* bag = bagInfos[i].first;
		string info = bagInfos[i].second;
		string fullLine = info;
		info = info.substr(0, (int)info.size() - 1); // Get rid of '.'

		string subBagStr = info;
		int subBagIndex = (int)info.rfind(',');
		while (subBagIndex != -1)
		{
			subBagStr = info.substr(subBagIndex + 2);
			info = info.substr(0, subBagIndex);
			subBagIndex = (int)info.rfind(',');

			bag->AddSubBagFromString(subBagStr);
		}
		bag->AddSubBagFromString(info);
	}

	Bag* shinyGoldBag = FindBag("shiny gold");
	{
		// Part One
		cout << "=== Part 1 ===" << endl;

		int numContainingShinyGoldBag = 0;
		for (auto bagIter = m_bagHashes.begin(); bagIter != m_bagHashes.end(); ++bagIter)
		{
			Bag* bag = bagIter->second;
			if (bag->ContainsBag(shinyGoldBag, true))
				numContainingShinyGoldBag++;
		}
		cout << "The number of bags that contain a shiny gold bag is: " << numContainingShinyGoldBag << endl;
	}

	{
		// Part Two
		cout << "=== Part 2 ===" << endl;

		int numShinyGoldChildBags = shinyGoldBag->GetQuantityChildBags(true);
		cout << "The number of bags contained within a shiny gold bag is: " << numShinyGoldChildBags << endl;
	}

	for (auto bagIter = m_bagHashes.begin(); bagIter != m_bagHashes.end(); ++bagIter)
	{
		delete bagIter->second;
		bagIter->second = nullptr;
	}
}
