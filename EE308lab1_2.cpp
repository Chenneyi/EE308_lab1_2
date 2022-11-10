#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib> 
#include <stack>
#include <vector>

using namespace std;
const string keywords[] = {"auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","int","long","register","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while"};
vector<string> file_content;

void file_read(string filename)
{
	ifstream inFile;
	string line;
	inFile.open(filename.c_str());
	while (getline(inFile, line)) {
		file_content.push_back(line);
	} 
	inFile.close(); 
}

void del_useless_cont()
{
	string Line,words;
	int index;
	size_t tabs, annotation, quot, star1, star2;
	
	for(int x=0;x<file_content.size(); x++){
		Line=file_content[x];
		if(Line.find("\t") != Line.npos && Line.find("\t") == 0){
			do{
				tabs = Line.find("\t")+1;
				Line = Line.substr(tabs,Line.length()-tabs);
			}while(Line.find("\t") != Line.npos && Line.find("\t") == 0);
			file_content[x] = Line;
		}
		if (Line.find("//") != Line.npos) {    
			annotation = Line.find("//");       
			if (annotation == 0) {
				file_content.erase(file_content.begin()+x);      
				x--;
			} else {
				words = Line.substr(0,annotation);       
				file_content[x] = words;
			}
		}else if(Line.find("\"") != Line.npos){
			size_t temp[50] = {0};
			tabs = 0;
			index = 0;
			while ((quot = Line.find("\"",tabs)) != Line.npos) {
				temp[index] = quot;
				index++;
				tabs = quot +1;
			}
			temp[index] = Line.length();
			words= Line.substr(0,temp[0]);
			for (int y=1; temp[y]!=0; y+=2) {
				words += Line.substr(temp[y]+1,temp[y+1]-temp[y]-1);      
			}
			file_content[x]=words;	
		} else if(Line.find("/*") != Line.npos){
			star1 = Line.find("/*");
			star2 = Line.find("*/");
			if (star2 != Line.npos) {   
				if (star1 == 0) {
					file_content[x] = Line.substr(star2+2,Line.length());
				} else {
					file_content[x] = Line.substr(0,star1);
				}
				x--;
			}else{
				file_content[x] = Line.substr(0,star1);
				x += 1;
				Line = file_content[x];
				while (Line.find("*/") == Line.npos) {
					file_content.erase(file_content.begin()+x);     
					Line = file_content[x];
				}
				star2 = Line.find("*/")+2;
				file_content[x] = Line.substr(star2,Line.length()-star2);
			}
			
		}
	}
}
bool judge_keywords(string line,long x)
{
	if (line[x] < 48 || (line[x] > 57 && line[x] < 65) || (line[x] > 90 && line[x] < 97) ||line[x] >122) {
		return true;
	} else {
		return false;
	}
}

bool optimize_judge(string line, string word)
{
	size_t word_loc;
	word_loc = line.find(word);
	if (word_loc == 0) {
		if (judge_keywords(line, word.length()) || word.length() == line.length()) {
			return true;
		} else {
			return false;
		}
	} else {
		if (judge_keywords(line, word_loc-1) && (judge_keywords(line, word_loc+word.length()) || word_loc+word.length() == line.length())) {
			return true;
		} else {
			return false;
		}
	}
	
}
void cal_keywords_num()
{
	int k_num=0;
	string line;
	for (int x=0; x<file_content.size(); x++) {
		line = file_content[x];
		for (int y=0; y<32; y++) {
			size_t keywords_loc = line.find(keywords[y], 0);
			while (keywords_loc!=line.npos && optimize_judge(line,keywords[y])) {
				k_num++;
				keywords_loc = line.find(keywords[y],keywords_loc + 1);
			}
		}
	}
	cout << "total num: " << k_num << endl;
}
void cal_switch_case_num()
{
	int switch_num = 0,last = -1,case_num[100]={0};
	string line;
	for (int x=0; x<file_content.size(); x++) {
		line = file_content[x];
		if (line.find("switch") != line.npos && optimize_judge(line, "switch")) {        
			switch_num += 1;
			last += 1;
		}
		if (line.find("case") != line.npos && optimize_judge(line, "case")) {      
			case_num[last] += 1;
		}
	}
	cout << "switch num: " << switch_num << endl;
	cout << "case num:";
	for (int y=0; y<=last; y++) {
		cout << " " << case_num[y];
	}
	cout << endl;	
}
void cal_if_else_num(int l)
{
	int if_else_num = 0, if_elseif_else_num= 0;
	stack<int> s;
	string line;
	for (int x=0; x<file_content.size(); x++) {
		line = file_content[x];
		if (line.find("if") != line.npos && line.find("else") == line.npos && optimize_judge(line, "if")) {      
			s.push(1);
		} else if (line.find("if") == line.npos && line.find("else") != line.npos &&  s.empty() == false) {
			if (s.top() == 1) {
				if_else_num++;    
			} else {
				if_elseif_else_num++;    
			}
			s.pop();       
			
		}else  if (line.find("if") != line.npos && line.find("else") != line.npos && optimize_judge(line, "if")) {
			s.push(2);    
		}
	}
	
	if (l==3)
	{
		cout << "if-else num: " << if_else_num << endl;
	}
	if (l==4)
	{
		cout << "if-else num: " << if_else_num << endl;
		cout << "if-else-if num: " << if_elseif_else_num << endl;
	}   
}
int main() 
{
	int level = 0;
	string filename;
	cout << "please enter the path of the code file: " << endl;
	cin >> filename; 
	cout << "please input the level:"<<endl;
	cin>>level;
	file_read(filename);
	del_useless_cont();  
	if (level==1)
	{
		cal_keywords_num();
	}
	else if (level==2)
	{
		cal_keywords_num();
		cal_switch_case_num();
	}
	else if (level==3)
	{
		cal_keywords_num();
		cal_switch_case_num();
		cal_if_else_num(level);
	}
	else
	{
		cal_keywords_num();
		cal_switch_case_num();
		cal_if_else_num(level);
	}   
	return 0;
}
