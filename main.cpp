#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

fstream f_out; //used to print to file

struct LITERAL
{
	string val; // store variable's name
	int sign; // sign
	friend bool operator==(const LITERAL &a, const LITERAL &b)
	{
		return (a.sign == b.sign && a.val == b.val);
	}
};
struct CLAUSE
{
	vector<LITERAL> v_Literal; // array uses to store literals
	CLAUSE(){}
	CLAUSE(const CLAUSE &b)
	{
		for (int i = 0; i < b.v_Literal.size(); i++)
		{
			v_Literal.push_back(b.v_Literal[i]);
		}
	}
	friend bool operator==(const CLAUSE& c1, const CLAUSE &c2)
	{
		if (c1.v_Literal.size() != c2.v_Literal.size())
			return false;
		for (int i = 0; i < c1.v_Literal.size(); i++)
		{
			if (find(c2.v_Literal.begin(), c2.v_Literal.end(), c1.v_Literal[i]) == c2.v_Literal.end())// can't find literal of c1 in c2
				return false;
		}
		return true;
	}
};
struct SENTENCE // a LOGIC contains Clauses
{
	vector<CLAUSE> v_Clause;
	bool isSubset(SENTENCE s)
	{
		for (int i = 0; i < v_Clause.size(); i++)
		{
			if (find(s.v_Clause.begin(), s.v_Clause.end(), v_Clause[i]) == s.v_Clause.end())
				return false;
		}
		return true;
	}
	SENTENCE(){}
	SENTENCE(const SENTENCE &b)
	{
		for (int i = 0; i < b.v_Clause.size(); i++)
		{
			v_Clause.push_back(b.v_Clause[i]);
		}
	}
	friend bool operator==(const SENTENCE& s1, const SENTENCE &s2)
	{
		if (s1.v_Clause.size() != s2.v_Clause.size())
			return false;
		for (int i = 0; i < s1.v_Clause.size(); i++)
		{
			if (find(s2.v_Clause.begin(), s2.v_Clause.end(), s1.v_Clause[i]) == s2.v_Clause.end())// can't find clause of s11 in s2
				return false;
		}
		return true;
	}
};


//sentence Knowledge Base
SENTENCE m_sentence; // used to store Knowledge base
//sentence Result
SENTENCE m_logic; // used to store initial Knowledge base( Knowledge base and input resolution clause)
// input resolution clause
CLAUSE m_resolution; // input resolution clause

//------------------------------------------ function--------------------------------
/*
----use to analyze a string to each LITERAL and store into clause
----and among LITERALS is a token( token may be '|'(or) or '&'(and))
*/
void readClause(CLAUSE &clause, string s, char token)
{

	//--------------analyze a clause-----------------------------
	int i_old = s.find_first_of(token);
	int i_new;

	string tmp = s.substr(0, i_old);

	while (tmp != "")
	{
		LITERAL lit_tmp;
		if (tmp[0] == '~')
		{
			lit_tmp.sign = -1;
			lit_tmp.val = tmp.substr(1, tmp.length() - 1);
		}
		else
		{
			lit_tmp.sign = 1;
			lit_tmp.val = tmp;
		}
		clause.v_Literal.push_back(lit_tmp);
		//--------if this clause has only one literal-------
		if (i_old == -1)
			break;
		//-------------------------------------------------------
		//update
		i_new = s.find(token, i_old + 1);
		tmp = s.substr(i_old + 1, i_new - i_old - 1);
		
		i_old = i_new;
		if (i_old == -1)
		{
			if (tmp[0] == '~')
			{
				lit_tmp.sign = -1;
				lit_tmp.val = tmp[1];
			}
			else
			{
				lit_tmp.sign = 1;
				lit_tmp.val = tmp[0];
			}
			clause.v_Literal.push_back(lit_tmp);
			break;
		}
	}//end while
}
/*
---Used to print all of the LITERALs in a CLAUSE
*/
void outputClause(CLAUSE clause, ostream &os)
{
	for (int i = 0; i < clause.v_Literal.size(); i++)
	{
		if (clause.v_Literal[i].sign == -1)
		{
			os << "~" << clause.v_Literal[i].val;
		}
		else
		{
			os << clause.v_Literal[i].val;
		}
		if (i != clause.v_Literal.size() - 1)
			os << "|";
	}
}
/*
---Used to print all of the CLAUSES in a SENTENCE and among CLAUSES is a ','
*/
void outputSentence(SENTENCE sen, ostream &os)
{
	for (int i = 0; i < sen.v_Clause.size(); i++)
	{
		outputClause(sen.v_Clause[i], os);
		if (i != sen.v_Clause.size() - 1)
			os << ",";
	}
}

/*
---Used to check if two LITERALS are negative with each other or not
*/
bool isNegative(LITERAL a, LITERAL b)
{
	if (a.val == b.val)
		if (a.sign == -b.sign)
			return true;
	return false;// two literals are not negative
}

