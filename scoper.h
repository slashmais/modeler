#ifndef _modeler_scoper_h_
#define _modeler_scoper_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

struct Modeler;

struct Scoper : public Ctrl
{
	using CLASSNAME=Scoper;
	
	Modeler *pModeler;
	
	Scoper();
	virtual ~Scoper();

	bool Initialize(Modeler *pMod);
	void Terminate();

};


#endif
