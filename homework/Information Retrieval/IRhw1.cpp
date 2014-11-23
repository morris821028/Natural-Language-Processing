// IRhw1.cpp : 定義主控台應用程式的進入點。
//

#include <stdio.h>
#include <sstream>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <iostream>
#include <fstream>								// ifstream to read file
#include <string>
#include <time.h>
#include <algorithm>
using namespace std;

class CWord
{
public:
	string c;
	int Use_f;
};

class CReport
{
public:
	int TitleWord;
	int CommentWord;
	CWord title[50];
	CWord comment[100];
	CWord site;
	int id, id_f;
	CWord date[4];
	CWord publisher;
	CWord type;
};

class CInverted
{
public:
	string com;
	int Doc[10000];
	int Doc_num;
};

int _tmain(int argc, _TCHAR* argv[])
{
	CReport report[35200];
	istringstream SS1, SS2, SS3, SS4, SS5, SS6, SS;
	string temp;
	string word;
	int Total = 0;
	CInverted Index[20000];
	int Index_num = 0;
	clock_t st, ed;
	ifstream file("en09062011.news");
	if(!file)
	{
		printf("Error occur!\n");
		system("pause");
		return 0;
	}
	st = clock();
	for(int i = 0;; i++)
	{
		getline(file, temp);
		if(temp == "")											// EOF break
		{
			Total = i;
			printf("%d\n", i);
			break;
		}
		istringstream SS(temp);
		report[i].TitleWord = 0;
		while(SS >> word)
		{
			//transform(word.begin(),word.end(),word.begin(),tolower);
			report[i].title[report[i].TitleWord].c = word;
			report[i].title[report[i].TitleWord].Use_f = 0;
			report[i].TitleWord++;
		}

		getline(file, temp);
		istringstream SS1(temp);
		report[i].CommentWord = 0;
		while(SS1 >> word)
		{
			report[i].comment[report[i].CommentWord].c = word;
			report[i].comment[report[i].CommentWord].Use_f = 0;
			report[i].CommentWord++;
		}

		getline(file, temp);
		istringstream SS2(temp);
		SS2 >> report[i].site.c;
		report[i].site.Use_f = 0;

		getline(file, temp);
		istringstream SS3(temp);
		SS3 >> report[i].id;
		report[i].id_f = 0;

		getline(file, temp);
		istringstream SS4(temp);
		int j = 0;
		while(SS4 >> word)
		{
			if(j == 1)
			{
				transform(word.begin(),word.end(),word.begin(),tolower);
			}
			report[i].date[j].c = word;
			report[i].date[j].Use_f = 0;
			j++;
		}

		getline(file, temp);
		istringstream SS5(temp);
		SS5 >> report[i].publisher.c;
		report[i].publisher.Use_f = 0;

		getline(file, temp);
		istringstream SS6(temp);
		SS6 >> report[i].type.c;
		report[i].type.Use_f = 0;
		getline(file, temp);
	}
	ed = clock();
	file.close();
	printf("%f\n", (float)(ed - st)/CLOCKS_PER_SEC);

	// Inverted index
	
	for(int i = 0; i < Total; i++)
	{
		// 標題與所有比對
		st = clock();
		for(int j = 0; j < report[i].TitleWord; j++)
		{
			if(report[i].title[j].Use_f == 1)
			{
			}
			else
			{
				Index[Index_num].com = report[i].title[j].c;
				Index[Index_num].Doc_num = 0;
				Index[Index_num].Doc[Index[Index_num].Doc_num] = report[i].id;
				Index[Index_num].Doc_num++;
				Index_num++;
				for(int s = i; s < Total; s++)
				{
					for(int t = j+1; t < report[s].TitleWord; t++)
					{
						if(Index[Index_num].com == report[s].title[t].c)
						{
							report[s].title[t].Use_f = 1;
							Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
							if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
							{
								Index[Index_num].Doc_num++;
							}
						}
					}
					for(int t = 0; t < report[s].CommentWord; t++)
					{
						if(Index[Index_num].com == report[s].comment[t].c)
						{
							report[s].comment[t].Use_f = 1;
							Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
							if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
							{
								Index[Index_num].Doc_num++;
							}
						}
					}
					if(Index[Index_num].com == report[s].date[1].c)
					{
						report[s].date[1].Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
					if(Index[Index_num].com == report[s].publisher.c)
					{
						report[s].publisher.Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
					if(Index[Index_num].com == report[s].type.c)
					{
						report[s].type.Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
				}
			}
		}
		// 內容與所有比對
		for(int j = 0; j < report[i].CommentWord; j++)
		{
			if(report[i].comment[j].Use_f == 1)
			{
			}
			else
			{
				Index[Index_num].com = report[i].comment[j].c;
				Index[Index_num].Doc_num = 0;
				Index[Index_num].Doc[Index[Index_num].Doc_num] = report[i].id;
				Index[Index_num].Doc_num++;
				Index_num++;
				for(int s = i; s < Total; s++)
				{
					if(s != i)
					{
						for(int t = 0; t < report[s].TitleWord; t++)
						{
							if(Index[Index_num].com == report[s].title[t].c)
							{
								report[s].title[t].Use_f = 1;
								Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
								if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
								{
									Index[Index_num].Doc_num++;
								}
							}
						}
					}
					for(int t = j+1; t < report[s].CommentWord; t++)
					{
						if(Index[Index_num].com == report[s].comment[t].c)
						{
							report[s].comment[t].Use_f = 1;
							Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
							if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
							{
								Index[Index_num].Doc_num++;
							}
						}
					}
					if(Index[Index_num].com == report[s].date[1].c)
					{
						report[s].date[1].Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
					if(Index[Index_num].com == report[s].publisher.c)
					{
						report[s].publisher.Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
					if(Index[Index_num].com == report[s].type.c)
					{
						report[s].type.Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
				}
			}
		}
		// 日期
		for(int j = 0; j < 4; j++)
		{
			if(report[i].date[j].Use_f == 1)
			{
			}
			else
			{
				Index[Index_num].com = report[i].date[j].c;
				Index[Index_num].Doc_num = 0;
				Index[Index_num].Doc[Index[Index_num].Doc_num] = report[i].id;
				Index[Index_num].Doc_num++;
				Index_num++;
				for(int s = i+1; s < Total; s++)
				{
					for(int t = 0; t < report[s].TitleWord; t++)
					{
						if(Index[Index_num].com == report[s].title[t].c)
						{
							report[s].title[t].Use_f = 1;
							Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
							if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
							{
								Index[Index_num].Doc_num++;
							}
						}
					}
					for(int t = 0; t < report[s].CommentWord; t++)
					{
						if(Index[Index_num].com == report[s].comment[t].c)
						{
							report[s].comment[t].Use_f = 1;
							Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
							if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
							{
								Index[Index_num].Doc_num++;
							}
						}
					}
					for(int r = 0; r < 4; r++)
					{
						if(Index[Index_num].com == report[s].date[r].c)
						{
							report[s].date[r].Use_f = 1;
							Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
							if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
							{
								Index[Index_num].Doc_num++;
							}
						}
					}
				}
			}
		}
		// 出版
		if(report[i].publisher.Use_f == 0)
		{
			Index[Index_num].com = report[i].publisher.c;
			Index[Index_num].Doc_num = 0;
			Index[Index_num].Doc[Index[Index_num].Doc_num] = report[i].id;
			Index[Index_num].Doc_num++;
			Index_num++;
			for(int s = i+1; s < Total; s++)
			{
				for(int t = 0; t < report[s].TitleWord; t++)
				{
					if(Index[Index_num].com == report[s].title[t].c)
					{
						report[s].title[t].Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
				}
				for(int t = 0; t < report[s].CommentWord; t++)
				{
					if(Index[Index_num].com == report[s].comment[t].c)
					{
						report[s].comment[t].Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
				}
				if(Index[Index_num].com == report[s].publisher.c)
				{
					report[s].publisher.Use_f = 1;
					Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
					if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
					{
						Index[Index_num].Doc_num++;
					}
				}
			}

		}
		if(report[i].type.Use_f == 0)
		{
			Index[Index_num].com = report[i].type.c;
			Index[Index_num].Doc_num = 0;
			Index[Index_num].Doc[Index[Index_num].Doc_num] = report[i].id;
			Index[Index_num].Doc_num++;
			Index_num++;
			for(int s = i+1; s < Total; s++)
			{
				for(int t = 0; t < report[s].TitleWord; t++)
				{
					if(Index[Index_num].com == report[s].title[t].c)
					{
						report[s].title[t].Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
				}
				for(int t = 0; t < report[s].CommentWord; t++)
				{
					if(Index[Index_num].com == report[s].comment[t].c)
					{
						report[s].comment[t].Use_f = 1;
						Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
						if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
						{
							Index[Index_num].Doc_num++;
						}
					}
				}
				if(Index[Index_num].com == report[s].type.c)
				{
					report[s].type.Use_f = 1;
					Index[Index_num].Doc[Index[Index_num].Doc_num] = report[s].id;
					if(Index[Index_num].Doc[Index[Index_num].Doc_num] != Index[Index_num].Doc[Index[Index_num].Doc_num-1])
					{
						Index[Index_num].Doc_num++;
					}
				}
			}
		}
		printf("%d\n",i);
		ed = clock();
		printf("%f\n", (float)(ed - st)/CLOCKS_PER_SEC);
		
	}
	




	int test;
	while(1) {
		cin >> test;
		if(test == 99999) 
			break;
		for(int i = 0; i < report[test].TitleWord; i++)
			printf("%s ", report[test].title[i].c.c_str());
		printf("\n");
		for(int i = 0; i < report[test].CommentWord; i++)
			printf("%s ", report[test].comment[i].c.c_str());
		printf("\n");
		printf("%s\n", report[test].site.c.c_str());
		printf("%d\n", report[test].id);
		for(int i = 0; i < 4; i++)
			printf("%s ", report[test].date[i].c.c_str());
		printf("\n");
		printf("%s\n", report[test].publisher.c.c_str());
		printf("%s\n", report[test].type.c.c_str());
	}
	return 0;
}

