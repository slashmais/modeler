
#include "analyzer.h"
#include "dbanalyzer.h"
#include "modeler.h"

Analyzer::Analyzer()
{
	pModeler=nullptr;
}

Analyzer::~Analyzer(){}
	
bool Analyzer::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	return true;
}

void Analyzer::Terminate(){}
