#ifndef _modeler_dblibrary_h_
#define _modeler_dblibrary_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

#include "library.h"


struct DBLibrary : public DBsqlite3
{
	virtual ~DBLibrary() {}
	
	bool ImplementSchema();

	//bool Save(JItem &I);
	//bool Load(ItemList &L);
	//bool Delete(JItem &I);

};

extern DBLibrary dblibrary;




#endif
