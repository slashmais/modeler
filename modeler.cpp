
#include "modeler.h"
#include <utilfuncs/utilfuncs.h>
#include "dbmodeler.h"
#include "modpics.h"

AppConfig ModConf;

bool Project::Save() { return tellifdberror(dbmodeler.SaveProject(*this), &dbmodeler); }
bool Project::Delete() { return tellifdberror(dbmodeler.DeleteProject(*this), &dbmodeler); }

bool ProjectList::Load() { return tellifdberror(dbmodeler.Load(*this), &dbmodeler); }

Modeler::Modeler()
{
	Icon(GetPic(PICICON));
	Title("Modeler");
	Sizeable();

	pcurProject=nullptr;
	pWaitCur=nullptr;
	
	AddFrame(MSB);
	MSB.ShowDBStatus();
	
	pwordlist=nullptr;
	pprober=nullptr;
	pcollector=nullptr;
	pscoper=nullptr;
	panalyzer=nullptr;
	pdesigner=nullptr;
	pproducer=nullptr;

	panProject.Add(lblRD.SetLabel(t_("Project:")).LeftPosZ(4, 50).TopPosZ(4, 20));
	panProject.Add(cbProject.LeftPosZ(60, 292).TopPosZ(4, 20));
	panProject.Add(btnE.SetLabel(t_("..")).SetFont(StdFont().Bold()).Tip(t_("edit")).LeftPosZ(356, 20).TopPosZ(4, 20));
	panProject.Add(btnA.SetLabel(t_("+")).SetFont(StdFont().Bold()).Tip(t_("add")).LeftPosZ(380, 20).TopPosZ(4, 20));
	panProject.Add(btnD.SetLabel(t_("-")).SetFont(StdFont().Bold()).Tip(t_("delete")).LeftPosZ(404, 20).TopPosZ(4, 20));

	Add(panProject.SetFrame(ThinInsetFrame()).HSizePosZ(8, 8).TopPosZ(6, 30));
	Add(Tabs.SetFrame(ThinInsetFrame()).HSizePosZ(8, 8).VSizePosZ(40, 8));

	btnE.Tip("Edit").WhenAction << [&]{ EditProject(); };
	btnA.Tip("Add").WhenAction << [&]{ AddProject(); };
	btnD.Tip("Remove").WhenAction << [&]{ DeleteProject(); };

	AddPinnedTabs();
	
	FillProject();
	cbProject.WhenAction << [&]{ change_project(); };
	
	Tabs.WhenAction << [&]{ if (pcurProject) { pcurProject->lasttab=Tabs.Get(); pcurProject->Save(); }};
	
}

Modeler::~Modeler()
{
	CloseProject();
	ClosePinnedTabs();
}

void Modeler::OnDBBusy(bool b) //demblinkenlights
{
	static int nwc=0;
	
	if (b&&pWaitCur) { nwc++; return; }
	if (!b&&pWaitCur&&(nwc>1)) { nwc--; return; }
	if (b) { pWaitCur=new WaitCursor; pWaitCur->Show(); nwc++; }
	else { delete pWaitCur; pWaitCur=nullptr; nwc=0; }
	MSB.ShowDBStatus(b);
	Sync();
}

void Modeler::AddPinnedTabs()
{
	pin_tab("Journal", "dbjournal.sqlite3", journal, dbjournal);
	pin_tab("Library", "dblibrary.sqlite3", library, dblibrary);
	pin_tab("Deferred", "dbdeferred.sqlite3", deferred, dbdeferred);
}

void Modeler::ClosePinnedTabs()
{
	deferred.Terminate();
	dbdeferred.Close();
	library.Terminate();
	dblibrary.Close();
	journal.Terminate();
	dbjournal.Close();
}

void Modeler::FillProject()
{
	projectlist.Load();
	cbProject.Clear();
	for (auto p:projectlist.list)
	{
		cbProject.Add(p.second.name.c_str());
	}
}

void Modeler::change_project()
{
	MSB.ShowDBStatus(true); //red-dot
	Tabs.Set(0); //select journal-tab
	CloseProject(); //all Project-specific modules
	if (cbProject.GetIndex()<0) { pcurProject=nullptr; return; }
	pcurProject=projectlist.Get(cbProject.Get().ToString().ToStd());
	OpenProject();
	Tabs.Set(pcurProject->lasttab);
	MSB.ShowDBStatus(); //green
}

bool Modeler::edit_project(std::string &sn, std::string &sw, std::string &sd, bool bnew)
{
	DlgProject dlg;
	dlg.ebN.SetData(sn.c_str());
	dlg.ebW.SetData(sw.c_str());
	dlg.lblw.Enable(bnew);
	dlg.ebW.Enable(bnew);
	dlg.deD.SetData(sd.c_str());
	dlg.Execute();
	if (dlg.bok)
	{
		std::string tn, tw, td;
		tn=dlg.ebN.GetData().ToString().ToStd(); TRIM(tn);
		tw=dlg.ebW.GetData().ToString().ToStd(); TRIM(tw);
		td=dlg.deD.GetData().ToString().ToStd();
		if (tn.empty()||tw.empty()) return false;
		sn=tn; sw=tw; sd=td;
		return true;
	}
	return false;
}

