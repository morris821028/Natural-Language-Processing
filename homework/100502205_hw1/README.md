title: '自然語言處理 - HW01'
date: 2014-10-25 20:59:20
tags: [自然語言]
categories: [學校課程, 自然語言]
---

## 編譯環境 ##

Dev-C++ 5.6.3

##  Language model implementation ##

實作語言處理模型，要求讀入一連串的英文語句做為基底，接著詢問一句的正確性為多少，也就是這句話是正確拼讀的機率為何，算是一個可用的句子嗎？

{% math-block %}
P(w_{i}) = \frac{Count(w_{i})}{\sum_{j} Count(w_{j})} \\
P(w_{i}, w_{i+1}) = \frac{Count(w_{i}, w_{i+1})}{\sum_{j} Count(w_{j}, w_{j+1})} \\
P(w_{i+1}|w_{i}) = \frac{P(w_{i}, w_{i+1})}{P(w_{i})}
{% endmath-block %}

上述分別是一個單詞的機率，以及計算相鄰兩個單詞的機率，最後推估在這個單詞下，它們相鄰的機率。請查閱 ** 貝氏定理 **。

{% blockquote 貝氏定理（Bayes' theorem） http://zh.wikipedia.org/wiki/%E8%B4%9D%E5%8F%B6%E6%96%AF%E5%AE%9A%E7%90%86 %}
* P(A)是A的先驗機率或邊緣機率。之所以稱為"先驗"是因為它不考慮任何B方面的因素。
* P(A|B)是已知B發生後A的條件機率，也由於得自B的取值而被稱作A的後驗機率。
* P(B|A)是已知A發生後B的條件機率，也由於得自A的取值而被稱作B的後驗機率。
* P(B)是B的先驗機率或邊緣機率，也作標准化常量（normalizing constant）.
{% endblockquote %}

{% math-block %}
P(s) = P(w_{0}) \times P(w_{1}|w_{0}) \times ... \times P(w_{n-1}|w_{n-2})
{% endmath-block %}

上述為一個句子的機率，一個句子可以表示成一個序列單詞，利用連乘積將機率算出。當句子越常時，很容易發現機率陡降的速度相當快，就算是數個字詞，機率大約也是在 {% math 10^{-3} %} 左右。因此這麼算下去，長句子的正確性就會大幅減少，但是在邏輯上，如果句子都是短短幾個單詞也是相當奇怪的，口語上也許是，文章判讀上就難說。至於要不要取 {% math \sqrt[n]{x} %} 值得思考。

{% math-block %}
\begin{cases}
Count^{*}(w_{i}) = (Count(w_{i})+1) \times \frac{N_{Count(w_{i})+1}}{N_{Count(w_{i})}} & \text{ if } Count(w_{i}) < k \\ 
Count^{*}(w_{i}) = Count(w_{i}) & \text{ if } Count(w_{i}) \geq k 
\end{cases} \\
\text{unigram } N_{0} = 80000 
{% endmath-block %}

當我們去計算一個單詞的機率時，必須相對於其他單詞的出現機率，然而像介係詞、助詞等，出現次數是相對高出取多，必須取一個閥值，來忽略過高無用的機率，以免將其他的單詞的機率過低不均。

{% math-block %}
\begin{cases}
Count^{*}(w_{i}, w_{i+1}) = (Count(w_{i}, w_{i+1})+1) \times \frac{N_{Count(w_{i}, w_{i+1})+1}}{N_{Count(w_{i}, w_{i+1})}} & \text{ if } Count(w_{i}, w_{i+1}) < k \\ 
Count^{*}(w_{i}, w_{i+1}) = Count(w_{i}, w_{i+1}) & \text{ if } Count(w_{i}, w_{i+1}) \geq k
\end{cases} \\
\text{bigram } N_{0} = 80000 \times 80000
{% endmath-block %}

在計算相鄰兩個單詞的出現次數時，一樣會遇到這種情況。既然在計算次數上需要做 smoothing 的調整，在機率處理上也是一樣動作。

{% math-block %}
\begin{cases}
 P(w_{i}) = \frac{N_{1}}{N} & \text{ if } Count(w_{i}) = 0 \\ 
 P(w_{i}) = \frac{Count^{*}(w_{i})}{N} & \text{ if } Count(w_{i}) < K \\ 
 P(w_{i}) = \frac{Count(w_{i})}{N} & \text{ if } Count(w_{i}) \geq K
\end{cases}
{% endmath-block %}

{% math-block %}
\begin{cases}
 P(w_{i}, w_{i+1}) = \frac{Count^{*}(w_{i}, w_{i+1})}{N} & \text{ if } Count(w_{i}, w_{i+1}) < K \\ 
 P(w_{i}, w_{i+1}) = \frac{Count(w_{i}, w_{i+1})}{N} & \text{ if } Count(w_{i}, w_{i+1}) \geq K
\end{cases}
{% endmath-block %}

在單詞出現次數很少時，就必須使用 smoothing，因為出現 1 次、2 次、3 次 ... 的單詞次數，之間大小關係不保證嚴格遞增或遞減，

## 實作細節 ##

* 公式都已經給定，基本上麻煩就是在於資料紀錄而已，其他就照著流程跑。
* 雖然沒有規定語言，以下代碼在 Dev-C++ 下編過，別問 VS 到底行不行。

原則上，讀檔案，建立模型可以在 1.4 秒內完成

```
processing dataset.txt ...
read file end. Start prepare language model ...
input a sentence in a line :
  
--------------------------------
Process exited after 1.493 seconds with return value 0
```

## 測試輸入 ##

```
causes AIDS and is associated with AIDS cases primarily in West Africa
AIDS cases primarily in West Africa
AIDS cases primarily at West Africa
AIDS cases primarily AIDS West Africa
morris
West Africa
East Africa
Taiwan Africa
```

## 測式輸出 ##

考量長句子的機率，採用 log 平均結果，輸出應該為負值。

```
-4.597715
-4.796396
-5.992288
-7.245960
-1.857392
-4.639120
-8.003706
-8.003706
```

原始版本，直接輸出 P(s)，這裡採用科學記號表示法。

```
1.101764e-026
2.622182e-015
6.068425e-019
9.372080e-023
2.436068e-002
9.031667e-007
3.733396e-011
3.733396e-011
```

## 結語 ##

有 STL library，代碼不用太長，跑得時間也不會太久。想到插手別學校的自然語言處理，玩了一下他們作業，把同學的代碼效率修得好一點，其實也不錯玩得，只是在公式計算上比較沒有，但是要求高效率的搜索結構，讀檔案進來後，依據什麼建表、該用什麼順序完成是很重要的。

切記，在寫這種程式時，發現跑的速度太久，一部分是因為使用太多的標準輸出，也就是用太多 `printf()`、`cout <<` 或 `System.out.println()` 之類的在進行流程監控。輸出的效率必須將檔案寫到螢幕顯示的 memory buffer，因此 context-switch 什麼，消耗時間太大，盡可能不要輸出，要不然就是按比例輸出，速度可以快個數百倍。

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

{% endcodeblock %}