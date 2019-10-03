
#include "collector.h"
#include "dbcollector.h"
#include "modeler.h"


Collector::Collector()
{
	pModeler=nullptr;
}

Collector::~Collector() {}

bool Collector::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	return true;
}

void Collector::Terminate(){}
