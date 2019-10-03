#ifndef _modeler_analyzer_h_
#define _modeler_analyzer_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include "globdefs.h"
#include <dbsqlite3/dbsqlite3types.h>

struct Modeler;

struct Analyzer : public Ctrl
{
	using CLASSNAME=Analyzer;
	
	Modeler *pModeler;
	
	Analyzer();
	~Analyzer();
	
	bool Initialize(Modeler *pMod);
	void Terminate();
	
};


#endif
