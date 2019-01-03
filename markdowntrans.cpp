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
					std::string tempstr = (*vecIt);
					//get rid of #
					for (auto it = tempstr.end() - 1; it != tempstr.begin(); it--)
					{
						if (*it == '#')
						{
							tempstr.erase(it);
							it = tempstr.end();
						}
						else break;
					}
					for (auto it = tempstr.begin(); it != tempstr.end(); )
					{
						if (*it == '#')
						{
							tempstr.erase(it);
							it = tempstr.begin();
						}
						else break;
					}
					//Int to string
					std::stringstream ss;
					ss << sign;
					std::string hnumber = 'h' + ss.str();
					newStr = '<' + hnumber + '>' + tempstr + "</" + hnumber + '>';
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
						std::string tempstr = (*vecIt);
						for (auto it = tempstr.end() - 1; it != tempstr.begin(); it--)
						{
							if (*it == '#')
							{
								tempstr.erase(it);
								it = tempstr.end();
							}
							else break;
						}
						int refer_counter = 0;
						auto it = tempstr.begin();

						while (*it == '>' && it != tempstr.end())
						{
							refer_counter++;
							tempstr.erase(it);
							it = tempstr.begin();
						}
						for (; it != tempstr.end();)
						{
							if (*it == '#')
							{
								tempstr.erase(it);
								it = tempstr.begin();
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
						newStr += '<' + hnumber + '>' + tempstr + "</" + hnumber + '>';
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
				std::string::iterator tempstriter = (*tempvectorIt).begin();
				//If the second line is found, and output, and change the value of vectorIt, end the loop
				if ((*tempvectorIt).size() >= 3 && (*tempstriter) == '`' && (*(tempstriter + 1)) == '`' && (*(tempstriter + 2)) == '`')
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
					
					std::string tempstr = (*vectorIt);
					std::string::iterator striter2 = tempstr.begin();
					if (*striter2 == '>'&&striter2!=tempstr.end()-1)
					{
						temptotal++;
						striter2++;
						for (; striter2 != tempstr.end(); striter2++)
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
							auto tempstriter = tempstr.begin();
							while(x--)
							{
								tempstr.erase(tempstriter);
								tempstriter = tempstr.begin();
							}
							for (int i = 0; i < (temptotal - refer_level.top());i++)
							{
								tempstr = "<blockquote>" + tempstr;
							}
							refer_level.push(temptotal);//add <>
						}
						else if (refer_level.empty())
						{
							int x = temptotal;
							auto tempstriter = tempstr.begin();
							while(x--)
							{
								tempstr.erase(tempstriter);
								tempstriter = tempstr.begin();
							}
							for (int i = 0; i < temptotal; i++)
							{
								tempstr = "<blockquote>" + tempstr;
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
							auto tempstriter = tempstr.begin();
							while (x--)
							{
								tempstr.erase(tempstriter);
								tempstriter = tempstr.begin();
							}
							for (int i = 0; i < n; i++)
							{
								tempstr =  "</blockquote>"+ tempstr ;
							}
							refer_level.pop();
							refer_level.push(temptotal);//add <>
						}
						*vectorIt = tempstr;
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
	std::string::iterator tempstriter = str.begin() + position;
	if ((str.end() - tempstriter) > 2 && ((*tempstriter) == '*' || (*tempstriter) == '+' || (*tempstriter) == '-') && *(tempstriter + 1) == ' ')
	{
		return 2;
	}
	else if ((int)(*tempstriter)>=48&& (int)(*tempstriter)<=57)//Determine if it is number
	{ 
		tempstriter++;
		while ((int)(*tempstriter) >= 48 && (int)(*tempstriter) <= 57)
		{
			tempstriter++;
		}
		if ((str.end() - tempstriter) > 2 && *tempstriter == '.'&& *(tempstriter + 1) == ' ')
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
	std::string tempstr = (*vectorIt);
	std::string::iterator tempstriter = tempstr.begin() + position;

	while (((*tempstriter) == '*' || (*tempstriter) == '+' || (*tempstriter) == '-' || ((int)(*tempstriter) >= 48 && (int)(*tempstriter) <= 57)) && tempstriter != tempstr.end())
	{
		tempstr.erase(tempstriter);
		tempstriter = tempstr.begin() + position;
	}

	if(tempstriter != tempstr.end())//delete spaces
	{
		tempstr.erase(tempstriter);
		tempstriter = tempstr.begin() + position;
	}
	if (sign == 3)
	{
		tempstr.insert(position, "<li>");
		tempstr = tempstr + "</li>";
		(*vectorIt) = tempstr;
	}
	else if (sign == 1)
	{
		tempstr.erase(tempstriter);
		tempstriter = tempstr.begin() + position;
		tempstr.insert(position, "<ol><li>");
		tempstr = tempstr + "</li>";
		(*vectorIt) = tempstr;
	}
	else if (sign == 2)
	{
		tempstr.insert(position, "<ul><li>");
		tempstr = tempstr + "</li>";
		(*vectorIt) = tempstr;
	}
}
