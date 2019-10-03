
#include "library.h"
#include "dblibrary.h"
#include "modeler.h"

Library::Library()
{
	pModeler=nullptr;
}
	
Library::~Library(){}

bool Library::Initialize(Modeler *pMod)
{
	pModeler=pMod;
	return true;
}

void Library::Terminate(){}
