#include<string>

#include<vector>
#include<stack>
#include<queue>

#include<fstream>
#include<sstream>
#include<regex>

MarkdownTrans::MarkdownTrans(std::string mdfile_path)
{
	file_path = mdfile_path;
}

MarkdownTrans::~MarkdownTrans()
{
}

std::string MarkdownTransform::transform()
{
	std::string outFile = "";// file to output
	std::ifstream inFile;
	std::queue<char> escapeCharQueue; //to store escape chars
	inFile.open(file_position);
	if (inFile.is_open())
	{
		std::string line;
		std::vector<std::string> totalstring;

		while (getline(inFile, line))
		{
			totalstring.push_back(line);

		}
		int strveclen = totalstring.size();
		auto vit = totalstring.begin();
		trans_escape_first(vit, totalstring,escapeCharQueue);
		trans_code(vit, totalstring);
		trans_list(vit, totalstring);
		trans_parting_line(vit, totalstring);
		trans_title(vit, totalstring); // H1-H5
		trans_refer(vit, totalstring);
		trans_bold_and_italic(vit, totalstring);
		trans_image(vit, totalstring);
		trans_url(vit, totalstring);
		trans_linebreak(vit, totalstring);
		trans_escape_last(vit, totalstring, escapeCharQueue);
		trans_emptyline(vit, totalstring);

		for (auto ptr = totalstring.begin(); ptr != totalstring.end(); ptr++)
		{
			outFile += (*ptr) + '\n';
		}
		inFile.close();
	}
	return outFile;
}

void MarkdownTransform::trans_title(std::vector<std::string>::iterator & vecIt, std::vector<std::string> & str)
{
	std::string newStr;
	std::vector<std::string>::iterator initialVectorIt = vecIt;

	if (vecIt == str.end())
	{
		return;
	}

	int count = 0;
	for (; vecIt != str.end(); vecIt++)
	{
		std::string::iterator strIt = (*vecIt).begin();
		if (!(*vecIt).empty())
		{
			count = 0;
			if (*strIt == '#')
			{
				count++;
				strIt++;
				for (; strIt != (*vecIt).end(); strIt++)
				{
					if (*strIt == '#'&&*(strIt - 1) == '#')
					{
						if (count < 5)
						{
							count++;
						}
					}
					else break;
				}

				int sign = count;
				//std::cout << sign;
				//H1-H5
				if (sign >= 1 && sign <= 5)
				{
					std::string tempStr = (*vecIt);
					//get rid of #
					for (auto it = tempStr.end() - 1; it != tempStr.begin(); it--)
					{
						if (*it == '#')
						{
							tempStr.erase(it);
							it = tempStr.end();
						}
						else break;
					}
					for (auto it = tempStr.begin(); it != tempStr.end(); )
					{
						if (*it == '#')
						{
							tempStr.erase(it);
							it = tempStr.begin();
						}
						else break;
					}
					//Int to string
					std::stringstream ss;
					ss << sign;
					std::string hnumber = 'h' + ss.str();
					newStr = '<' + hnumber + '>' + tempStr + "</" + hnumber + '>';
					(*vecIt) = newStr;
					newStr = "";
				}
			}
			else if (*strIt == '>')
			{
				strIt++;
				while (*strIt == '>'&& strIt != (*vecIt).end())
				{
					strIt++;
				}
				if (*strIt == '#'&& strIt != (*vecIt).end())
				{
					count++;
					strIt++;
					for (; strIt != (*vecIt).end(); strIt++)
					{
						if (*strIt == '#'&&*(strIt - 1) == '#')
						{
							if (count < 5)
							{
								count++;
							}
						}
						else break;
					}
					int sign = count;
					if (sign >= 1 && sign <= 5)
					{
						std::string tempStr = (*vecIt);
						for (auto it = tempStr.end() - 1; it != tempStr.begin(); it--)
						{
							if (*it == '#')
							{
								tempStr.erase(it);
								it = tempStr.end();
							}
							else break;
						}
						int refer_counter = 0;
						auto it = tempStr.begin();

						while (*it == '>' && it != tempStr.end())
						{
							refer_counter++;
							tempStr.erase(it);
							it = tempStr.begin();
						}
						for (; it != tempStr.end();)
						{
							if (*it == '#')
							{
								tempStr.erase(it);
								it = tempStr.begin();
							}
							else break;
						}
						while (refer_counter--)
						{
							newStr += '>';
						}
						std::stringstream ss;
						ss << sign;
						std::string hnumber = 'h' + ss.str();
						newStr += '<' + hnumber + '>' + tempStr + "</" + hnumber + '>';
						(*vecIt) = newStr;
						newStr = "";
					}

				}
			}
		}

	}
	vecIt = initialVectorIt;
}