/*
---Return all clauses that can be obtained by resolving clauses a and b.
---For example: a = (~A|B|C), b = (~B|A)
---=> return [(~A | A | C), (B | ~B | C)]
*/
SENTENCE resolve(CLAUSE a, CLAUSE b)
{
	
	SENTENCE new_clauses;
	for (int ai = 0; ai < a.v_Literal.size(); ai++)
	{
		for (int bi = 0; bi < b.v_Literal.size(); bi++)
		{
			if (isNegative(a.v_Literal[ai], b.v_Literal[bi]) == true)
			{
				///remove these literals in a_tmp and b_tmp
				CLAUSE a_tmp(a), b_tmp(b);
				a_tmp.v_Literal.erase(a_tmp.v_Literal.begin() + ai);
				b_tmp.v_Literal.erase(b_tmp.v_Literal.begin() + bi);
				CLAUSE tmp(a_tmp);//copy from a_tmp
								  //copy from b_tmp
				for (int i = 0; i < b_tmp.v_Literal.size(); i++)
				{
					if (std::find(tmp.v_Literal.begin(), tmp.v_Literal.end(), b_tmp.v_Literal[i]) == tmp.v_Literal.end())//not exists in tmp
					{
						tmp.v_Literal.push_back(b_tmp.v_Literal[i]);
					}
				}
				//end copy
				//push to new_clauses
				new_clauses.v_Clause.push_back(tmp);
			}//end if
		}//end for
	}//end for
	return new_clauses;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
vector<pair<int, int> > m_v; // with each value in m_v contains a pair which is indexes of two clauses resolving into 
int r_i, r_j; // indexes of the last pair is resolved

/*
---Pre: old_clauses is initial Knowledge base( without resolving)
---		   new_clauses is Knowledge base after resolving
---			i, j are indexes of two clauses which resolve into a clause
*/
void backward(SENTENCE &old_clauses, SENTENCE new_clauses, int i, int j)
{
	//if i is not the index of clauses in Initial Knowledge base
	if (m_v[i].first != -1)
	{
		backward(old_clauses, new_clauses, m_v[i].first, m_v[i].second);
	}
	//if j is not the index of clauses in Initial Knowledge base
	if (m_v[j].first != -1)
	{
		backward(old_clauses, new_clauses, m_v[j].first, m_v[j].second);
	}
	//print
	//first find the clause which is resolved by i and j
	int index = -1;
	for (int k = 0; k < m_v.size(); k++)
	{
		if (m_v[k].first == i && m_v[k].second == j)
		{
			index = k;
			break;
		}
	}
	if (index == -1)
		return;
	//then find the two clauses with indexes i and j respectively in the old_clauses(initial Knowledge base(without resolving)
	vector<CLAUSE>::iterator it1 = std::find(old_clauses.v_Clause.begin(), old_clauses.v_Clause.end(),
		new_clauses.v_Clause[i]);
	vector<CLAUSE>::iterator it2 = std::find(old_clauses.v_Clause.begin(), old_clauses.v_Clause.end(),
		new_clauses.v_Clause[j]);
	// then delete these two clauses( because they are resolved)
	if (i < j)
	{
		old_clauses.v_Clause.erase(it2);
		old_clauses.v_Clause.erase(it1);
	}
	else
	{
		old_clauses.v_Clause.erase(it1);
		old_clauses.v_Clause.erase(it2);
	}
	//then push new resolved clause to old_clauses(initial Knowledge base)
	old_clauses.v_Clause.push_back(new_clauses.v_Clause[index]);
	//then print current Knowledge base
	outputSentence(old_clauses, cout);
	//print current KB to file
	outputSentence(old_clauses, f_out);
	f_out << endl;
	cout << endl;
}
bool pl_resolution(SENTENCE& clauses)
{
	SENTENCE new_clauses;
	
	// Recall: m_v is a vector
	//with each value in m_v contains a pair which is indexes of two clauses resolving into
	// it bring about that with each index of a clause in Initial Knowledge base, there is no pair which resolves into this clause.
	// Hence, I set pair(-1,-1) for every index in m_v from 0 to the length of Initial KB - 1
	for (int i = 0; i < clauses.v_Clause.size(); i++)
	{
		m_v.push_back(make_pair(-1, -1));
	}
	//
	while (true)
	{
		int length = clauses.v_Clause.size(); // the number of clauses in KB pluses input resolution
											  ////
		vector<pair<CLAUSE, CLAUSE> > pairs; // a pair is composed of 2 clauses.
		vector<pair<int, int> > pair_index; // with each pair, stores two indexes of two clauses of this pair.
		for (int i = 0; i < length; i++)
		{
			for (int j = i + 1; j < length; j++)
			{
				pairs.push_back(make_pair(clauses.v_Clause[i], clauses.v_Clause[j]));// push two clauses
				pair_index.push_back(make_pair(i, j));// push two indexes
			}
		}
		//
		SENTENCE resolvents;// stores Clauses after resolving two clauses
		//
		map<int, pair<int, int> > m_map;// map links each clause with a pair which contains two indexes of clauses
															// resolving into this clause.

		for (int i = 0; i < pairs.size(); i++)// resolving process
		{
			resolvents = resolve(pairs[i].first, pairs[i].second);//resolving
			//check if dual clauses exists or not
			for (int k = 0; k < resolvents.v_Clause.size(); k++)
			{
				if (resolvents.v_Clause[k].v_Literal.size() == 0)
				{
					r_i = pair_index[i].first;
					r_j = pair_index[i].second;
					return true; // two dual clauses exist
				}
				//if dual clauses have not appeared
				if (find(new_clauses.v_Clause.begin(), new_clauses.v_Clause.end(), resolvents.v_Clause[k])
					== new_clauses.v_Clause.end())// have not existed yet
				{
					new_clauses.v_Clause.push_back(resolvents.v_Clause[k]);
					//
					m_map[new_clauses.v_Clause.size() - 1] = make_pair(pair_index[i].first, pair_index[i].second);
				}
			}
		}//end for(end checking each pair)
		//check if new_clauses is a subset of clauses
		bool isSub = false;
		if (new_clauses.isSubset(clauses) == true)
			isSub = true;
		// if a clause in new_clauses have not existed in clauses
		// add it into clauses
		for (int i = 0; i < new_clauses.v_Clause.size(); i++)
		{
			if (find(clauses.v_Clause.begin(), clauses.v_Clause.end(), new_clauses.v_Clause[i]) == clauses.v_Clause.end())
			{
				clauses.v_Clause.push_back(new_clauses.v_Clause[i]);
				m_v.push_back(m_map[i]);
			}
		}
		if (isSub)
		{
			r_i = m_v[m_v.size() - 1].first;
			r_j = m_v[m_v.size() - 1].second;
			return false;
		}
	}//end while
	
}
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Wrong syntax\n";
	}
	else
	{//read file
		fstream f(argv[1]);
		if (!f)
		{
			cout << "Can't open this file.\n";
			return 0;
		}
		f_out.open("1512577_1512575.txt", ios::out);

		string s;// used to read each line in the file.

		//-----------READ KNOWLEDGE BASE----------------------
		getline(f, s);//  read first line of input.txt(KB)
		while (true)
		{
			CLAUSE clause;
			getline(f, s);
			if (s == "ENDKB")// stop reading Knowledge base
				break;
			readClause(clause, s, '|');
			//-------------------------------------------------------------------
			m_sentence.v_Clause.push_back(clause);// push a clause to SENTENCE sentence
		}
		//---------------------------END READ KNOWLEDGE BASE-------------------

		//---------------------------READ INPUT RESOLUTION-------------------------
		getline(f, s);//read input resolution
		f.close();

		readClause(m_resolution, s, '&');
		//negate input resolution
		CLAUSE negative_resolution;
		//get negative form of input resolution
		for (int i = 0; i < m_resolution.v_Literal.size(); i++)
		{
			LITERAL lit_tmp;
			lit_tmp.sign = -m_resolution.v_Literal[i].sign;
			lit_tmp.val = m_resolution.v_Literal[i].val;
			negative_resolution.v_Literal.push_back(lit_tmp);
		}
		//---------------------------END READ INPUT RESOLUTION-------------------------

		//-----------------------------------------RESOLUTION-----------------------------------------

		for (int i = 0; i < m_sentence.v_Clause.size(); i++)
		{
			//add clauses to m_logic(knowledge base)
			m_logic.v_Clause.push_back(m_sentence.v_Clause[i]);
		}
		m_logic.v_Clause.push_back(negative_resolution);//add negative form of input resolution to m_logic
		//output to screen and file
		outputClause(m_resolution, f_out); // output the input resolution
		outputClause(m_resolution, cout); // output the input resolution
		f_out << endl;
		cout << endl;
		outputSentence(m_logic, cout);// output the initial Knowledge base
		outputSentence(m_logic, f_out);// output the initial Knowledge base
		f_out << endl;
		//
		SENTENCE tmp_logic(m_logic);// COPY the main logic(initial Knowledge base)
		bool result = pl_resolution(tmp_logic);
		//-----------------------------------------------------------------------
		cout << endl;
		//-----------------------PRINT PROCESS OF RESOLUTION---------------
		backward(m_logic, tmp_logic, r_i, r_j);
		//----------------------------------------------------------------------------------------
		if (result == true)
		{
			cout << "True";
			f_out << "True";
		}
		else
		{
			cout << "False";
			f_out << "False";
		}
		f_out.close();
		//--------------------------------------FINISH---------------------------------------------
		system("pause");
	}
	return 0;
}

