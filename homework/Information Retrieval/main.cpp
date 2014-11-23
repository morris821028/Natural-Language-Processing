#include <stdio.h>
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;

class ParseTool {
	public:
		static vector<string> getToken(string s) {
			stringstream sin(s);
			string token;
			vector<string> ret;
			while (sin >> token)
				ret.push_back(token);
			return ret;
		}
		static set<string> getTokenSet(string s) {
			stringstream sin(s);
			string token;
			set<string> ret;
			while (sin >> token)
				ret.insert(token);
			return ret;
		}
		static map<string, vector<int> > parse(vector< set<string> > article, vector<int> aid) {
			map<string, vector<int> > ret;
			for (int i = 0; i < article.size(); i++) {
				for (set<string>::iterator it = article[i].begin();
					it != article[i].end(); it++) {
					ret[*it].push_back(aid[i]);
				}
			}
			return ret;
		}
		static int str2num(string s) {
			stringstream sin(s);
			int x;
			sin >> x;
			return x;
		}
};

class CWord {
	public:
		string str;
		int id;
		CWord(string s = "", int iid = -1):
			str(s), id(iid) {}
		static vector<CWord> toVector(vector<string> v, int iid) {
			vector<CWord> ret;
			for (int i = 0; i < v.size(); i++)
				ret.push_back(CWord(v[i], iid));
			return ret;
		}
};

class CReport {
	public:		
		void prepare(string s) {
			token_set.clear();
			token_set = ParseTool::getTokenSet(s);
		}
		vector<CWord> title;
		vector<CWord> content;
		CWord site_url;
		int id, id_f;
		vector<CWord> date;
		CWord publisher;
		CWord type;
		set<string> token_set;
		CReport(string format, int iid) {			
			prepare(format);
			id = iid;
			
			istringstream sin(format);
			string line;
			
			getline(sin, line);
			title = CWord::toVector(ParseTool::getToken(line), id);
			getline(sin, line);
			content = CWord::toVector(ParseTool::getToken(line), id);
			getline(sin, line);
			site_url = line;
			getline(sin, line);
			id = ParseTool::str2num(line);
			getline(sin, line);
			date = CWord::toVector(ParseTool::getToken(line), id);			
			getline(sin, line);
			publisher = line;
			getline(sin, line);
			type = line;
		}
		void print() {
			printf("Title ID : %d\n", id);
			printf("Title : `");
			for (int i = 0; i < title.size(); i++)
				printf("%s%c", title[i].str.c_str(), i != title.size() - 1 ? ' ' : '`');
			puts("");
			printf("Date : ");
			for (int i = 0; i < date.size(); i++)
				printf("%s%c", date[i].str.c_str(), i != date.size() - 1 ? ' ' : '\n');
			printf("Publisher : %s\n", publisher.str.c_str());
			printf("News_type : %s\n", type.str.c_str());
			puts("\n------");
			for (int i = 0; i < content.size(); i++)
				printf("%s ", content[i].str.c_str());
			puts("");
			puts("#----------------------------------------------#");
		}
};
vector<CReport> reports;
map<int, int> id_reports;
int main() {
	ifstream file("en09062011.news");
	
	string line, format;
	while (getline(file, line)) {
		format = line;
		while (getline(file, line)) {
			if (line == "")	break;
			format = format + '\n' + line;
		}
		reports.push_back(CReport(format, reports.size()));
		id_reports[reports[reports.size()-1].id] = reports.size() - 1;
	}
	
	vector< set<string> > news_tokens;
	vector< int > news_id;
	for (int i = 0; i < reports.size(); i++) {
		news_tokens.push_back(reports[i].token_set);
		news_id.push_back(reports[i].id);
	}
	map <string, vector<int> > tokenInWhere = ParseTool::parse(news_tokens, news_id);
	
	puts("Find token in ?");
	while (cin >> line) {
		cout << "find -> " << line << endl;
		vector<int> &vec = tokenInWhere[line];
		for (int i = 0; i < vec.size(); i++) {
			reports[id_reports[vec[i]]].print();
		}
		puts("--- more --- (press)");
	}
	return 0;
}
