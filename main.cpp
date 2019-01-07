#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<stack>
#include<queue>
#include<regex>
#include "mdtransform.h"// 

int main() {
	MarkdownTrans mdtrans("test.md");

	std::string contents = mdtrans.transform();

	// Prepare end part of the HTML file to be written
	std::string head = "<!DOCTYPE html><html><head>\
        <meta charset=\"utf-8\">\
        <title>Markdown</title>\
        <link rel=\"stylesheet\" href=\"my-markdown.css\">\
        </head><body><article class=\"markdown-body\">";
	std::string end = "</article></body></html>";

	// Output
	std::ofstream out;
	out.open("output\\index.html");		   
	out << head + contents + end;
	out.close();

	return 0;
}
