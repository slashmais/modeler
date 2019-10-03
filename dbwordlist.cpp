
#include "dbwordlist.h"
#include <utilfuncs/utilfuncs.h>

DBWordlist dbwordlist;

bool DBWordlist::ImplementSchema()
{
	bool b=bDBOK;
	DBResult RS;
	std::string SQL;
	//if (b)
	//{
	//	SQL = "CREATE TABLE IF NOT EXISTS IDS (name, id, del)";
	//	ExecSQL(&RS, SQL); //create/reuse ids: init_ids(tabelname) once after CREATE TABLE.. and new_id() / del_id() after
	//	b=NoError(this);
	//}
	
	if (b)
	{
		b = (MakeTable("lists", "name TEXT PRIMARY KEY, description", "WITHOUT ROWID"));
		if (b) { b=AddList("fillers", "ignored words"); }
		if (b) { b=AddList("actions", "action: act,change,relation,conditional-case"); }
		if (b) { b=AddList("things", "thing: object,situation,process,event"); }
	}

	if (b) b = (MakeTable("fillers", "word TEXT PRIMARY KEY, disp", "WITHOUT ROWID"));
	if (b) b = (MakeTable("actions", "word TEXT PRIMARY KEY, disp", "WITHOUT ROWID"));
	if (b) b = (MakeTable("things",  "word TEXT PRIMARY KEY, disp", "WITHOUT ROWID"));
	//..

	return b;
}

bool DBWordlist::AddList(const std::string &listname, const std::string &description)
{
	DBResult RS;
	std::string ln=lcase(listname);
	if (ExecSQL(&RS, spf("SELECT * FROM lists WHERE name=", SQLSafeQ(ln)))==0)
	{
		ExecSQL(&RS, spf("INSERT INTO lists(name, description) ", SQLValues(ln, description)));
		return NoError(this);
	}
	return false;
}

bool DBWordlist::SaveList(const std::string &listname, const std::string &description) //updates description only
{
	DBResult RS;
	std::string ln=lcase(listname);
	ExecSQL(&RS, spf("UPDATE lists SET description=", SQLSafeQ(description), " WHERE name=", SQLSafeQ(ln)));
	return NoError(this);
}

bool DBWordlist::LoadLists(ListListing &L)
{
	DBResult RS;
	L.clear();
	size_t i=0,n;
	n=ExecSQL(&RS, spf("SELECT * FROM lists"));
	if (NoError(this))
	{
		while (i<n) { L[SQLRestore(RS.GetVal("name",i))]=SQLRestore(RS.GetVal("description",i)); i++; }
		return true;
	}
	return false;
}

bool DBWordlist::DropList(const std::string &listname)
{
	DBResult RS;
	std::string ln=lcase(listname);
	ExecSQL(&RS, spf("DROP TABLE ", ln));
	ExecSQL(&RS, spf("DELETE FROM lists WHERE name=", SQLSafeQ(ln)));
	return NoError(this);
}

bool DBWordlist::Save(const std::string &listname, const std::string &wrd, WordDisp disp)
{
	DBResult RS;
	std::string ln=lcase(listname);
	std::string w=lcase(wrd);
	std::string SQL=spf("INSERT INTO ", ln, "(word, disp) ", SQLValues(w, disp));
	ExecSQL(&RS, SQL);
	if (NoError(this)) return true;
	else
	{
		int rc=GetLastErrorRC();
		if (rc==SQLITE_CONSTRAINT_PRIMARYKEY)
		{
			SQL=spf("UPDATE ", ln, " SET disp=", (int)disp, " WHERE word=", SQLSafeQ(w));
			ExecSQL(&RS, SQL);
			return NoError(this);
		}
		return false;
	}
}

bool DBWordlist::Flag(const std::string &listname, const std::string &wrd, WordDisp disp)
{
	DBResult RS;
	std::string ln=lcase(listname);
	std::string w=lcase(wrd);
	std::string SQL{};
	SQL=spf("UPDATE ", ln, " set disp=", (int)disp, " WHERE word=", SQLSafeQ(w));
	ExecSQL(&RS, SQL);
	return NoError(this);
}

bool DBWordlist::Load(const std::string &listname, WordListing &wl, WordDisp disp)
{
	wl.clear();
	DBResult RS;
	std::string ln=lcase(listname);
	size_t i=0, n;
	n=ExecSQL(&RS, spf("SELECT * FROM ", ln, " WHERE disp=", (int)disp));
	if (NoError(this))
	{
		while (i<n) { wl[SQLRestore(RS.GetVal("word", i))]=(WordDisp)stot<int>(RS.GetVal("disp", i)); i++; }
		return true; //empty ok
	}
	return false;
}

bool DBWordlist::Load(const std::string &listname, WordListing &wl)
{
	wl.clear();
	DBResult RS;
	std::string ln=lcase(listname);
	size_t i=0, n;
	n=ExecSQL(&RS, spf("SELECT * FROM ", ln));
	if (NoError(this))
	{
		while (i<n) { wl[SQLRestore(RS.GetVal("word", i))]=(WordDisp)stot<int>(RS.GetVal("disp", i)); i++; }
		return true; //empty ok
	}
	return false;
}

bool DBWordlist::Drop(const std::string &listname, const std::string &wrd)
{
	DBResult RS;
	std::string ln=lcase(listname);
	std::string w=lcase(wrd);
	std::string SQL=spf("DELETE FROM ", ln, " WHERE word=", SQLSafeQ(w));
	ExecSQL(&RS, SQL);
	return NoError(this);
}

//===============================================================================
