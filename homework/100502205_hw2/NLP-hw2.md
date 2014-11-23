title: '自然語言處理 - HW02'
date: 2014-11-10 15:03:13
tags: [自然語言, xml]
categories: [學校課程, 自然語言]
---

## 編譯環境 ##

Dev-C++ 5.6.3

## 作業內容 ##

給予 5000 筆的亞馬遜書店評論，每一個評論都已經分類好，總共有五種類型的書評，分別為 `book`, `dvd`, `health`, `music`, 以及 `toys_games`。

利用這 5000 筆分類好的書評寫一個分類器，接著讀入另外測試資料，測試資料每一個書評有既定分類，使用分類器判斷是否符合既定分類，分別計算對於每一種分類書評的準確度 (Accuracy) 和精準度 (Precision)。

* 準確度：接近正確答案的機率，在這裡可以理解為分類出來的結果等於實際結果的機率。
* 精準度：測試結果一致的機率，在這裡可以理解為分類出來的結果中，實際上是正確的機率。(是否集中於某個實際結果)

還有一個混用準確度和精準度的概念 f，假設準確度為 R，精準度為 P。

{% math-block %}
F = \frac{1}{\alpha  \frac{1}{P} + (1 - \alpha) \frac{1}{R}} = \frac{(\beta^{2} + 1) PR}{\beta^{2}P+R}
{% endmath-block %}

所謂的 F1 定義為 {% math \beta = 1, \alpha = 1/2 %}，最後得到 {% math F = 2PR / (P+R) %}

