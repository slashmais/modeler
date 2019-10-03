#ifndef _modeler_library_h_
#define _modeler_library_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

struct Modeler;

struct Library : public Ctrl
{
	using CLASSNAME=Library;
	
	Modeler *pModeler;
	
	Library();
	virtual ~Library();
	
	bool Initialize(Modeler *pMod);
	void Terminate();

};

#endif
