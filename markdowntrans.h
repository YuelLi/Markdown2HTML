#ifndef MARKDOWNTRANS_H
#define MARKDOWNTRANS_H

class MarkdownTrans
{
public:
	MarkdownTrans(std::string);
	~MarkdownTrans();
	std::string getContents();
	void trans_title(std::vector<std::string>::iterator &, std::vector<std::string> & );
	void trans_code(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void trans_refer(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void trans_list(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void trans_escape_first(std::vector<std::string>::iterator &, std::vector<std::string> & , std::queue<char>&);
	void trans_escape_last(std::vector<std::string>::iterator &, std::vector<std::string> &, std::queue<char>&);
	void trans_bold_and_italic(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void trans_parting_line(std::vector<std::string>::iterator &, std::vector<std::string> &);
	void trans_linebreak(std::vector<std::string>::iterator&, std::vector<std::string>&);
	void trans_emptyline(std::vector<std::string>::iterator&, std::vector<std::string>&);
	void trans_url(std::vector<std::string>::iterator&, std::vector<std::string>&);
	void trans_image(std::vector<std::string>::iterator&, std::vector<std::string>&);

private:
	std::string file_path;
};