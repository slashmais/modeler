
#include "designer.h"
#include "dbdesigner.h"
#include <utilfuncs/utilfuncs.h>
#include "modeler.h"

Designer::Designer()
{
	pModeler=nullptr;
}

Designer::~Designer()
{
}

bool Designer::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	return true;
}

void Designer::Terminate(){}


