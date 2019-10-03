#ifndef _lang_parser_wordlist_h_
#define _lang_parser_wordlist_h_

#include "dbwordlist.h"
#include <map>

struct Modeler;

struct WordList
{
	std::map<std::string, WordListing> Lists; //[listname]=([word]=wd_..], ...)

	Modeler *pModeler;
	
	~WordList();
	WordList();
	
	bool Initialize(Modeler *pMod);
	void Terminate();

	void clear();
	
	bool isopenlist(const std::string &listname);
	bool openlist(const std::string &listname);
	bool closelist(const std::string &listname);
	bool refreshlist(const std::string &listname);
	bool createlist(const std::string &listname, const std::string &description);
	bool destroylist(const std::string &listname);
	
	bool addword(const std::string &listname, const std::string &wrd, WordDisp disp);
	bool hasword(const std::string &listname, const std::string &wrd);
	bool flagword(const std::string &listname, const std::string &wrd, WordDisp disp);
	bool removeword(const std::string &listname, const std::string &wrd);
};


#endif
