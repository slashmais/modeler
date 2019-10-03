#ifndef _modeler_dbanalyzer_h_
#define _modeler_dbanalyzer_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

#include "analyzer.h"


struct DBAnalyzer : public DBsqlite3
{
	virtual ~DBAnalyzer() {}
	
	bool ImplementSchema();

	//bool Save(JItem &I);
	//bool Load(ItemList &L);
	//bool Delete(JItem &I);

};

extern DBAnalyzer dbanalyzer;


#endif
