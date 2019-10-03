#ifndef _modeler_dbprober_h_
#define _modeler_dbprober_h_

#include <dbsqlite3/dbsqlite3.h>
#include <string>

struct SItem;
struct SItems;
struct SPage;
struct SPages;
struct Seed;
struct Seeds;
struct Assoc;
struct Assocs;
struct Tag;
struct Tags;

struct DBProber : public DBsqlite3
{
	virtual ~DBProber() {}
	
	bool ImplementSchema();

	bool Save(SItem &I);
	bool Delete(SItem &I);
	bool Load(SItems &L);

	bool Save(Seed &S);
	bool Delete(Seed &S);
	bool Load(Seeds &L, int pageid);

	bool Save(Assoc &A);
	bool Delete(Assoc &A);
	bool Load(Assocs &L, int pageid);

	bool Save(Tag &T);
	bool Delete(Tag &T);
	bool Load(Tags &L, int pageid);

	bool Save(SPage &P);
	bool Delete(SPage &P);
	bool Load(SPages &L);


};

extern DBProber dbprober;


#endif