void MarkdownTransform::trans_code(std::vector<std::string>::iterator & vectorIt, std::vector<std::string> & str)
{
	std::string newStr;
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	if (vectorIt == str.end())
	{
		return;
	}
	//deal with code
	for (; vectorIt != str.end(); vectorIt++)
	{
		std::string::iterator striter = (*vectorIt).begin();
		//Found a line ```, the following line to determine whether there is a match, ```// modified newstr
		if ((*vectorIt).size() >= 3 && (*striter) == '`' && (*(striter + 1)) == '`' && (*(striter + 2)) == '`')
		{
			std::vector<std::string>::iterator tempvectorIt = vectorIt;
			std::string temp = *vectorIt;
			*vectorIt = "<pre><code>";
			//Traverse the document to find the second line ```
			for (; tempvectorIt != str.end(); tempvectorIt++)
			{
				std::string::iterator tempStriter = (*tempvectorIt).begin();
				//If the second line is found, and output, and change the value of vectorIt, end the loop
				if ((*tempvectorIt).size() >= 3 && (*tempStriter) == '`' && (*(tempStriter + 1)) == '`' && (*(tempStriter + 2)) == '`')
				{
					vectorIt = tempvectorIt;
					*tempvectorIt = "</code></pre>";
					break;
				}
				else continue;
			}
			if (tempvectorIt == str.end())//Forced adcountustment
			{
				*vectorIt = temp;
			}
		}
		//Looking for inline code blocks 
		//modified newstr
		else
		{
			std::stack<char> tempb, outc;
			std::string::iterator striter = (*vectorIt).begin();
			if (!(*vectorIt).empty())
			{
				tempb.push((*striter++));
				int counter = 0;
				for (; striter != (*vectorIt).end(); striter++)
				{
					if ((*striter) == '`'&&tempb.top() == '`')
					{
						continue;
					}
					else if ((*striter) == '`')
					{
						counter++;
					}
					tempb.push((*striter));
				}
				while (!tempb.empty())
				{
					outc.push(tempb.top());
					tempb.pop();
				}

				int totalcount = counter;
				while (!outc.empty())
				{
					if (outc.top() == '`')
					{
						if (totalcount > 1 && counter - 1 == 0 && totalcount % 2 == 1)
						{
						}
						else if (totalcount > 1 && (totalcount - counter) % 2 == 0)
						{
							newStr = newStr + "<code>";
						}
						else if (totalcount > 1 && (totalcount - counter) % 2 == 1)
						{
							newStr = newStr + "</code>";
						}
						outc.pop();
						counter--;
					}
					else
					{
						char ch = outc.top();
						newStr = newStr + ch;
						outc.pop();
					}
				}
				*vectorIt = newStr;
				newStr = "";
			}
		}
	}
	vectorIt = initialVectorIt;
}

