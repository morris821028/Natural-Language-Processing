#include <stdio.h>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <sstream>
#include <math.h>
#include <ctype.h>
using namespace std;
#define DEBUG
class Model {
	public:
	map<string, int> Count;
	map<pair<string, string>, int> Count2;
	map<int, int> Ni;
	map<int, int> Ni2;
	int totalCountWord, totalCount2Word;
	static const int K;
	static const int V;
	string str2lower(string s) {
		for (int i = 0; i < s.length(); i++)
			s[i] = tolower(s[i]);
		return s;
	}
	void prepare() {
		totalCountWord = totalCount2Word = 0;
		for (map<string, int>::iterator it = Count.begin();
			it != Count.end(); it++) {
			Ni[it->second]++;
			totalCountWord += it->second;
//#ifdef DEBUG
//			if (it->second > 1000)
//			printf("Count(%s) = %d\n", it->first.c_str(), it->second);
//#endif
		}	
		for (map<pair<string, string>, int>::iterator it = Count2.begin();
			it != Count2.end(); it++) {
			Ni2[it->second]++;
			totalCount2Word += it->second;
		}		
		int smooth = 0x3f3f3f3f;
		for (map<int, int>::iterator it = Ni.begin();
			it != Ni.end(); it++) {
//			smooth = min(smooth, it->second);
//			it->second = smooth;
//#ifdef DEBUG
//			printf("N(%d) = %d\n", it->first, it->second);
//			getchar();
//#endif
		}
	}
	double getN(int i) { // N_{Count(w)}
		if (i == 0)	return 80000;
		map<int, int>::iterator it = Ni.lower_bound(i), jt;
		if (it == Ni.begin())	return it == Ni.end() ? 1 : it->second;
		jt = it, jt--;
		double sx = jt->first, sy = jt->second, ex = it->first, ey = it->second;
		return sy + (ey - sy) / (ex - sx) * (i - sx);
	}
	double getN2(int i) { // N_{Count(w_{i}, w_{i+1})}
		if (i == 0)	return 80000.0 * 80000.0;
		map<int, int>::iterator it = Ni2.lower_bound(i), jt;
		if (it == Ni2.begin())	return it == Ni2.end() ? 1 : it->second;
		jt = it, jt--;
		double sx = jt->first, sy = jt->second, ex = it->first, ey = it->second;
		return sy + (ey - sy) / (ex - sx) * (i - sx);
	}
	double getCountStar(const string &w) { // Count^{*}(w_{i})
		int n = Count[w];
		if (n < K) {
			return (double) (n + 1) * (getN(n + 1) / getN(n));
		} else {
			return n;
		}
	}
	double getCountStar2(const string &w1, const string &w2) { // Count^{*}(w_{i}, w_{i+1})
		int n = Count2[make_pair(w1, w2)];
		if (n < K) {
			return (double) (n + 1) * (getN2(n + 1) / getN2(n));
		} else {
			return n;
		}
	}
	double getPofW1(const string &w) { // P(w_{i}) = \frac{Count(w_{i})}{\sum_{j} Count(w_{j})}
		map<string, int>::iterator it = Count.find(w);
		if (it == Count.end() || it->second == 0) { // Count(w_{i}) = 0
			double Nu0 = V - Count.size();
			return (double) getN(1) / Nu0 / totalCountWord; // \frac{N_{1}}{N}
		} else if (it->second < K) { // 0 < Count(w_{i}) < K
			return (double) getCountStar(w) / totalCountWord; // \frac{Count^{*}(w_{i})}{N}
		} else { // Count(w_{i}) \geq K
			return (double) it->second / totalCountWord; // \frac{Count(w_{i})}{N}
		}
	}
	double getPofW2(const string &w1, const string &w2) { // P(w_{i}, w_{i+1})
		map< pair<string, string>, int>::iterator it = Count2.find(make_pair(w1, w2));
		if (it == Count2.end() || it->second == 0) {
			double Nb0 = (double) V * V - Count2.size();
			return (double) getN2(1) / Nb0 / totalCount2Word;
		} else if (it->second < K) { // Count(w_{i}, w_{i+1}) < K
			return (double) getCountStar2(w1, w2) / totalCount2Word; // \frac{Count^{*}(w_{i}, w_{i+1})}{N}
		} else { // Count(w_{i}, w_{i+1}) \geq K
			return (double) it->second / totalCount2Word; // \frac{Count(w_{i}, w_{i+1})}{N}
		}
	}
	void parseStmt(vector<string> &stmt) {
		for (int i = 0; i < stmt.size(); i++) {
			stmt[i] = str2lower(stmt[i]);
			Count[stmt[i]]++;
			if (i)
				Count2[make_pair(stmt[i-1], stmt[i])]++;
		}
	}
	double getPs(string in) {
		stringstream sin(in);
		vector<string> stmt;
		string token;
		while (sin >> token)
			stmt.push_back(str2lower(token));
		// P(s) = P(w_{0}) \times P(w_{1}|w_{0}) \times ... \times P(w_{n-1}|w_{n-2})
		double p = 1;
		if (stmt.size() > 0)
			p = getPofW1(stmt[0]);
		for (int i = 1; i < stmt.size(); i++ ) {
			p *= getPofW2(stmt[i-1], stmt[i]) / getPofW1(stmt[i-1]);
//			printf("%lf\n", getPofW2(stmt[i-1], stmt[i]) / getPofW1(stmt[i-1]));
//			cout << stmt[i-1] << " "  << stmt[i]  << endl; 
//			printf("%lf\n", getPofW2(stmt[i-1], stmt[i]));
		}
		return p;
	}
} tool;
const int Model::K = 10, Model::V = 80000;
int main() {
	ifstream fin("dataset.txt");
//	freopen("tmp.txt", "w+t", stdout); 
	string token, line;
	vector<string> stmt;
	puts("processing dataset.txt ...");
	while (getline(fin, line)) {
		stringstream sin(line);
		stmt.clear();
		while (sin >> token) {
			stmt.push_back(token);
		}
		tool.parseStmt(stmt);
	}
	puts("read file end. Start prepare language model ...");
	tool.prepare();
	puts("input a sentence in a line :");
	while (getline(cin, line)) {
		printf("P*('%s') : %.10e\n", line.c_str(), tool.getPs(line));
	}
	return 0;
}
/*
causes AIDS and is associated with AIDS cases primarily in West Africa
AIDS cases primarily in West Africa
AIDS cases primarily at West Africa
AIDS cases primarily AIDS West Africa
morris
IL-2 is a gene .
IL-2 is an gene .
*/
	

