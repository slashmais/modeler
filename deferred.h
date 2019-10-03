#ifndef _modeler_deferrer_h_
#define _modeler_deferrer_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

struct Modeler;


struct Deferred : public Ctrl
{
	using CLASSNAME=Deferred;

	Modeler *pModeler;
	
	Deferred();
	virtual ~Deferred();
	
	bool Initialize(Modeler *pMod);
	void Terminate();
};


#endif
