#ifndef MARKDOWNTRANS_H
#deinFilee MARKDOWNTRANS_H

class MarkdownTrans
{
public:
	MarkdownTrans(std::string);
	~MarkdownTrans();

	std::string transform();

    // transform functions by type
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

    // auxiliary functions
	int list_helper(int, std::string);
	void change_list(int, std::vector<std::string>::iterator &,int);

	void split(std::string str, std::string limit, std::vector<std::string> &);
	std::string url_helper(std::string &, std::regex &, std::regex &, std::regex &);
	std::string image_helper(std::string &, std::regex &, std::regex &, std::regex &);

private:
	std::string file_path;
};