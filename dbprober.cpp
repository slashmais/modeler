
#include "dbprober.h"
#include "prober.h"
#include <utilfuncs/utilfuncs.h>


//-------------------------------------------------------------------------------------------------
DBProber dbprober;

//-------------------------------------------------------------------------------------------------
bool DBProber::ImplementSchema()
{
	bool b=bDBOK;
	DBResult RS;
	std::string SQL;
	
	if (b)
	{
		SQL = "CREATE TABLE IF NOT EXISTS IDS (name, id, del)";
		ExecSQL(&RS, SQL); //create/reuse ids: init_ids(tabelname) once after CREATE TABLE.. and new_id() / del_id() after
		b=NoError(this);
	}
	
	/* SEE: DEV_NOTES.txt (Prober) */

	if (b) b = (MakeTable("items", "iditem, name, note")&&init_ids("items"));//re-use/share
	if (b) b = (MakeTable("pages", "idpage, X, Y, pgAn, pgPL")&&init_ids("pages"));
	if (b) b = (MakeTable("seeds", "idseed, idpage, iditem, x, y, disp")&&init_ids("seeds")); //one item can have multiple guises
	if (b) b = (MakeTable("assocs", "idassoc, idpage, iditem, fidseed, tidseed")&&init_ids("assocs"));
	if (b) b = (MakeTable("tags", "idtag, idpage, tagdata")&&init_ids("tags")); //tagdata==map::shape::shapedata

	return b;
}

//-------------------------------------------------------------------------------------------------items
bool DBProber::Save(SItem &I)
{
	DBResult RS;
	std::string SQL;
	if (I.iditem>0)
	{
		SQL=spf("UPDATE items SET name=", SQLSafeQ(I.name),
								", note=", SQLSafeQ(I.def),
								" WHERE iditem=", I.iditem);
		ExecSQL(&RS, SQL);
	}
	else
	{
		I.iditem=new_id("items");
		SQL=spf("INSERT INTO items(iditem, name, note) VALUES(", I.iditem,
															 ", ", SQLSafeQ(I.name),
															 ", ", SQLSafeQ(I.def),
															 ")");
		ExecSQL(&RS, SQL);
	}
	return NoError(this);
}

bool DBProber::Delete(SItem &I)
{
	DBResult RS;
	del_id("items", I.iditem);
	ExecSQL(&RS, spf("DELETE FROM items WHERE iditem=", I.iditem));
	I.iditem=0;
	return NoError(this);
}

