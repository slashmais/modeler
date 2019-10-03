#ifndef _modeler_designer_h_
#define _modeler_designer_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

struct Modeler;

struct Designer : public Ctrl
{
	using CLASSNAME=Designer;
	
	Modeler *pModeler;
	
	Designer();
	virtual ~Designer();

	bool Initialize(Modeler *pMod);
	void Terminate();
};



#endif
