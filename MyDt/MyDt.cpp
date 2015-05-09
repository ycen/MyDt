#include <fstream>
#include <iostream>
#include <vector>
#include "MyDt.h"

using namespace std;

int samefeature(node *p, vector< vector<double> >& mt){
	for (int i = 1; i < p->index.size(); i++)
	{
		if (mt[0] != mt[i])
		{
			return 0;
		}
	}
	return 1;
}

double entropy(vector<int>& bset, vector<int>& cv)
{
	double ent = 0.0;
	int N = bset.size();
	if (N == 0) { return 0; }
	int nPos = 0;
	for (int i = 0; i < N; i++)
	{
		if (cv[bset[i]] == 1) { nPos++; }
	}
	double pPos = (double)nPos / N;
	double pNeg = 1 - pPos;
	ent = -1 * pPos*log2(pPos) - pNeg*log2(pNeg);
	return ent;
}

void split(node *p, vector< vector<double> >& mt, vector<int>& cv){
	int nt = p->index.size();
	if (p->index.size() <= 1)
	{
		int npos = 0;
		for (int i = 0; i < nt; i++)
		{
			if (cv[p->index[i]] == 1)
			{
				npos++;
			}
		}
		int nneg = nt - npos;
		if (nneg>npos)
		{
			p->fvalue = 0;
		}
		else
		{
			p->fvalue = 1;
		}
		cout << "single sample: " << p->feature <<" "<<p->index.size()<< " " << p->fvalue << endl;
		return;
	}
	if (samefeature(p, mt))
	{
		int npos = 0;
		for (int i = 0; i < nt; i++)
		{
			if (cv[p->index[i]] == 1)
			{
				npos++;
			}
		}
		int nneg = nt - npos;
		if (nneg>npos)
		{
			p->fvalue = 0;
		}
		else
		{
			p->fvalue = 1;
		}
		cout << "samefeature: " << p->feature <<" "<<p->index.size()<< " " << p->fvalue << endl;
		return;
	}
	int nfeature = mt[0].size();
	double ign = 0.0;
	int splitfeature = -1;
	double initialEnt = entropy(p->index,cv);
	vector<int> lset, rset;
	for (int i = 0; i < nfeature; i++)
	{
		lset.clear();
		rset.clear();
		for (int j = 0; j < nt; j++)
		{
			if (mt[p->index[j]][i] == 1) { lset.push_back(p->index[j]); }
			if (mt[p->index[j]][i] == 0) { rset.push_back(p->index[j]); }
		}
		double lent = entropy(lset, cv)*((double)lset.size()/nt);
		double rent = entropy(rset, cv)*((double)rset.size()/nt);
		if (initialEnt - lent - rent>ign)
		{
			ign = initialEnt - lent - rent;
			splitfeature = i;
		}
	}
	if (splitfeature == -1)
	{ 
		int npos = 0;
		for (int i = 0; i < nt; i++)
		{
			if (cv[p->index[i]] == 1)
			{
				npos++;
			}
		}
		int nneg = nt - npos;
		if (nneg>npos)
		{
			p->fvalue = 0;
		}
		else
		{
			p->fvalue = 1;
		}
		cout << "no gain: " << p->feature << " " << p->index.size() << " " << p->fvalue << endl;
		return;
	}
	for (int i = 0; i < nt; i++)
	{
		lset.clear();
		rset.clear();
		if (mt[p->index[i]][splitfeature] == 1) { lset.push_back(p->index[i]); }
		if (mt[p->index[i]][splitfeature] == 0) { rset.push_back(p->index[i]); }
	}
	p->feature = splitfeature;
	node* lr = new node;
	node* rr = new node;
	for (int i = 0; i < nt; i++)
	{
		if (mt[p->index[i]][splitfeature] == 1) { lr->index.push_back(p->index[i]); }
		if (mt[p->index[i]][splitfeature] == 0) { rr->index.push_back(p->index[i]); }
	}
	p->lc = lr;
	p->rc = rr;
	cout << "splitfeature: " << splitfeature << " "<<p->index.size() <<" "<<p->fvalue<<endl;
	split(p->lc, mt, cv);
	split(p->rc, mt, cv);
}

int predict(node* p, vector<int>& tv)
{
	if (p->fvalue == 1) { return 1; };
	if (p->fvalue == 0) { return 0; };
	if (tv[p->feature] == 1) { return predict(p->rc, tv); };
	if (tv[p->feature] == 0) { return predict(p->lc, tv); };
}

int main(){
	//-------------------------------------Training---------------------------------------//
	vector<int> CatValue;
	ifstream inputs;
	inputs.open("data.txt");
	int nfeature;
	inputs >> nfeature;
	vector< vector<double> > mt(80000, vector<double>(nfeature, 0));
	//mt[feature][feature value]
	int nt = 0;
	while (!inputs.eof())
	{
		int a;
		int nouse;
		inputs >> nouse;
		inputs >> a;
		CatValue.push_back(a);
		for (int i = 0; i < nfeature; i++)
		{
			inputs >> mt[nt][i];
		}
		nt++;
	}
	inputs.close();
	//nt--;
	mt.resize(nt*nfeature);
	/*for (int i = 0; i < nfeature; i++)
	{
		cout << mt[2][i] << " " << endl;
	}*/
	node* root = new node;
	for (int i = 0; i < nt; i++)
	{
		root->index.push_back(i);
	}
	//cout << root->feature << " " << root->fvalue << endl;
	split(root, mt, CatValue);
	//---------------------------------------Predicting---------------------------------//
	ifstream predictInputs;
	predictInputs.open("test.txt");
	int ntest = 0;
	int nright = 0;
	while (!predictInputs.eof())
	{
		int a;
		int nouse;
		predictInputs >> nouse;
		predictInputs >> a;
		//CatValue.push_back(a);
		vector<int> onetest;
		for (int i = 0; i < nfeature; i++)
		{
			int b;
			predictInputs >> b;
			onetest.push_back(b);
		}
		int result = predict(root, onetest);
		ntest++;
		if (result == a)
		{
			nright++;
		}
	}
	cout << "test number: " << ntest << endl;
	cout << "right test: " << nright << endl;
	return 0;
}