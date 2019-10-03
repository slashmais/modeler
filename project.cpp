
#include "schema.h"
#include "collector.h"

/*
	DBIDType ID;
	IDGenerator idgen; //generates id's unique for entire project
	std::string name;
	std::string note;
	std::map<IDType, std::pair<Schema*, SchemaCtrl*> schemes;
	Collector *pCollector;
*/


Project::Project()
{
	ID=0;
	idgen.prepare("schema");
	name="New Project";
	note.clear();
	schemas.clear();
	//define self as first topic..
	Schema S; S.name=name; S.note=note; S.type=TT_MAIN;
	S.id=idgen.newid("schema");
	AddSchema(S);
}

Project::~Project()
{
}
	
IDType Project::NewSchema()
{
}

void Project::AddSchema(const Schema &S)
{
	schemas[S.id]=S;
//	pCollector->RefreshProject(this);
}

void Project::RemoveSchema(IDType id)
{
	schemas.erase(id);
//	pCollector->RefreshProject(this);
}


void Project::OpenSchema(IDType id) //schemactrl
{
//	if (schemas.find(id)!=schemas.end()) pCollector->ShowProjectSchema(this, id);
}

void Project::CloseSchema(IDType id) //schemactrl & set to nullptr
{
//	if (schemas.find(id)!=schemas.end()) pCollector->HideProjectSchema(this, id);
}

