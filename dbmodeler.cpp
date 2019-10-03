
#include "dbmodeler.h"
#include "modeler.h"

DBModeler dbmodeler;

bool DBModeler::ImplementSchema()
{
	bool b=bDBOK;
	DBResult RS;
	std::string SQL;
	
	if (b)
	{
		SQL = "CREATE TABLE IF NOT EXISTS IDS (name, id, del)";
		ExecSQL(&RS, SQL); //create/reuse ids: init_ids(tabelname) once after CREATE TABLE.. and new_id() / del_id() after
		b=NoError(this);
	}
	
	if (b) b = (MakeTable("projects", "idproject, name, workspace, description, lasttab")&&init_ids("projects"));

	//table: "project_custom" (idproject, lasttab, ???) //lasttab=last active/used b4 close

	return b;
}

bool DBModeler::SaveProject(Project &project)
{
	DBResult RS;
	std::string SQL;
	//size_t n=0;
	if (project.idproject!=0)
	{
		SQL=spf("UPDATE projects SET name=", SQLSafeQ(project.name),
							", workspace=", SQLSafeQ(project.workspace),
							", description=", SQLSafeQ(project.description),
							", lasttab=", project.lasttab,
							" WHERE idproject=", project.idproject);
		ExecSQL(&RS, SQL);
	}
	else
	{
		project.idproject=new_id("projects");
		SQL=spf("INSERT INTO projects(idproject, name, workspace, description, lasttab)",
							" VALUES(", project.idproject,
									", ", SQLSafeQ(project.name),
									", ", SQLSafeQ(project.workspace),
									", ", SQLSafeQ(project.description),
									", ", project.lasttab,
									")");
		ExecSQL(&RS, SQL);
	}
	return NoError(this);
}

bool DBModeler::DeleteProject(Project &project)
{
	DBResult RS;
	del_id("projects", project.idproject);
	ExecSQL(&RS, spf("DELETE FROM projects WHERE idproject=", project.idproject));
	return NoError(this);
}

bool DBModeler::Load(ProjectList &L)
{
	DBResult RS;
	size_t i=0,n;
	L.clear();
	n=ExecSQL(&RS, "SELECT * FROM projects");
	if (NoError(this))
	{
		while (i<n)
		{
			Project P;
			P.idproject=stot<int>(RS.GetVal("idproject", i));
			P.name=SQLRestore(RS.GetVal("name", i));
			P.workspace=SQLRestore(RS.GetVal("workspace", i));
			P.description=SQLRestore(RS.GetVal("description", i));
			P.lasttab=stot<int>(RS.GetVal("lasttab", i));
			L.list[P.idproject]=P;
			i++;
		}
		return true; //empty ok
	}
	return false;
}

