#ifndef _modeler_dbdesigner_h_
#define _modeler_dbdesigner_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

#include "designer.h"


struct DBDesigner : public DBsqlite3
{
	virtual ~DBDesigner() {}
	
	bool ImplementSchema();

	//bool Save(JItem &I);
	//bool Load(ItemList &L);
	//bool Delete(JItem &I);

};

extern DBDesigner dbdesigner;



#endif
