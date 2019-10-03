
#include "producer.h"
#include "dbproducer.h"
#include "modeler.h"

Producer::Producer()
{
	pModeler=nullptr;
}

Producer::~Producer() {}

bool Producer::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	return true;
}

void Producer::Terminate(){}

