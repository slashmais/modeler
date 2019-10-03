#ifndef _modeler_uppglobs_h_
#define _modeler_uppglobs_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;


#include <string>
#include <vector>
//#include <functional>


struct Panel : public Ctrl
{
	using CLASSNAME=Panel;
	Panel() {} // SetFrame(ThinOutsetFrame()); } //?
	virtual ~Panel(){}
	virtual void Paint(Draw &drw) { drw.DrawRect(GetSize(), SColorFace()); } //if needed...

};



#endif
