
#include "modpics.h"

#define  IMAGEFILE <modeler/modpics.iml>
#define  IMAGECLASS MODPICS
#include <Draw/iml.h>


Image GetPic(int p)
{
	switch(p)
	{
		case MID_INFO:			return MODPICS::picInfo();
		case MID_TODO:			return MODPICS::picTodo();
		case MID_ATTENTION:		return MODPICS::picAttention();
		case MID_ISSUE:			return MODPICS::picIssue();
		case MID_HISTORY:		return MODPICS::picHistory();
		case PICTBCURSOR:		return MODPICS::picTBCursor();
		case PICARROW:			return MODPICS::picArrow();
		case PICDRAWTOOL:		return MODPICS::picDrawTool();
		case PICUNDO:			return MODPICS::picUndo();
		case PICREDO:			return MODPICS::picRedo();
		case PICSELECT:			return MODPICS::picSelect();
		case PICPEN:			return MODPICS::picPen();
		case PICLINE:			return MODPICS::picLine();
		case PICRECT:			return MODPICS::picRect();
		case PICOVAL:			return MODPICS::picOval();
		case PICJOT:			return MODPICS::picJot();
		//? case PICPAINT:			return MODPICS::picPaint();
		case PICSIZE1:			return MODPICS::picSize1();
		case PICSIZE3:			return MODPICS::picSize3();
		case PICSIZE5:			return MODPICS::picSize5();
		case PICFG:				return MODPICS::picFG();
		case PICBG:				return MODPICS::picBG();
		//case PICDBBUSY:			return MODPICS::pic_red_dot();
		//case PICDBREADY:		return MODPICS::pic_green_dot();
		case PICDBBUSY:			return MODPICS::picredled();
		case PICDBREADY:		return MODPICS::picgreenled();
		case PICICON:			return MODPICS::picIcon();
		
	}
	return MODPICS::picDefault();
}


