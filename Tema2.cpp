#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <unordered_set>

using namespace std;
ifstream f("input.txt");

struct Productie {
	char nonterminal;
	string rezultat;
};

struct Earley {
	char nonterminal;
	string stanga, dreapta;
	int j;

};

struct Gramatica {
	set<char> multimeNonTerminale;
	char start;
	set<char> multimeTerminale;
	vector<Productie> vectorProductii;
	set<char>lambdaTerminale;
	void calculeazaLambda()
	{
		bool continua = true;
		while (continua)
		{
			continua = false;
			for (auto productie : vectorProductii)
			{
				if (lambdaTerminale.find(productie.nonterminal) != lambdaTerminale.end())
					continue;

				if (productie.rezultat == "")
				{
					lambdaTerminale.insert(productie.nonterminal);
					continua = true;
				}
				else
				{
					bool contineDoarNonterminale = true;

					for (char x : productie.rezultat)
					{
						if (islower(x) || lambdaTerminale.find(x) == lambdaTerminale.end())
						{
							contineDoarNonterminale = false;
							break;
						}
					}

					if (contineDoarNonterminale)
						lambdaTerminale.insert(productie.nonterminal);
				}

			}

		}
	}

};

Gramatica gramatica;

void citire()
{
	int nrNonterminale;
	f >> nrNonterminale;
	for (int i = 0; i < nrNonterminale; i++)
	{
		char c;
		f >> c;
		gramatica.multimeNonTerminale.insert(c);
	}

	f >> gramatica.start;

	int nrTerminale;
	f >> nrTerminale;
	for (int i = 0; i < nrTerminale; i++)
	{
		char c;
		f >> c;
		gramatica.multimeTerminale.insert(c);
	}

	int nrProductii;
	f >> nrProductii;
	for (int i = 0; i < nrProductii; i++)
	{
		Productie p;
		f >> p.nonterminal;
		f >> p.rezultat;
		if (p.rezultat == ".") {
			p.rezultat = "";
		}
		gramatica.vectorProductii.push_back(p);
	}
}

void push(Earley configuratie, vector<Earley>& S, bool& repeta)
{
	for (int i = 0; i < S.size(); i++)
	{
		if (S[i].dreapta == configuratie.dreapta && S[i].stanga == configuratie.stanga && S[i].nonterminal == configuratie.nonterminal && S[i].j == configuratie.j)
			return;
	}

	repeta = true;
	S.push_back(configuratie);
}

bool metodaEarley(Gramatica gramatica, string cuvant) {
	vector<vector<Earley> >S(cuvant.length() + 1, vector<Earley>());

	gramatica.multimeNonTerminale.insert('P');

	Productie prod;
	prod.nonterminal = 'P';
	prod.rezultat = "S";

	gramatica.vectorProductii.push_back(prod);

	gramatica.calculeazaLambda();
	Earley configuratie;
	configuratie.nonterminal = 'P';
	configuratie.stanga = "";
	configuratie.dreapta = "S";
	configuratie.j = 0;

	S[0].push_back(configuratie);

	for (int i = 0; i <= cuvant.length(); i++)
	{
		bool repeta = true;
		while (repeta)
		{
			repeta = false;
			for (int j = 0; j < S[i].size(); j++)
			{
				configuratie = S[i][j];

				if (configuratie.dreapta != "")
				{	//predictie
					for (auto productie : gramatica.vectorProductii)
					{
						if (productie.nonterminal != configuratie.dreapta.at(0))
							continue;

						Earley temp;
						temp.nonterminal = productie.nonterminal;
						temp.stanga = "";
						temp.dreapta = productie.rezultat;
						temp.j = i;

						push(temp, S[i], repeta);
					}
					if (gramatica.lambdaTerminale.find(configuratie.dreapta.at(0)) != gramatica.lambdaTerminale.end())
					{
						Earley temp;
						temp.nonterminal = configuratie.nonterminal;
						temp.stanga = configuratie.stanga + configuratie.dreapta.at(0);
						string dupaPunct(configuratie.dreapta.begin() + 1, configuratie.dreapta.end());
						temp.dreapta = dupaPunct;
						temp.j = configuratie.j;

						push(temp, S[i], repeta);

					}
				}
				else
				{
					for (int jj = 0; jj < S[configuratie.j].size(); jj++)
					{	//completare
						if (S[configuratie.j][jj].dreapta != "" && S[configuratie.j][jj].dreapta.at(0) == configuratie.nonterminal)
						{
							Earley temp = S[configuratie.j][jj];
							string dupaPunct(S[configuratie.j][jj].dreapta.begin() + 1, S[configuratie.j][jj].dreapta.end());
							temp.stanga = temp.stanga + configuratie.nonterminal;
							temp.dreapta = dupaPunct;

							push(temp, S[i], repeta);
						}
					}
				}
			}
		}

		for (int j = 0; j < S[i].size(); j++)
		{
			configuratie = S[i][j];

			if (configuratie.dreapta != "" && i < cuvant.length() && configuratie.dreapta.at(0) == cuvant.at(i))
			{	//scanare
				Earley temp;
				temp.j = configuratie.j;
				temp.nonterminal = configuratie.nonterminal;
				temp.stanga = configuratie.stanga + configuratie.dreapta.at(0);
				string dupaPunct(configuratie.dreapta.begin() + 1, configuratie.dreapta.end());
				temp.dreapta = dupaPunct;

				push(temp, S[i + 1], repeta);
			}
		}

		cout << "S" << i << '\n';

		for (int j = 0; j < S[i].size(); j++)
		{
			cout << S[i][j].nonterminal << "->" << S[i][j].stanga << "." << S[i][j].dreapta << "," << S[i][j].j << '\n';
		}
	}

	bool ok = false;
	for (auto configuratie : S[cuvant.length()])
	{
		int j = configuratie.j;
		if (configuratie.nonterminal = 'P' && configuratie.dreapta == "" && configuratie.stanga == "S" && j == 0)
		{
			ok = true;
			break;
		}
	}

	return ok;
}

int main()
{
	citire();
	string cuvant;
	while (f >> cuvant)
	{
		bool inLimbaj = metodaEarley(gramatica, cuvant);

		if (inLimbaj)
			cout << "Cuvantul " << cuvant << " " << "se afla in limbaj\n";
		else
			cout << "Cuvantul " << cuvant << " " << "nu se afla in limbaj\n";
		cout << '\n';
	}
	return 0;
	f.close();
}