#ifndef _collector_h_
#define _collector_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

struct Modeler;

struct Collector : public Ctrl
{
	using CLASSNAME=Collector;
	
	Modeler *pModeler;
	
	Collector();
	virtual ~Collector();

	bool Initialize(Modeler *pMod);
	void Terminate();
};




#endif