void MarkdownTransform::trans_refer(std::vector<std::string>::iterator &vectorIt, std::vector<std::string> &str)
{
	std::string newStr;
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	int vectorItincreasement = 0;
	std::stack<int> refer_level;
	if (vectorIt == str.end())
	{
		return;
	}
	
	for (; vectorIt != str.end(); vectorIt++)
	{
		
		if (!(*vectorIt).empty())
		{
			std::string::iterator striter = (*vectorIt).begin();
			if (*striter == '>')
			{
				newStr = "";
				while ((*vectorIt) != "")
				{
					vectorIt++;
					vectorItincreasement++;
				}
				for (int i=0; i<vectorItincreasement; i++)
				{
					vectorIt--;
				}
				for (int i = 0; i < vectorItincreasement; i++,vectorIt++)
				{
					int temptotal = 0;
					
					std::string tempStr = (*vectorIt);
					std::string::iterator striter2 = tempStr.begin();
					if (*striter2 == '>'&&striter2!=tempStr.end()-1)
					{
						temptotal++;
						striter2++;
						for (; striter2 != tempStr.end(); striter2++)
						{
							if (*striter2 == '>'&&*(striter2 - 1) == '>')
							{
								temptotal++;
								
							}
							else break;
						}
						
						if (!refer_level.empty() && refer_level.top() < temptotal)
						{
							int x = temptotal;
							auto tempStriter = tempStr.begin();
							while(x--)
							{
								tempStr.erase(tempStriter);
								tempStriter = tempStr.begin();
							}
							for (int i = 0; i < (temptotal - refer_level.top());i++)
							{
								tempStr = "<blockquote>" + tempStr;
							}
							refer_level.push(temptotal);//add <>
						}
						else if (refer_level.empty())
						{
							int x = temptotal;
							auto tempStriter = tempStr.begin();
							while(x--)
							{
								tempStr.erase(tempStriter);
								tempStriter = tempStr.begin();
							}
							for (int i = 0; i < temptotal; i++)
							{
								tempStr = "<blockquote>" + tempStr;
							}
							refer_level.push(temptotal);//add <>
						}
						else if (!refer_level.empty() && refer_level.top() == temptotal)
						{
							continue;
						}
						else if (!refer_level.empty() && refer_level.top() > temptotal)
						{
							int n=refer_level.top()-temptotal;//add </>
							int x = temptotal;
							auto tempStriter = tempStr.begin();
							while (x--)
							{
								tempStr.erase(tempStriter);
								tempStriter = tempStr.begin();
							}
							for (int i = 0; i < n; i++)
							{
								tempStr =  "</blockquote>"+ tempStr ;
							}
							refer_level.pop();
							refer_level.push(temptotal);//add <>
						}
						*vectorIt = tempStr;
					}
				}
				int n = refer_level.top();
				for (int i = 0; i < n; i++)
				{
					newStr+= "</blockquote>";
				}
				while (!refer_level.empty())
				{
					refer_level.pop();
				}
				(*vectorIt) = newStr;
				newStr = "";
			}
		}
	}
	vectorIt = initialVectorIt;
}