void Modeler::AddProject()
{
	std::string sn{};
	std::string sw{};
	std::string sd{};
	if (edit_project(sn, sw, sd))
	{
		if (pcurProject)
		{
			if (projectlist.hasname(sn)&&(!seqs(pcurProject->name, sn))) { tellerror("Name already used"); return; }
			if (projectlist.hasworkspace(sw)&&(!seqs(pcurProject->workspace, sw))) { tellerror("Workspace already used"); return; }
		}
		else
		{
			if (projectlist.hasname(sn)) { tellerror("Name already used"); return; }
			if (projectlist.hasworkspace(sw)) { tellerror("Workspace already used"); return; }
		}
		Project(sn, sw, sd).Save();
		FillProject();
		journal.AddItem(MID_ATTENTION, sn, spf("Working notes for project '", sn, "' concerning ", sd, "\n"));
		cbProject.SetData(sn.c_str());
		change_project();
	}
}

void Modeler::EditProject()
{
	if (!pcurProject) return;
	std::string sn=pcurProject->name;
	std::string sw=pcurProject->workspace;
	std::string sd=pcurProject->description;
	if (edit_project(sn, sw, sd, false))
	{
		if (projectlist.hasname(sn)&&(!seqs(pcurProject->name, sn))) { tellerror("Name already used"); return; }
		if (projectlist.hasworkspace(sw)&&(!seqs(pcurProject->workspace, sw))) { tellerror("Workspace already used"); return; }
		bool b=(!seqs(pcurProject->workspace, sw));
		pcurProject->name=sn;
		pcurProject->workspace=sw;
		pcurProject->description=sd;
		pcurProject->Save();
		FillProject();
		cbProject.SetData(sn.c_str());
		update_modules();
		if (b) change_project();
	}
}

void Modeler::DeleteProject()
{
	if (!pcurProject) return;
	std::string sw=path_append(pcurProject->workspace, "modeler");
	if (!dir_exist(sw)){ tellerror("Cannot access workdir: '", sw, "'"); return; }
	std::string sn=pcurProject->name;
	std::string sdelname=spf(path_name(sw), "_DELETED_PROJECT");
	std::string msg=spf("Do you want remove project '", sn, "'?",
						"\n\nThe workspace-entry '", sw, "'",
						"\nwill be renamed to '", sdelname, "'");
	
	if (PromptOKCancel(DeQtf(msg.c_str())))
	{
		dir_rename(sw, sdelname);
		pcurProject->Delete();
		pcurProject=nullptr;
		FillProject();
		change_project();
	}
}

void Modeler::update_modules()
{
	pprober->update_main_topic(pcurProject->name, pcurProject->description);
	//todo: re-parse description ...
}

void Modeler::OpenProject()
{
	if (pcurProject)
	{
		bool b;
		setup_projectwordlist();

		b=((add_tab("Prober", "dbprober.sqlite3", &pprober, dbprober)) && pprober->Initialize(this));
		if (b) b=((add_tab("Collector", "dbcollector.sqlite3", &pcollector, dbcollector)) && pcollector->Initialize(this));
		if (b) b=((add_tab("Scoper", "dbscoper.sqlite3", &pscoper, dbscoper)) && pscoper->Initialize(this));
		if (b) b=((add_tab("Analyzer", "dbanalyzer.sqlite3", &panalyzer, dbanalyzer)) && panalyzer->Initialize(this));
		if (b) b=((add_tab("Designer", "dbdesigner.sqlite3", &pdesigner, dbdesigner)) && pdesigner->Initialize(this));
		if (b) b=((add_tab("Producer", "dbproducer.sqlite3", &pproducer, dbproducer)) && pproducer->Initialize(this));

		if (!b) tellerror("some module(s) could not be activated"); //todo todo something
	}
}

void Modeler::setup_projectwordlist()
{
	if (pwordlist) close_projectwordlist();
	
	pwordlist=new WordList;
	if (!pwordlist) return; //throw?
	
	std::string WorkDir=pcurProject->workspace;
	const std::string WLDB{"dbwordlist.sqlite3"};
	std::string DBPath=path_append(WorkDir, "modeler");
	if (!path_realize(DBPath)) { tellerror("Cannot access workdir: '", DBPath, "'"); return; }
	DBPath=path_append(DBPath, WLDB);
	if (!file_exist(DBPath)) //copy base-db from HoM/modeler if not existing
	{
		std::string srcdb=path_append(ModConf.QDPath, WLDB);
		srcdb+=".BASE";
		if (!file_copy(srcdb, DBPath)) { tellerror("Cannot prepare wordlist-database"); return; }
	}
	//bool bok;
	if (!dbwordlist.Open(DBPath)) tellerror("Database: '", DBPath, "' - failed: ", dbwordlist.GetLastError());
	pwordlist->Initialize(this);
	
	//todo todo if Initialize() fails - do something
	
}

void Modeler::CloseProject()
{
	close_projectwordlist();
	
	if (pprober) remove_tab(&pprober, dbprober);
	if (pcollector) remove_tab(&pcollector, dbcollector);
	if (pscoper) remove_tab(&pscoper, dbscoper);
	if (panalyzer) remove_tab(&panalyzer, dbanalyzer);
	if (pdesigner) remove_tab(&pdesigner, dbdesigner);
	if (pproducer) remove_tab(&pproducer, dbproducer);
}

void Modeler::close_projectwordlist()
{
	if (!pwordlist) return;
	pwordlist->Terminate();
	dbwordlist.Close();
	pwordlist->clear();
	delete pwordlist;
	pwordlist=nullptr;
}




