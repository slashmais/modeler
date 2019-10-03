#ifndef _dbwordlist_h_
#define _dbwordlist_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>
#include <tuple>

typedef DB_ID_TYPE WordType;

enum WordDisp//osition
{
	WD_USE=0,
	WD_IGNORE=1, //to en/disable without deleting(=losing) entry
};

typedef std::map<std::string, WordDisp> WordListing; //[word]=wd_..
typedef std::map<std::string, std::string> ListListing; //[listname]=description

struct DBWordlist : public DBsqlite3
{
	virtual ~DBWordlist() {}
	
	bool ImplementSchema();
	
	bool AddList(const std::string &listname, const std::string &description);
	bool SaveList(const std::string &listname, const std::string &description); //updates description only
	bool LoadLists(ListListing &L);
	bool DropList(const std::string &listname);
	
	bool Save(const std::string &listname, const std::string &wrd, WordDisp disp=WD_USE);
	bool Flag(const std::string &listname, const std::string &wrd, WordDisp disp);
	bool Load(const std::string &listname, WordListing &wl, WordDisp disp);
	bool Load(const std::string &listname, WordListing &wl);
	bool Drop(const std::string &listname, const std::string &wrd); //deletes from table; don't - rather just flag to ignore
};

extern DBWordlist dbwordlist;



#endif
