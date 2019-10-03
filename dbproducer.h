#ifndef _modeler_dbproducer_h_
#define _modeler_dbproducer_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

#include "producer.h"

struct DBProducer : public DBsqlite3
{
	virtual ~DBProducer() {}
	
	bool ImplementSchema();

	//bool Save(JItem &I);
	//bool Load(ItemList &L);
	//bool Delete(JItem &I);

};

extern DBProducer dbproducer;


#endif
