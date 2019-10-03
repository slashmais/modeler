
#include "scoper.h"
#include "dbscoper.h"
#include "modeler.h"

Scoper::Scoper()
{
	pModeler=nullptr;
}

Scoper::~Scoper() {}

bool Scoper::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	return true;
}

void Scoper::Terminate(){}
