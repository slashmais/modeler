
#include "wordlist.h"
#include <utilfuncs/utilfuncs.h>
#include "modeler.h"


WordList::~WordList() { clear(); }
WordList::WordList() { clear(); }

bool WordList::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	
	//maybe load/prep some lists?
	
	return true;
}

void WordList::Terminate()
{
	//anything to save?
}

void WordList::clear() { Lists.clear(); }

bool WordList::isopenlist(const std::string &listname)
{
	std::string ln=lcase(listname);
	return (Lists.find(ln)!=Lists.end());
}

bool WordList::openlist(const std::string &listname)
{
	if (isopenlist(listname)) return true;
	std::string ln=lcase(listname);
	if (!dbwordlist.Load(ln, Lists[ln], WD_USE)) return tellerror("List '", listname, "': ", dbwordlist.GetLastError());
	return true;
}

bool WordList::closelist(const std::string &listname)
{
	std::string ln=lcase(listname);
	if (isopenlist(ln)) { Lists[ln].clear(); Lists.erase(ln); }
	return true; //just for consistency
}

bool WordList::refreshlist(const std::string &listname)
{
	std::string ln=lcase(listname);
	closelist(ln);
	return openlist(ln);
}

bool WordList::createlist(const std::string &listname, const std::string &description)
{
	return dbwordlist.AddList(listname, description);
}

bool WordList::destroylist(const std::string &listname)
{
	return dbwordlist.DropList(listname);
}

bool WordList::addword(const std::string &listname, const std::string &wrd, WordDisp disp)
{
	if (!openlist(listname)) return false;
	return dbwordlist.Save(listname, wrd, disp);
}

bool WordList::hasword(const std::string &listname, const std::string &wrd)
{
	if (!openlist(listname)) return false;
	std::string ln=lcase(listname);
	std::string w=lcase(wrd);
	return (Lists[ln].find(w)!=Lists[ln].end());
}

bool WordList::flagword(const std::string &listname, const std::string &wrd, WordDisp disp)
{
	if (!openlist(listname)) return false;
	return dbwordlist.Flag(listname, wrd, disp);
}

bool WordList::removeword(const std::string &listname, const std::string &wrd)
{
	closelist(listname);
	return dbwordlist.Drop(listname, wrd);
}
