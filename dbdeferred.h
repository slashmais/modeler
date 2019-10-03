#ifndef _modeler_dbdeferred_h_
#define _modeler_dbdeferred_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

#include "deferred.h"

struct DBDeferred : public DBsqlite3
{
	virtual ~DBDeferred() {}
	
	bool ImplementSchema();

	//bool Save(JItem &I);
	//bool Load(ItemList &L);
	//bool Delete(JItem &I);

};

extern DBDeferred dbdeferred;


#endif
