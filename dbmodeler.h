#ifndef _modeler_dbmodeler_h_
#define _modeler_dbmodeler_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>
#include "globdefs.h"


struct Project;
struct ProjectList;

struct DBModeler : public DBsqlite3
{
	virtual ~DBModeler() {}
	
	bool ImplementSchema();

	bool SaveProject(Project &project);
	bool DeleteProject(Project &project);
	bool Load(ProjectList &L);

};

extern DBModeler dbmodeler;

#endif
