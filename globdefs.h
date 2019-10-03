#ifndef _globals_h_
#define _globals_h_


#include <idgen/idgen.h>
#include <cstdint>


typedef IDGen<uint64_t> IDGenerator;
using IDType=IDGenerator::IDType;

struct PaperSize { int w, h; };
void GetPaperSize(int nAn, int &w, int &h);

//moved to mapwidget...
//enum TOPICTYPE
//{
//	TT_GENERIC=0,
//	TT_MAIN=1,
//	TT_ISSUE, //"the scream"!
//	TT_IDEA,
//	TT_TASK,
//	TT_DATA,
//	TT_EVALUATE,
//	TT_NOTE,
//	TT_THING,
//	TT_USERINTERFACE,
//	TT_OBJECTIVE,
//	TT_PROJECT,
//	TT_PROCESS,
//	TT_ROLE,
//	TT_RULE,
//	TT_WARN,
//
//};
//
//enum LINKTYPE
//{
//	LT_ASSOC=0, // [(C)from]---[(C)to] C==center
//	LT_FLOW, // [(R)from]-->--[(L)to] // (proc)-->--(proc)
//	LT_DATAIN, // [(C)from]--#>--[(C)to] // |data-in|--#>--(proc)--#>--|data-out|
//	LT_DATAOUT, // [(C)from]--#>--[(C)to] // |data-in|--#>--(proc)--#>--|data-out|
//	LT_CONTENT, // [(C)from]--#>--[(C)to] // |container|--+>--|content|
//	LT_ONETOMANY,
//};

enum { LANDSCAPE, PORTRAIT, };


#endif