//list
void MarkdownTransform::trans_list(std::vector<std::string>::iterator & vectorIt, std::vector<std::string> & str)
{
	std::string newStr;
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	std::stack<int> storageulol;
	int flag = 0, preflag = 0;
	int importantflag = 0;
	if (vectorIt == str.end())
	{
		return;
	}
	for (; vectorIt != str.end(); vectorIt++)
	{
		std::string::iterator striter = (*vectorIt).begin();
		if ((*vectorIt) == "")
		{
			while (!(storageulol.empty()))//Exclude special cases
			{
				int elem = storageulol.top();
				if (elem == 1)
				{
					(*vectorIt) = (*vectorIt) + "</ol>";//Add </ol> at the end of the paragraph 
				}
				else
				{
					(*vectorIt) = (*vectorIt) + "</ul>";//Add </ul> at the end of the paragraph 
				}
				storageulol.pop();
			}
			importantflag = 0;
			preflag = flag;
			flag = 0;
		}
		else
		{
			while (*striter == '>') { striter++; }
			while (*striter == ' ')
			{
				if (*striter == ' ') { flag++; }
				striter++;
			}
			if (flag == 0  && importantflag == 0)
			{
				int position = striter - (*vectorIt).begin();
				int e = list_helper(position, *vectorIt);
				if (e != 0)
				{
					storageulol.push(e);
					change_list(position, vectorIt, e);
					importantflag = 1;
				}
				
			}
			else if (((flag + 3) / 4) > ((preflag + 3) / 4))
			{
				int n = ((flag + 3) / 4) - ((preflag + 3) / 4);
				if (n >= 2) { continue; }
				else
				{
					int position = striter - (*vectorIt).begin();
					int e = list_helper(position, *vectorIt);
					if (e != 0)
					{
						storageulol.push(e);
						change_list(position, vectorIt, e);
					}
				}
			}
			else if (((flag + 3) / 4) == ((preflag + 3) / 4))
			{
				int position = striter - (*vectorIt).begin();
				int e = list_helper(position, *vectorIt);
				if (e != 0)
				{
					change_list(position, vectorIt, 3);
				}
			}
			else //(((flag + 3) / 4) < ((preflag + 3) / 4))
			{
				int n = ((preflag + 3) / 4) - ((flag + 3) / 4);
				int position = striter - (*vectorIt).begin();
				int e = list_helper(position, *vectorIt);
				if (e != 0)
				{
					change_list(position, vectorIt, 3);
					while (n--)
					{
						if (storageulol.top())
						{
							int elem = storageulol.top();
							if (elem == 1)
							{
								*vectorIt = "</ol>"+(*vectorIt);
							}
							else
							{
								*vectorIt = "</ul>" + (*vectorIt);
							}
							storageulol.pop();
						}
					}
				}
			}
			preflag = flag;
			flag = 0;
		
		}
	}
	
	vectorIt = initialVectorIt;
}
// Note that the use of string[i] in the ascii code comparison, may be wrong
// Returns 1 for ol 2 for ul, you must reset the striker
int MarkdownTransform::list_helper(int position, std::string str)
{
	std::string::iterator tempStriter = str.begin() + position;
	if ((str.end() - tempStriter) > 2 && ((*tempStriter) == '*' || (*tempStriter) == '+' || (*tempStriter) == '-') && *(tempStriter + 1) == ' ')
	{
		return 2;
	}
	else if ((int)(*tempStriter)>=48&& (int)(*tempStriter)<=57)//Determine if it is number
	{ 
		tempStriter++;
		while ((int)(*tempStriter) >= 48 && (int)(*tempStriter) <= 57)
		{
			tempStriter++;
		}
		if ((str.end() - tempStriter) > 2 && *tempStriter == '.'&& *(tempStriter + 1) == ' ')
		{
			return 1;
		}
		else { return 0; }
	}
	else { return 0; }//none of the cases
}
//Delete the identifier and insert ul or ol and <li> and </li>
void MarkdownTransform::change_list(int position, std::vector<std::string>::iterator &vectorIt,int sign)
{
	std::string tempStr = (*vectorIt);
	std::string::iterator tempStriter = tempStr.begin() + position;

	while (((*tempStriter) == '*' || (*tempStriter) == '+' || (*tempStriter) == '-' || ((int)(*tempStriter) >= 48 && (int)(*tempStriter) <= 57)) && tempStriter != tempStr.end())
	{
		tempStr.erase(tempStriter);
		tempStriter = tempStr.begin() + position;
	}

	if(tempStriter != tempStr.end())//delete spaces
	{
		tempStr.erase(tempStriter);
		tempStriter = tempStr.begin() + position;
	}
	if (sign == 3)
	{
		tempStr.insert(position, "<li>");
		tempStr = tempStr + "</li>";
		(*vectorIt) = tempStr;
	}
	else if (sign == 1)
	{
		tempStr.erase(tempStriter);
		tempStriter = tempStr.begin() + position;
		tempStr.insert(position, "<ol><li>");
		tempStr = tempStr + "</li>";
		(*vectorIt) = tempStr;
	}
	else if (sign == 2)
	{
		tempStr.insert(position, "<ul><li>");
		tempStr = tempStr + "</li>";
		(*vectorIt) = tempStr;
	}
}

//escape
void MarkdownTransform::trans_escape_first(std::vector<std::string>::iterator & vectorIt, 
	std::vector<std::string> & str, std::queue<char>& charqueue)
{
	std::string newStr;
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	if (vectorIt == str.end())
	{
		return;
	}
	for (; vectorIt != str.end(); vectorIt++)
	{
		std::string::iterator striter = (*vectorIt).begin();
		if (!(*vectorIt).empty())
		{
			for (; striter != (*vectorIt).end(); )
			{
				if ((*striter) == '\\' && striter != ((*vectorIt).end()-1))
				{
					char ch = *(striter + 1);
					charqueue.push(ch);
					striter++;
					int position = striter - (*vectorIt).begin();
					(*vectorIt).erase(striter);
					striter = (*vectorIt).begin() + position ;
				}
				else striter++;
			}
		}
	}
	vectorIt = initialVectorIt;
	
}

