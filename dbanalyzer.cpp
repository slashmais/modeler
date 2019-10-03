
#include "dbanalyzer.h"

DBAnalyzer dbanalyzer;


bool DBAnalyzer::ImplementSchema()
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
	
	if (b) b = (MakeTable("DUMMYTABLE", "DUMMY1, DUMMY2")); //&&init_ids("item"));


	return b;


}