bool DBProber::Load(SItems &L)
{
	DBResult RS;
	std::string SQL;
	size_t i=0, n;
	L.clear();
	n=ExecSQL(&RS, "SELECT * FROM items");
	if (NoError(this))
	{
		while (i<n)
		{
			SItem I;
			I.iditem=stot<int>(RS.GetVal("iditem", i));
			I.name=SQLRestore(RS.GetVal("name", i));
			I.def=SQLRestore(RS.GetVal("note", i));
			L.push(I);
			i++;
		}
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------seeds
bool DBProber::Save(Seed &S)
{
	if ((S.idpage==0)||(S.iditem==0)) { SetLastError("[Save(Seed)]: invalid page or item"); return false; }
	DBResult RS;
	std::string SQL;
	if (S.idseed>0)
	{
		SQL=spf("UPDATE seeds SET idpage=", S.idpage,
								", iditem=", S.iditem,
								", x=", S.x,
								", y=", S.y,
								", disp=", S.disp,
								" WHERE idseed=", S.idseed);
		ExecSQL(&RS, SQL);
	}
	else
	{
		S.idseed=new_id("seeds");
		SQL=spf("INSERT INTO seeds(idseed, idpage, iditem, x, y, disp)",
							" VALUES(", S.idseed,
									", ", S.idpage,
									", ", S.iditem,
									", ", S.x,
									", ", S.y,
									", ", S.disp,
									")");
		ExecSQL(&RS, SQL);
	}
	return NoError(this);
}

bool DBProber::Delete(Seed &S)
{
	DBResult RS;
	del_id("seeds", S.idseed);
	ExecSQL(&RS, spf("DELETE FROM seeds WHERE idseed=", S.idseed));
	return NoError(this);
}

bool DBProber::Load(Seeds &L, int pageid)
{
	if (pageid==0) { SetLastError("[Load(Seeds)]: invalid page"); return false; }
	DBResult RS;
	std::string SQL;
	size_t i=0, n;
	L.clear();
	n=ExecSQL(&RS, spf("SELECT * FROM seeds WHERE idpage=", pageid));
	if (NoError(this))
	{
		while (i<n)
		{
			Seed S;
			S.idseed=stot<int>(RS.GetVal("idseed", i));
			S.idpage=stot<int>(RS.GetVal("idpage", i));
			S.iditem=stot<int>(RS.GetVal("iditem", i));
			S.x=stot<int>(RS.GetVal("x", i));
			S.y=stot<int>(RS.GetVal("y", i));
			S.disp=stot<int>(RS.GetVal("disp", i));
			L.push(S);
			i++;
		}
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------assocs
bool DBProber::Save(Assoc &A)
{
	if ((A.idpage==0)||(A.iditem==0)) { SetLastError("[Save(Assoc)]: invalid page or item"); return false; }
	DBResult RS;
	std::string SQL;
	if (A.idassoc>0)
	{
		SQL=spf("UPDATE assocs SET idpage=", A.idpage,
								", iditem=", A.iditem,
								", fidseed=", A.fidseed,
								", tidseed=", A.tidseed,
								" WHERE idassoc=", A.idassoc);
		ExecSQL(&RS, SQL);
	}
	else
	{
		A.idassoc=new_id("assocs");
		SQL=spf("INSERT INTO assocs(idassoc, idpage, iditem, fidseed, tidseed)",
							" VALUES(", A.idassoc,
							", ", A.idpage,
							", ", A.iditem,
							", ", A.fidseed,
							", ", A.tidseed,
							")");
		ExecSQL(&RS, SQL);
	}
	return NoError(this);
}

bool DBProber::Delete(Assoc &A)
{
	DBResult RS;
	del_id("assocs", A.idassoc);
	ExecSQL(&RS, spf("DELETE FROM assocs WHERE idassoc=", A.idassoc));
	return NoError(this);
}

bool DBProber::Load(Assocs &L, int pageid)
{
	DBResult RS;
	std::string SQL;
	size_t i=0, n;
	L.clear();
	n=ExecSQL(&RS, spf("SELECT * FROM assocs WHERE idpage=", pageid));
	if (NoError(this))
	{
		while (i<n)
		{
			Assoc A;
			A.idassoc=stot<int>(RS.GetVal("idassoc", i));
			A.idpage=stot<int>(RS.GetVal("idpage", i));
			A.iditem=stot<int>(RS.GetVal("iditem", i));
			A.fidseed=stot<int>(RS.GetVal("fidseed", i));
			A.tidseed=stot<int>(RS.GetVal("tidseed", i));
			L.push(A);
			i++;
		}
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------tags
bool DBProber::Save(Tag &T)
{
	DBResult RS;
	std::string SQL;
	if (T.idtag>0)
	{
		SQL=spf("UPDATE tags SET idpage=", T.idpage, ", tagdata=", SQLSafeQ(T.tagdata), " WHERE idtag=", T.idtag);
		ExecSQL(&RS, SQL);
	}
	else
	{
		T.idtag=new_id("tags");
		SQL=spf("INSERT INTO tags(idtag, idpage, tagdata) VALUES(", T.idtag, ", ", T.idpage, ", ", SQLSafeQ(T.tagdata), ")");
		ExecSQL(&RS, SQL);
	}
	return NoError(this);
}

bool DBProber::Delete(Tag &T)
{
	DBResult RS;
	del_id("tags", T.idtag);
	ExecSQL(&RS, spf("DELETE FROM tags WHERE idtag=", T.idtag));
	T.idtag=0;
	return NoError(this);
}

bool DBProber::Load(Tags &L, int pageid)
{
	DBResult RS;
	size_t i=0, n;
	L.clear();
	n=ExecSQL(&RS, spf("SELECT * FROM tags WHERE idpage=", pageid));
	if (NoError(this))
	{
		while (i<n)
		{
			Tag tag;
			tag.idtag=stot<int>(RS.GetVal("idtag",i));
			tag.idpage=stot<int>(RS.GetVal("idpage",i));
			tag.tagdata=SQLRestore(RS.GetVal("tagdata",i));
			L.push(tag);
			i++;
		}
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------pages
bool DBProber::Save(SPage &P)
{
	DBResult RS;
	std::string SQL;
	if (P.idpage>0)
	{
		SQL=spf("UPDATE pages SET X=", P.X, ", Y=", P.Y, ", pgAn=", P.paperAn, ", pgPL=", P.pagePL, " WHERE idpage=", P.idpage);
		ExecSQL(&RS, SQL);
	}
	else
	{
		P.idpage=new_id("pages");
		SQL=spf("INSERT INTO pages(idpage, X, Y, pgAn, pgPL) VALUES(", P.idpage, ", ", P.X, ", ", P.Y, ", ", P.paperAn, ", ", P.pagePL, ")");
		ExecSQL(&RS, SQL);
	}
	return NoError(this);
}

bool DBProber::Delete(SPage &P)
{
	DBResult RS;
	ExecSQL(&RS, spf("DELETE FROM seeds WHERE idpage=", P.idpage));
	ExecSQL(&RS, spf("DELETE FROM assocs WHERE idpage=", P.idpage));
	ExecSQL(&RS, spf("DELETE FROM tags WHERE idpage=", P.idpage));
	del_id("pages", P.idpage);
	ExecSQL(&RS, spf("DELETE FROM pages WHERE idpage=", P.idpage));
	return NoError(this);
}

bool DBProber::Load(SPages &L)
{
	DBResult RS;
	std::string SQL;
	size_t i=0, n;
	n=ExecSQL(&RS, "SELECT * FROM pages");
	if (NoError(this))
	{
		bool b=true;
		while (b&&(i<n))
		{
			SPage P;
			P.idpage=stot<int>(RS.GetVal("idpage", i));
			P.X=stot<int>(RS.GetVal("X", i));
			P.Y=stot<int>(RS.GetVal("Y", i));
			P.paperAn=stot<int>(RS.GetVal("pgAn", i));
			P.pagePL=stot<int>(RS.GetVal("pgPL", i));
			L.push(P);
			i++;
		}
		return true;
	}
	return false;
}