void MarkdownTransform::trans_escape_last(std::vector<std::string>::iterator & vectorIt,
	std::vector<std::string> & str, std::queue<char>& charqueue)
{
	std::string newStr;
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	if (vectorIt == str.end())
	{
		return;
	}
	for (; vectorIt != str.end(); vectorIt++)
	{
		std::string::iterator striter = (*vectorIt).begin();
		if (!(*vectorIt).empty())
		{
			for (; striter != (*vectorIt).end(); striter++)
			{
				if (*striter == '\\'&& !charqueue.empty())
				{
					char ch=charqueue.front();
					*striter = ch;
					charqueue.pop();
				}
			}
		}
	}
	vectorIt = initialVectorIt;
}

//bold and itatlics
void MarkdownTransform::trans_bold_and_italic(std::vector<std::string>::iterator & vectorIt, std::vector<std::string> & str)
{
	std::string tempStr;
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	int itaCounter = 0, boldCounter=0;

	if (vectorIt == str.end())
	{
		return;
	}

	for (; vectorIt != str.end(); vectorIt++)
	{
		tempStr = (*vectorIt);
		std::string::iterator striter = tempStr.begin();
		if (!tempStr.empty())
		{
			for (; striter != tempStr.end(); striter++)
			{
				int position = striter - tempStr.begin();
				if ((tempStr.end()-striter)>2&&(*striter) == '*'&&(*(striter + 1)) == '*'&& boldCounter==0)
				{
					tempStr.erase(striter);
					striter = tempStr.begin() + position;
					tempStr.erase(striter);
					striter = tempStr.begin() + position;
					tempStr.insert(position, "<strong>");
					(*vectorIt) = tempStr;
					striter = tempStr.begin() + position;
					boldCounter = 1;
				}
				else if ((tempStr.end() - striter)>2 && (*striter) == '*' && (*(striter + 1)) == '*'&& boldCounter == 1)
				{
					tempStr.erase(striter);
					striter = tempStr.begin() + position;
					tempStr.erase(striter);
					striter = tempStr.begin() + position;
					tempStr.insert(position, "</strong>");
					(*vectorIt) = tempStr;
					striter = tempStr.begin() + position;
					boldCounter = 0;
				}
				else if ((*striter) == '*'&& itaCounter == 0)
				{
					tempStr.erase(striter);
					striter = tempStr.begin() + position;
					tempStr.insert(position, "<em>");
					(*vectorIt) = tempStr;
					striter = tempStr.begin() + position;
					itaCounter = 1;
				}
				else if ((*striter) == '*'&& itaCounter == 1)
				{
					tempStr.erase(striter);
					striter = tempStr.begin() + position;
					tempStr.insert(position, "</em>");
					(*vectorIt) = tempStr;
					striter = tempStr.begin() + position;
					itaCounter = 0;
				}
			}
			if (itaCounter == 1) { (*vectorIt) = (*vectorIt) + "</em>"; itaCounter = 0; }
			if (boldCounter == 1) { (*vectorIt) = (*vectorIt) + "</strong>"; boldCounter = 0; }
		}
	}
	vectorIt = initialVectorIt;
}

void MarkdownTransform::trans_parting_line(std::vector<std::string>::iterator & vectorIt, std::vector<std::string> & str)
{
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	if (vectorIt == str.end())
	{
		return;
	}
	int count = 0;
	for (; vectorIt != str.end(); vectorIt++)
	{
		std::string::iterator striter = (*vectorIt).begin();
		if (!(*vectorIt).empty())
		{
			count = 0;
			if (*striter == '*')
			{
				count++;
				striter++;
				for (; striter != (*vectorIt).end(); striter++)
				{
					if (*striter == '*'&&*(striter - 1) == '*')
					{
						if (count < 3)
						{
							count++;
							
						}
					}
					else break;
				}
				if (striter == (*vectorIt).end()&&count==3) { (*vectorIt) = "<hr>";  }
			}
		}
	}
	vectorIt = initialVectorIt;
}

