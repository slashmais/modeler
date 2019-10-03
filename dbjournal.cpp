
#include "dbjournal.h"
#include <utilfuncs/utilfuncs.h>


DBJournal dbjournal;

bool DBJournal::ImplementSchema()
{
	bool b=bDBOK;
	//DBResult RS;
	//std::string SQL;

//	if (b)
//	{
//		sSQL = "CREATE TABLE IF NOT EXISTS IDS (name, id, del)";
//		ExecSQL(&RS, SQL); //create/reuse ids: init_ids(tabelname) once after CREATE TABLE.. and new_id() / del_id() after
//		b=NoError(this);
//	}
	
	if (b) b = (MakeTable("item", "hdts, flag, name, note")); //&&init_ids("item"));


	return b;

}

bool DBJournal::Save(JItem &I)
{
	DBResult RS;
	std::string SQL;

	Delete(I);
	SQL=spf("INSERT INTO item(hdts, flag, name, note) VALUES(", SQLSafeQ(I.hdts), ", ", I.flag, ", ", SQLSafeQ(I.name), ", ", SQLSafeQ(I.note), ")");
	ExecSQL(&RS, SQL);
	return NoError(this);
}

bool DBJournal::Load(ItemList &L)
{
	DBResult RS;
	size_t i=0,n;
	L.clear();
	n=ExecSQL(&RS, "SELECT hdts, flag, name, note FROM item ORDER BY hdts ASC");
	if (NoError(this))
	{
		while (i<n)
		{
			JItem I;
			I.hdts=SQLRestore(RS.GetVal("hdts", i));
			I.flag=stot<int>(RS.GetVal("flag", i));
			I.name=SQLRestore(RS.GetVal("name", i));
			I.note=SQLRestore(RS.GetVal("note", i));
			L.push_back(I);
			i++;
		}
		return true; //empty ok
	}
	return false;
}

bool DBJournal::Delete(JItem &I)
{
	DBResult RS;
	std::string SQL;
	SQL=spf("DELETE FROM item WHERE hdts=", SQLSafeQ(I.hdts));
	ExecSQL(&RS, SQL);
	return NoError(this);
}





