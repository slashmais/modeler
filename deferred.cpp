
#include "deferred.h"
#include "dbdeferred.h"
#include "modeler.h"


Deferred::Deferred()
{
	pModeler=nullptr;
}

Deferred::~Deferred() {}


bool Deferred::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	return true;
}

void Deferred::Terminate(){}