void MarkdownTransform::trans_linebreak(std::vector<std::string>::iterator&vectorIt, std::vector<std::string>&str)
{
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	if (vectorIt == str.end())
	{
		return;
	}
	for (; vectorIt != str.end(); vectorIt++)
	{
		if (!(*vectorIt).empty())
		{
			if (*vectorIt == "<pre><code>")
			{
				while (vectorIt != str.end() && *vectorIt != "</code></pre>")
				{
					vectorIt++;
				}
			}
			else 
			{
				std::string::iterator striter = (*vectorIt).end() - 1;
				if ((*vectorIt).length() > 1 && striter != (*vectorIt).begin() && *striter == ' '&&*(striter - 1) == ' ')
				{
					*vectorIt = (*vectorIt) + "<br>";
				}
			}
		}
	}
	vectorIt = initialVectorIt;
}

// add p tags
void MarkdownTransform::trans_emptyline(std::vector<std::string>::iterator&vectorIt, std::vector<std::string>&str)
{
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	if (vectorIt == str.end())
	{
		return;
	}
	for (; vectorIt != str.end(); vectorIt++)
	{
		if (!(*vectorIt).empty())
		{
			continue;
		}
		else
		{
			*vectorIt = "<p></p>";
		}
	}
	vectorIt= initialVectorIt;
}


// urls
void MarkdownTransform::trans_url(std::vector<std::string>::iterator&vectorIt, std::vector<std::string>&str)
{
	std::string tempStr;
	std::string newStr = "";
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	if (vectorIt == str.end())
	{
		return;
	}
	//Regex
	std::string totalpattern;
	totalpattern = "\\[.*\\]\\((https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|](((\\s+)\".*\")?)\\)";
	std::regex totalr(totalpattern);
	std::string urlpattern = "(https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]";
	std::string contentpattern = "\\[.*\\]\\(";
	std::string titlepattern = "\".*\"";
	std::regex urlr(urlpattern);
	std::regex titler(titlepattern);
	std::regex contentr(contentpattern);

	for (; vectorIt != str.end(); vectorIt++)
	{
		tempStr = (*vectorIt);
		std::string::iterator striter = tempStr.begin();
		if (!tempStr.empty())
		{
			//Stored in a vector container, fragmented by a regular expression
			std::vector<std::string> storage_tempStr;
			storage_tempStr.push_back(tempStr);
			if (!storage_tempStr.empty())
			{
				for (std::sregex_iterator riter(tempStr.begin(), tempStr.end(), totalr), end_it; riter != end_it; riter++)
				{
					std::string limit = riter->str();//current regex
					if (!storage_tempStr.empty())
					{
						std::string str = storage_tempStr.back();//last element in vector
						storage_tempStr.pop_back();//delete the last vector
						split(str, limit,storage_tempStr);
					}
				}
				std::string after_limit;
				int veci = 0;
				for (std::sregex_iterator riter(tempStr.begin(), tempStr.end(), totalr), end_it; riter != end_it; riter++,veci++)
				{
					std::string limit = riter->str();
					std::string after_limit = url_helper(limit, urlr,titler,contentr);
					newStr = newStr + storage_tempStr[veci] + after_limit;
				}
				if (storage_tempStr[veci] != "")
				{
					newStr += storage_tempStr[veci];
				}
			}
			*vectorIt = newStr;
			newStr = "";
		}
	}
	vectorIt = initialVectorIt;
}

