#ifndef _modeler_dbscoper_h_
#define _modeler_dbscoper_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

#include "scoper.h"

struct DBScoper : public DBsqlite3
{
	virtual ~DBScoper() {}
	
	bool ImplementSchema();

	//bool Save(JItem &I);
	//bool Load(ItemList &L);
	//bool Delete(JItem &I);

};

extern DBScoper dbscoper;


#endif
