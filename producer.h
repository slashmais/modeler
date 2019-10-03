#ifndef _modeler_producer_h_
#define _modeler_producer_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

struct Modeler;

struct Producer : public Ctrl
{
	using CLASSNAME=Producer;
	
	Modeler *pModeler;
	
	Producer();
	virtual ~Producer();

	bool Initialize(Modeler *pMod);
	void Terminate();
};



#endif