//The helper function splits the string into two segments according to the passed limit.
void MarkdownTransform::split(std::string str, std::string limit, std::vector<std::string> & result)
{
	std::vector<std::string>::size_type ptr;
	int len = str.length();
	ptr = str.find(limit, 0);
	if ((int)ptr < len)
	{
		std::string substr = str.substr(0, ptr);
		result.push_back(substr);
		std::string substr2 = str.substr(ptr+ limit.length(),len);
		result.push_back(substr2);
	}
}
//Process strings into <a href=""></a>
std::string MarkdownTransform::url_helper(std::string &str,std::regex &urlr,std::regex &titler,std::regex & contentr)
{
	std::string newStr;
	std::smatch urlresult,tlresult,ctresult;
	regex_search(str, urlresult, urlr);
	regex_search(str, ctresult, contentr);
	std::string ctresult_change = ctresult.str();
	std::string::iterator siter = ctresult_change.begin();
	ctresult_change.erase(siter);
	siter= ctresult_change.begin();
	siter = ctresult_change.end() - 1;
	for (int i = 0; i < 2; i++,siter--)
	{
		ctresult_change.erase(siter);
		siter = ctresult_change.end();
	}
	if (!regex_search(str, tlresult, titler))
	{
		newStr = "<a href=\"" + urlresult.str() + "\">" + ctresult_change + "</a>";
	}
	else
	{
		regex_search(str, tlresult, titler);
		newStr = "<a href=\"" + urlresult.str() + "\" title=" +tlresult.str()+">"+ ctresult_change + "</a>";
	}
	return newStr;
}

//Handling hyperlinks
void MarkdownTransform::trans_image(std::vector<std::string>::iterator&vectorIt, std::vector<std::string>&str)
{
	std::string tempStr;
	std::string newStr = "";
	std::vector<std::string>::iterator initialVectorIt = vectorIt;
	if (vectorIt == str.end())
	{
		return;
	}
	//Creating Regex
	std::string totalpattern;
	totalpattern = "!\\[.*\\]\\((https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|](((\\s+)\".*\")?)\\)";
	std::regex totalr(totalpattern);
	std::string urlpattern = "(https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]";
	std::string contentpattern = "!\\[.*\\]\\(";
	std::string titlepattern = "\".*\"";
	std::regex urlr(urlpattern);
	std::regex titler(titlepattern);
	std::regex contentr(contentpattern);

	for (; vectorIt != str.end(); vectorIt++)
	{
		tempStr = (*vectorIt);
		std::string::iterator striter = tempStr.begin();
		if (!tempStr.empty())
		{
			std::vector<std::string> storage_tempStr;
			storage_tempStr.push_back(tempStr);
			if (!storage_tempStr.empty())
			{
				for (std::sregex_iterator riter(tempStr.begin(), tempStr.end(), totalr), end_it; riter != end_it; riter++)
				{
					std::string limit = riter->str();
					if (!storage_tempStr.empty())
					{
						std::string str = storage_tempStr.back();
						storage_tempStr.pop_back();
						split(str, limit, storage_tempStr);
					}
				}
				std::string after_limit;
				int veci = 0;
				for (std::sregex_iterator riter(tempStr.begin(), tempStr.end(), totalr), end_it; riter != end_it; riter++, veci++)
				{
					std::string limit = riter->str();
					std::string after_limit = image_helper(limit, urlr, titler, contentr);
					newStr = newStr + storage_tempStr[veci] + after_limit;
				}
				if (storage_tempStr[veci] != "")
				{
					newStr += storage_tempStr[veci];
				}
			}
			*vectorIt = newStr;
			newStr = "";
		}
	}
	vectorIt = initialVectorIt;
}
// <img>
std::string MarkdownTransform::image_helper(std::string &str, std::regex &urlr, std::regex &titler, std::regex & contentr)
{
	std::string newStr;
	std::smatch urlresult, tlresult, ctresult;
	regex_search(str, urlresult, urlr);
	regex_search(str, ctresult, contentr);
	std::string ctresult_change = ctresult.str();
	std::string::iterator siter = ctresult_change.begin();
	for (int i = 0; i < 2; i++)
	{
		ctresult_change.erase(siter);
		siter = ctresult_change.begin();
	}
	siter = ctresult_change.end() - 1;
	for (int i = 0; i < 2; i++, siter--)
	{
		ctresult_change.erase(siter);
		siter = ctresult_change.end();
	}
	if (!regex_search(str, tlresult, titler))
	{
		newStr = "<img src=\"" + urlresult.str() + "\" alt=\"" + ctresult_change + "\" />";
	}
	else
	{
		regex_search(str, tlresult, titler);
		newStr = "<img src=\"" + urlresult.str() + "\" title=" + tlresult.str() + "\" alt=\"" + ctresult_change + "\" />";
	}
	return newStr;
}