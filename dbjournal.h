#ifndef _modeler_dbjournal_h_
#define _modeler_dbjournal_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

//struct JItem;
#include "journal.h"


struct DBJournal : public DBsqlite3
{
	virtual ~DBJournal() {}
	
	bool ImplementSchema();

	bool Save(JItem &I);
	bool Load(ItemList &L);
	bool Delete(JItem &I);

};

extern DBJournal dbjournal;

#endif
