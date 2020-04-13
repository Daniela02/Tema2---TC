#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <unordered_set>

using namespace std;
ifstream f("input.txt");

struct Production {
	char nonterminal;
	string rightSide;
};

struct Earley {
	char nonterminal;
	string left, right;
	int j;

};

struct Grammar {
	set<char> nonterminalSet;
	char start;
	set<char> terminalSet;
	vector<Production> productionVector;

	void calculateLamdas() 
	{
		bool canContinue = true;
		while (canContinue) 
		{
			canContinue = false;
			for (auto production : productionVector)
			{
				if (lambdaTerminals.find(production.nonterminal) != lambdaTerminals.end()) 
					continue;
				
				if (production.rightSide == "") 
				{
					lambdaTerminals.insert(production.nonterminal);
					canContinue = true;
				}
				else 
				{
					bool stringIsOnlyNonterminals = true;

					for (char x : production.rightSide) 
					{
						if (islower(x) || lambdaTerminals.find(x) == lambdaTerminals.end()) 
						{
							stringIsOnlyNonterminals = false;
							break;
						}
					}

					if (stringIsOnlyNonterminals) 
						lambdaTerminals.insert(production.nonterminal);
				}

			}

		}
	}
	set<char>lambdaTerminals;
};

Grammar grammar;

void citire()
{
	int nonterminalNumber;
	f >> nonterminalNumber;
	for (int i = 0; i < nonterminalNumber; i++) 
	{
		char c;
		f >> c;
		grammar.nonterminalSet.insert(c);
	}

	f >> grammar.start;

	int terminalNumber;
	f >> terminalNumber;
	for (int i = 0; i < terminalNumber; i++) 
	{
		char c;
		f >> c;
		grammar.terminalSet.insert(c);
	}

	int productionNumber;
	f >> productionNumber;
	for (int i = 0; i < productionNumber; i++) 
	{
		Production p;
		f >> p.nonterminal;
		f >> p.rightSide;
		if (p.rightSide == ".") {
			p.rightSide = "";
		}
		grammar.productionVector.push_back(p);
	}
}

void push(Earley earlyConfiguration, vector<Earley>& S, bool& canRepeat) 
{
	for (int i = 0; i < S.size(); i++) 
	{
		if (S[i].right == earlyConfiguration.right && S[i].left == earlyConfiguration.left && S[i].nonterminal == earlyConfiguration.nonterminal && S[i].j == earlyConfiguration.j) 
			return;
	}

	canRepeat = true;
	S.push_back(earlyConfiguration);
}

bool earleyMethod(Grammar grammar, string cuvant) {
	vector<vector<Earley> >S(cuvant.length() + 1, vector<Earley>());

	grammar.nonterminalSet.insert('P');

	Production prod;
	prod.nonterminal = 'P';
	prod.rightSide = "S";

	grammar.productionVector.push_back(prod);

	grammar.calculateLamdas();
	Earley earlyConfiguration;
	earlyConfiguration.nonterminal = 'P';
	earlyConfiguration.left = "";
	earlyConfiguration.right = "S";
	earlyConfiguration.j = 0;

	S[0].push_back(earlyConfiguration);

	for (int i = 0; i <= cuvant.length(); i++)
	{
		bool canRepeat = true;
		while (canRepeat) 
		{
			canRepeat = false;
			for (int j = 0; j < S[i].size(); j++) 
			{
				earlyConfiguration = S[i][j];

				if (earlyConfiguration.right != "") 
				{	//predictie
					for (auto productie : grammar.productionVector) 
					{
						if (productie.nonterminal != earlyConfiguration.right.at(0)) 
							continue;
						
						Earley temp;
						temp.nonterminal = productie.nonterminal;
						temp.left = "";
						temp.right = productie.rightSide;
						temp.j = i;

						push(temp, S[i], canRepeat);
					}
					if (grammar.lambdaTerminals.find(earlyConfiguration.right.at(0)) != grammar.lambdaTerminals.end()) 
					{
						Earley temp;
						temp.nonterminal = earlyConfiguration.nonterminal;
						temp.left = earlyConfiguration.left + earlyConfiguration.right.at(0);
						string afterDot(earlyConfiguration.right.begin() + 1, earlyConfiguration.right.end());
						temp.right = afterDot;
						temp.j = earlyConfiguration.j;

						push(temp, S[i], canRepeat);

					}
				}
				else 
				{
					for (int jj = 0; jj < S[earlyConfiguration.j].size(); jj++) 
					{	//completare
						if (S[earlyConfiguration.j][jj].right != "" && S[earlyConfiguration.j][jj].right.at(0) == earlyConfiguration.nonterminal)
						{
							Earley temp = S[earlyConfiguration.j][jj];
							string afterDot(S[earlyConfiguration.j][jj].right.begin() + 1, S[earlyConfiguration.j][jj].right.end());
							temp.left = temp.left + earlyConfiguration.nonterminal;
							temp.right = afterDot;

							push(temp, S[i], canRepeat);
						}
					}
				}
			}
		}

		for (int j = 0; j < S[i].size(); j++)
		{
			earlyConfiguration = S[i][j];

			if (earlyConfiguration.right != "" && i < cuvant.length() && earlyConfiguration.right.at(0) == cuvant.at(i))
			{	//scanare
				Earley temp;
				temp.j = earlyConfiguration.j;
				temp.nonterminal = earlyConfiguration.nonterminal;
				temp.left = earlyConfiguration.left + earlyConfiguration.right.at(0);
				string afterDot(earlyConfiguration.right.begin() + 1, earlyConfiguration.right.end());
				temp.right = afterDot;

				push(temp, S[i + 1], canRepeat);
			}
		}

		cout << "S" << i << '\n';

		for (int j = 0; j < S[i].size(); j++)
		{
			cout << S[i][j].nonterminal << "->" << S[i][j].left << "." << S[i][j].right << "," << S[i][j].j << '\n';
		}
	}

	bool isOk = false;
	for (auto earlyConfiguration : S[cuvant.length()])
	{
		int j = earlyConfiguration.j;
		if (earlyConfiguration.nonterminal = 'P' && earlyConfiguration.right == "" && earlyConfiguration.left == "S" && j == 0)
		{
			isOk = true;
			break;
		}
	}

	return isOk;
}

int main()
{
	citire();
	string cuvant;
	while(f >> cuvant)
	{
		bool isInLanguage = earleyMethod(grammar, cuvant);
		
		if (isInLanguage)
			cout << "Cuvantul " << cuvant << " " << "se afla in limbaj\n";
		else
			cout << "Cuvantul " << cuvant << " " << "nu se afla in limbaj\n";
		cout << '\n';
	}
	return 0;
	f.close();
}