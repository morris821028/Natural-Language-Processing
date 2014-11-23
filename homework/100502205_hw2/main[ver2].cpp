#include <stdio.h>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <sstream>
#include <math.h>
#include <ctype.h>
#include <string>
#include <string.h>
#include <set>
#include <algorithm>
using namespace std;
class XMLParser {
	public:
	struct Node {
		string tag_name, content;
		map<string, string> attr;
		vector<Node> child;
	};
	Node root; 
	XMLParser(string text = "") {
		sin << text;
		root = Node();
		build(root, "");
	}	
	void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    	if(from.empty())
        	return;
    	size_t start_pos = 0;
    	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        	str.replace(start_pos, from.length(), to);
        	start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    	}
	}
	string html2text(string s) {
		string ret(s);
		replaceAll(ret, "&amp;amp;quot;", "\"");
		replaceAll(ret, "&amp;amp;", "and");
		return ret;
	}
	private:
	stringstream sin;
	void getTagContent(Node &u) {
		char c, div;
		while (sin.get(c)) {
			if (c == '<') {
				while (sin.get(c) && isspace(c));
				u.tag_name = c;
				while (sin.get(c) && c != '>' && !isspace(c))
					u.tag_name += c;
				if (c == '>')	return;
				while (true) {
					while (sin.get(c) && isspace(c));
					if (c == '>')	return;
					string a = "", b = "";
					a += c;
					while (sin.get(c) && !isspace(c) && c != '=')
						a += c;
					while (sin.get(c) && (isspace(c) || c == '"' || c == '\'')) div = c;
					b += c;
					while (sin.get(c) && !isspace(c) && c != div)
						b += c;
					u.attr[a] = b;
				}
				return;
			}
		}
	}
	int build(Node &u, string last) {
		getTagContent(u);
		if (u.tag_name == last)
			return 0;
		while (sin.peek() != EOF && sin.peek() != '<') {
			char c;
			sin.get(c);
			u.content += c;
		}
		u.content = html2text(u.content);
		while (sin.peek() != EOF) {
			while (sin.peek() != EOF && isspace(sin.peek()))
				sin.get();
			if (sin.peek() == '<') {
				u.child.push_back(Node());
				if (!build(u.child[u.child.size() - 1], "/" + u.tag_name)) {
					u.child.pop_back();
					break;
				}
			}
		}	
		return 1;	
	}
};
class StatsModel {
	public: 
		map<string, map<string, int> > categories; 	// count(w, c)
		map<string, int > commentCount; 			// N_{c}
		map<string, int > count_category; 			// count(c)
		map<string, map<string, int> > judgeTable;
		int N, V;
		int microtable[2][2]; // [truth 0:1][classifier 0:1]
		StatsModel() {
			N = V = 0;
			memset(microtable, 0, sizeof(microtable));
		}
		vector<string> normalSentence(string content) {
			vector<string> w;
			stringstream sin(content);
			string token;
			while (sin >> token) {
				token = igntrim(token);
				token = str2lower(token);
				if (token.length() > 0)
					w.push_back(token);
			}
			return w;
		}
		void recordJudge(string truth, string classifier) {
			judgeTable[truth][classifier]++;
			for (map<string, int>::iterator it = commentCount.begin();
				it != commentCount.end(); it++) {
				int t = (it->first == truth);
				int c = (it->first == classifier);
				microtable[t][c]++;
			}
		}
		string judgeComment(string category, string content) {
			vector<string> w = normalSentence(content);
			double Pc, P;
			double maxPwc = - 1e+30;
			string chooseClass = "";
			for (map<string, int>::iterator it = commentCount.begin();
				it != commentCount.end(); it++) {
				Pc = (double)it->second / N;
				P = log(Pc);
				map<string, int> &R = categories[it->first];
				int count_c = count_category[it->first], count_w_c;
				for (int i = 0; i < w.size(); i++) {
					count_w_c = 0;
					if (R.find(w[i]) != R.end())
						count_w_c = R[w[i]];
					P += log((double) (count_w_c + 1)/ (count_c + R.size()));
				}
				if (P > maxPwc) 
					maxPwc = P, chooseClass = it->first;
			}
			recordJudge(category, chooseClass);
			return chooseClass;
		}
		void addComment(string category, string content) {
			commentCount[category]++, N++;
			map<string, int> &S = categories[category];
			vector<string> w = normalSentence(content);
			for (int i = 0; i < w.size(); i++)
				S[w[i]]++;
			count_category[category] += w.size(), V += w.size();
		}
		string igntrim(string s) {
			while (s.size() > 0) {
				if (isspace(s[0]) || s[0] == '.' || s[0] == ',' 
					|| s[0] == '!' || s[0] == '"' || s[0] == '\'')
					s = s.substr(1);
				else {
					int t = s.length();
					if (isspace(s[t-1]) || s[t-1] == '.' || s[t-1] == ',' 
						|| s[t-1] == '!' || s[t-1] == '"')
						s = s.substr(0, t-1);
					else
						return s;
				}
			}
			return s;
		}
		string str2lower(string s) {
			for (int i = 0; i < s.length(); i++)
				s[i] = tolower(s[i]);
			return s;
		}
		void custom() {
			set<string> common;
			int first_set = 1;
			for (map<string, map<string, int> >::iterator it = categories.begin();
				it != categories.end(); it++) {
				map<string, int> &R = it->second;
				set<string> s;
				for (map<string, int>::iterator jt = R.begin(); 
					jt != R.end(); jt++)
					s.insert(jt->first);
				if (first_set) 	common = s, first_set = 0;
				else {
					set<string> C;
					set_intersection(common.begin(), common.end(), s.begin(), s.end(), inserter(C, C.begin()));
					common = C;
				}
			}
			for (map<string, map<string, int> >::iterator it = categories.begin();
				it != categories.end(); it++) {
				map<string, int> &R = it->second;
				for (set<string>::iterator jt = common.begin();
					jt != common.end(); jt++)
					R.erase(*jt);
				int cnt = 0;
				for (map<string, int>::iterator jt = R.begin();
					jt != R.end(); jt++)
					cnt += jt->second;
//				printf("%d -> %d\n", count_category[it->first], cnt);
				count_category[it->first] = cnt;
			}
//			printf("%d\n", common.size());
		}
		void print() {
			printf("%-20s+%10s\n", string(20, '-').c_str(), string(10, '-').c_str());
			for (map<string, int>::iterator it = commentCount.begin();
				it != commentCount.end(); it++) {
				printf("%-20s|%10d\n", it->first.c_str(), it->second);
			}
			printf("%-20s+%10s\n", string(20, '-').c_str(), string(10, '-').c_str());
		}
		void report() {
			// print <judge table>
			printf("|%10s|", "AC\\Assign");
			for (map<string, int>::iterator it = commentCount.begin();
				it != commentCount.end(); it++)
				printf("%10s|", it->first.c_str());
			puts("");
			for (map<string, int>::iterator it = commentCount.begin();
				it != commentCount.end(); it++) {
				printf("|%10s|", it->first.c_str());
				for (map<string, int>::iterator jt = commentCount.begin();
					jt != commentCount.end(); jt++) {
					printf("%10d|", judgeTable[it->first][jt->first]);
				}
				puts("");
			}
			// homework format
			// recall: fraction of docs in class i classified correctly
			// precision: fraction of docs assigned class i that are actually about class i
			const double beta = 1;
			double micro_r = 0, micro_p = 0, macro_r = 0, macro_p = 0, f1;
			for (map<string, int>::iterator it = commentCount.begin();
				it != commentCount.end(); it++) {
				double recall = 0, precision = 0, f1 = 0;
				int sumr = 0, sump = 0;
				for (map<string, int>::iterator jt = commentCount.begin();
					jt != commentCount.end(); jt++) {
					sumr += judgeTable[it->first][jt->first];
					sump += judgeTable[jt->first][it->first];
				}
				recall = (double) judgeTable[it->first][it->first] / sumr;
				precision = (double) judgeTable[it->first][it->first] / sump;
				f1 = (beta * beta + 1) * precision * recall / (beta * beta * precision + recall);
				
				macro_r += recall, macro_p += precision;
				
				printf("%s\n", it->first.c_str());
				printf("%9s :%.10lf\n", "p", precision);
				printf("%9s :%.10lf\n", "r", recall);
				printf("%9s :%.10lf\n", "f", f1);
			}
//			for (int i = 0; i < 2; i++, puts("")) 
//				for (int j = 0; j < 2; j++)
//					printf("%5d ", microtable[i][j]);
			// [truth 0:1][classifier 0:1] = { {TN, FP}, {FN, TP} } 
			micro_r = (double) microtable[1][1] / (microtable[1][1] + microtable[1][0]); // TP / (TP + FN)
			micro_p = (double) microtable[1][1] / (microtable[1][1] + microtable[1][0]); // TP / (TP + FP)
			f1 = (beta * beta + 1) * micro_p * micro_r / (beta * beta * micro_p + micro_r);
			printf("%s\n", "Micro-average");
			printf("%9s :%.10lf\n", "p", micro_p);
			printf("%9s :%.10lf\n", "r", micro_r);
			printf("%9s :%.10lf\n", "f", f1);
			
			macro_r /= commentCount.size();
			macro_p /= commentCount.size();
			f1 = (beta * beta + 1) * macro_p * macro_r / (beta * beta * macro_p + macro_r);
			printf("%s\n", "Macro-average");
			printf("%9s :%.10lf\n", "p", macro_p);
			printf("%9s :%.10lf\n", "r", macro_r);
			printf("%9s :%.10lf\n", "f", f1);
		}
};
int main() {
	ifstream fin("gold_standard.xml");
	string xml = "", line;
	while (getline(fin, line))
		xml += "\n" + line;
	printf("process gold_standard.xml ...\n");
	
	XMLParser xmlDoc(xml);
	StatsModel statsModel;
	
	printf("statistics model built ...\n");
	for (int i = 0; i < xmlDoc.root.child.size(); i++) {
		string 	a = xmlDoc.root.child[i].attr["category"],
				b = xmlDoc.root.child[i].content;
		statsModel.addComment(a, b);
	}
	
	statsModel.print();
//	statsModel.custom();
	
	ifstream tin("test_outcome.xml");
	
	xml = "";
	while (getline(tin, line))
		xml += "\n" + line;
	printf("process test_outcome.xml ...\n");
	
	XMLParser testDoc(xml);
	
	printf("testing test_outcome.xml ...\n");
	for (int i = 0; i < testDoc.root.child.size(); i++) {
		statsModel.recordJudge(xmlDoc.root.child[i].attr["category"], testDoc.root.child[i].attr["category"]);
	}
	
	statsModel.report();
	
	return 0;
}
/*

*/
