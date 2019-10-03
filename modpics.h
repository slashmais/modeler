#ifndef _modeler_modpics_h_
#define _modeler_modpics_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

enum
{
	MID_NOTE=0, //..from here.. (must be first)
	MID_INFO,
	MID_TODO,
	MID_ATTENTION,	//..used in this order by journal..
	MID_ISSUE,
	MID_HISTORY, //..to here..
	MID_DEFAULT,
	PICTBCURSOR,
	PICARROW,
	PICDRAWTOOL,
	PICUNDO,
	PICREDO,
	PICSELECT,
	PICPEN,
	PICLINE,
	PICRECT,
	PICOVAL,
	PICJOT,
	//? PICPAINT,
	PICSIZE1,
	PICSIZE3,
	PICSIZE5,
	PICFG,
	PICBG,
	PICDBBUSY,
	PICDBREADY,
	PICICON,
};

Image GetPic(int p);


#endif
