#ifndef _dbcollector_h_
#define _dbcollector_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

#include "collector.h"



///NOT using DBIDType=DB_ID_TYPE; !!!

struct DBCollector : public DBsqlite3
{
	virtual ~DBCollector() {}
	
	bool ImplementSchema();

	//bool Save(JItem &I);
	//bool Load(ItemList &L);
	//bool Delete(JItem &I);

};

extern DBCollector dbcollector;







#endif