{% blockquote wiki http://en.wikipedia.org/wiki/Precision_and_recall %}

{% endblockquote %}

關於輸入輸出資料

* `gold_standard.xml` ： 黃金標準的 5000 筆評論
* `test_outcome.xml` ：原本認為是 ** 測試用的反饋資料，用來檢測寫的分類器好不好。 ** 後更正為助教的分類器跑出來的結果。

> 通常會拿到一大筆資料，經驗上 3/4 的資料會拿來訓練，剩下 1/4 會拿來檢測。也就是資料量於 3 : 1，至於一段資料怎麼切分有很多方式，可以擷取中間一段的 1/4 ... 等都可以。

看一下 xml 格式

{% codeblock gold_standard.xml %}
<RDF rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
  <Text category="music">Forget the fact that despite the grumbling
  about today's "music" ...</Text>
  <Text category="health">Printed all over the box are the words
  "with infrared heat". ...</Text>
  <Text category="music">I just finished seeing the movie and loved
  the song at the end ...</Text>
  ...
{% endcodeblock %}

看起來最多兩層，如果不用插件來解析 XML，遞迴處理寫一個 parser 應該不是問題。

於是就這麼蠻幹 XML parser 下去，仔細查閱一下給定的 XML，會發現文件中有 `&amp;amp;` 和 `&amp;amp;quot;` 的確在 XML 的元素 content 不會出現 `>`, `<` ... 等字元，就跟 HTML 的 character entities 一樣都需要被替換成特殊的顯示規格。但真沒有見過上述的那兩個格式，仔細檢查發現應該是對應 `and` 字串和 `"` 引號。解析完作 replace 動作。

回過頭來看這次所需要的公式：

{% math-block %}
P(c) = \frac{N_{c}}{N} \\
P(w|c) = \frac{count(w,c)+1}{count(c)+|V|} \\
P(c|s) = P(c) \prod P(w_{i}|c)
{% endmath-block %}

參數說明：

* {% math P(c) %} 表示該分類佔有群體的機率，也就是在 5000 筆評論中，分類 c 佔有百分比。{% math N_{c} %} 表示有多少筆評論屬於 c 分類。{% math N %} 表示評論筆數，這裡已知 {% math N = 5000 %}
* {% math P(w|c) %} 單詞 w 在分類 c 的出現機率為何，{% math count(w,c) %} 單詞 w 在分類 c 中出現的次數，{% math count(c) %} 屬於 c 分類中字詞總數 (評論總共有多少字)，{% math |V| %} 分類 c 中使用的單詞集合大小。
* {% math P(c|s) %} 評論句子 s 在分類 c 的機率為何。

隨後找 {% math P(c|s) %} 機率值最大作為分類依準。

最後要找到分類是否正確，對於每一種分類 c 會得到表格

|實際結果\分類結果|Classifier no   |Classifier yes|
|-----------|------------------|------------------|
|Truth no |true negative(TN)|false positive(FP)|
|Truth yes|false negative(FN) |true positive(TP) |

準確度 R 意即：`TP / (TP + FN)`、精準度 P 意即：`TP / (TP + FP)`

對於 Micro-average 和 Macro-average 的計算，Micro 和 Macro 概念上我理解程式幾何平均和算數平均的概念，Micro 會把每個分類 c 的表單結果累加到一個表格，隨後按照一般分類算法計算準確和精準度，而 Macro 則是將每個分類算完的結果取算術平均。

## 代碼 ##

傻傻地用分類器判斷與助教的決策比較的運行結果。

```
process gold_standard.xml ...
statistics model built ...
--------------------+----------
book                |      1000
dvd                 |      1000
health              |      1000
music               |      1000
toys_games          |      1000
--------------------+----------
process test_outcome.xml ...
testing test_outcome.xml ...
| AC\Assign|      book|       dvd|    health|     music|toys_games|
|      book|       892|        29|        17|         9|        25|
|       dvd|        22|       829|        16|        48|        41|
|    health|        30|        19|       975|        46|       177|
|     music|         5|        13|        12|       870|        28|
|toys_games|        10|        16|        43|        21|       807|
book
        p :0.9301355579
        r :0.9176954733
        f :0.9238736406
dvd
        p :0.9150110375
        r :0.8671548117
        f :0.8904403867
health
        p :0.9172154280
        r :0.7818765036
        f :0.8441558442
music
        p :0.8752515091
        r :0.9375000000
        f :0.9053069719
toys_games
        p :0.7486085343
        r :0.8996655518
        f :0.8172151899
Micro-average
        p :0.8746000000
        r :0.8746000000
        f :0.8746000000
Macro-average
        p :0.8772444134
        r :0.8807784681
        f :0.8790078886

--------------------------------
Process exited after 6.545 seconds with return value 0
```


{% blockquote wiki http://zh.wikipedia.org/wiki/ROC%E6%9B%B2%E7%BA%BF %}
後來發現是公式理解錯誤
* 真陽性 (TP, true positive)
    正確的肯定。又稱：命中 (hit)
* 真陰性 (TN, true negative)
    正確的否定。又稱：正確拒絕 (correct rejection)
* 偽陽性 (FP, false positive)
    錯誤的肯定，又稱：假警報 (false alarm)，第一型錯誤
* 偽陰性 (FN, false negative)
    錯誤的否定，又稱：未命中 (miss)，第二型錯誤
{% endblockquote %}

準確度就是在正確答案中找誤判，而精準度就是在判斷正確下找錯誤。

後來實驗用 ** gold_standard.xml ** train 和 test，運行結果為

```
| AC\Assign|      book|       dvd|    health|     music|toys_games|
|      book|       950|         7|        15|         3|        25|
|       dvd|         7|       895|        33|        21|        44|
|    health|         1|         0|       984|         1|        14|
|     music|         1|         3|        15|       968|        13|
|toys_games|         0|         1|        16|         1|       982|
book
        p :0.9906152242
        r :0.9500000000
        f :0.9698825932
dvd
        p :0.9878587196
        r :0.8950000000
        f :0.9391395593
health
        p :0.9256820320
        r :0.9840000000
        f :0.9539505574
music
        p :0.9738430584
        r :0.9680000000
        f :0.9709127382
toys_games
        p :0.9109461967
        r :0.9820000000
        f :0.9451395573
Micro-average
        p :0.9558000000
        r :0.9558000000
        f :0.9558000000
Macro-average
        p :0.9577890462
        r :0.9558000000
        f :0.9567934893
```

當然不能拿相同的 train 和 test data 來用，但是也明顯地發現，這個 model 仍然沒有辦法達到很高純度的結果，在部分比例中也只達到 90% 的精度。

那有沒有一種情況，我們將六種分類的共同交集字符給去除？這樣的效果會不會比較好呢？例如常用的 `I`、`think`、`it` ... 等，在 model 中直接無視這些常用字集的效果如何呢？

```
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
		count_category[it->first] = cnt;
	}
}
```

上述代碼為將每一個分類的字集與共同交集，並且將共同交集從每個分類中移除，同時將記數重新統計。根據實驗結果，很多分類都直接噴掉，也許是專有名詞使用的量太少，雖然每個字集仍然有上千上萬不等，但是在分類效應上某些分類完全消失。效果差到不行，所以就直接無視吧。或者提供點意見吧。

回過頭來解一下關於作業內容，其實作業內容可以化簡為：

> 輸入只有一組測資，該組測資會有數行，每一行上會有兩個分類結果，第一個分類結果為標準答案，第二個分類結果為根據模型運算的結果。對於測資輸出每一個分類的準確度、精準度以及 f1 的數據結果。

兩個輸入檔案可以壓縮為

### Input ###

```
5000
music music
book health
music music
toys_games book 
music music
... here are more rows
```

很明顯地，第二個評論中，誤把 book 類型判斷成 health。

### Output ###

```
| AC\Assign|      book|       dvd|    health|     music|toys_games|
|      book|       907|        25|        48|         7|        13|
|       dvd|        35|       873|        43|        24|        25|
|    health|        10|         2|       932|        11|        45|
|     music|         9|        46|        56|       865|        24|
|toys_games|        11|        10|       168|        21|       790|
book
        p :0.9331275720
        r :0.9070000000
        f :0.9198782961
dvd
        p :0.9131799163
        r :0.8730000000
        f :0.8926380368
health
        p :0.7473937450
        r :0.9320000000
        f :0.8295505118
music
        p :0.9321120690
        r :0.8650000000
        f :0.8973029046
toys_games
        p :0.8807134894
        r :0.7900000000
        f :0.8328940432
Micro-average
        p :0.8734000000
        r :0.8734000000
        f :0.8734000000
Macro-average
        p :0.8813053583
        r :0.8734000000
        f :0.8773348714

--------------------------------
Process exited after 1.736 seconds with return value 0
```

答案算出來當然跟助教一下啊，知道什麼是 sample output 嗎？在 ACMer 的眼中，代表根據算法不同，答案也許會有誤差，或者是測資不同造成答案不同。而 sample output 通常告訴我們的是輸出格式與可能情況，而非測資的正確答案。

> 我就是很笨，無法理解這個世界。不知道那檔案是有序對應，看到 attribute 名稱 name 一樣，內容 content 不一樣就自動補完無法理解的英文部分，而沒有去看 tag 中 inner content 內容是相同的。

{% codeblock lang:cpp %}
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
{% endcodeblock %}