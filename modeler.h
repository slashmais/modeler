#ifndef _modeler_h_
#define _modeler_h_

/*

	this is a fully functional Proof of Concept prototype
	(a model-of-a-model)
	
*/


#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include <HoM/HoM.h>
#include "collector.h"
#include <string>
#include <map>
#include "uppglobs.h"
#include "globdefs.h"
#include "modpics.h" //for ModStatus

//global-mods
#include "dbmodeler.h"

#include "journal.h"
#include "dbjournal.h"

#include "library.h"
#include "dblibrary.h"

#include "deferred.h"
#include "dbdeferred.h"

//project-mods
#include "wordlist.h"
#include "dbwordlist.h"

#include "prober.h"
#include "dbprober.h"

#include "collector.h"
#include "dbcollector.h"

#include "scoper.h"
#include "dbscoper.h"

#include "analyzer.h"
#include "dbanalyzer.h"

#include "designer.h"
#include "dbdesigner.h"

#include "producer.h"
#include "dbproducer.h"

extern AppConfig ModConf;
extern DBModeler dbmodeler;


struct Project
{
	int idproject;
	std::string name;
	std::string workspace;
	std::string description;
	int lasttab; //last active
	
	void clear() { name.clear(); workspace.clear(); description.clear(); lasttab=0; }

	virtual ~Project() { clear(); }
	Project() { clear(); idproject=0; }
	Project(const std::string &n, const std::string &w, const std::string &d="") { clear(); idproject=0; name=n; workspace=w; description=d; }
	Project(const Project &P) { *this=P; }
	Project& operator=(const Project &P)
	{
		idproject=P.idproject;
		name=P.name;
		workspace=P.workspace;
		description=P.description;
		lasttab=P.lasttab;
		return *this;
	}
	
	bool Save();
	bool Delete();
};

struct ProjectList
{
	std::map<int, Project> list; //make private?
	void clear() { list.clear(); }
	virtual ~ProjectList() { clear(); }
	ProjectList() { clear(); }
	ProjectList(const ProjectList &AL) { *this=AL; }
	ProjectList& operator=(const ProjectList &AL) { list=AL.list; return *this; }
	bool Load();
	bool hasname(const std::string &sn)
	{
		for (auto p:list) { if (seqs(p.second.name, sn)) return true; }
		return false;
	}
	bool hasworkspace(const std::string &ws)
	{
		for (auto p:list) { if (seqs(p.second.workspace, ws)) return true; }
		return false;
	}
	Project* Get(int projectid) { if (list.find(projectid)!=list.end()) return &(list[projectid]); return nullptr; }
	Project* Get(const std::string &sn)
	{
		for (auto p:list) { if (seqs(p.second.name, sn)) return &(list[p.first]); }
		return nullptr;
	}
};


struct DlgProject : public TopWindow
{
	using CLASSNAME=DlgProject;
	bool bok;
	Label lbln;
	EditString ebN;
	Label lblw;
	EditString ebW;
	Button btnWS;
	Label lbld;
	DocEdit deD;
	Button btnOK;
	Button btnCancel;
	
	DlgProject()
	{
		Title("Edit Project");
		SetRect(0,0,800,600);
		Sizeable();
		bok=false;

		Add(lbln.SetLabel(t_("Project:")).SetAlign(ALIGN_RIGHT).LeftPosZ(4, 60).TopPosZ(4, 19));
		Add(ebN.LeftPosZ(68, 240).TopPosZ(4, 19));
		Add(lblw.SetLabel(t_("Workspace:")).SetAlign(ALIGN_RIGHT).LeftPosZ(4, 60).TopPosZ(28, 19));
		Add(ebW.LeftPosZ(68, 280).TopPosZ(28, 19));
		Add(btnWS.SetLabel(t_("..")).SetFont(StdFont().Bold()).LeftPosZ(352, 20).TopPosZ(28, 20));
		Add(lbld.SetLabel(t_("Description")).LeftPosZ(4, 68).TopPosZ(52, 19));
		Add(deD.HSizePosZ(4, 4).VSizePosZ(72, 48));
		Add(btnOK.SetLabel(t_("OK")).RightPosZ(84, 60).BottomPosZ(12, 20));
		Add(btnCancel.SetLabel(t_("Cancel")).RightPosZ(16, 60).BottomPosZ(12, 20));

		btnWS.WhenAction << [&]{ String S=SelectDirectory(); if (!S.IsEmpty()) ebW.SetData(S); };
		btnOK.WhenAction << [&]{ bok=true; Close(); };
		btnCancel.WhenAction << [&]{ bok=false; Close(); };

	}
	virtual ~DlgProject(){}
	
};

struct ModStatus : public StatusBar
{
	using CLASSNAME=ModStatus;
	
	Image reddot;
	Image greendot;
	virtual ~ModStatus(){}
	ModStatus() { SetFrame(ThinInsetFrame()); reddot=GetPic(PICDBBUSY); greendot=GetPic(PICDBREADY); }
	void ShowDBStatus(bool b=false)
	{
		Set(0, ((b)?reddot:greendot), 30); Refresh();
		Set(1, ((b)?"database busy":"Ready"), 120); Refresh();
	}
	void ShowText(const std::string s="") { Set(2, s.c_str(), 500); Refresh(); }
	void ClearText() { ShowText(); }

};

struct Modeler : public TopWindow
{
	typedef Modeler CLASSNAME;
	
	ModStatus MSB;
	Label lblRD;
	DropList cbProject;
	Button btnA;
	Button btnE;
	Button btnD;
	Panel panProject;
	TabCtrl Tabs;
	
	ProjectList projectlist;
	Project *pcurProject;
	
	//pinned tabs
	Journal journal;
	Library library;
	Deferred deferred;

	//per Project
	WordList *pwordlist;

	//Project-tabs
	/*
		* definitions:
			elem :- situation(state,rule,role), event(random or planned happening/occurrence), action(task,function), thing(), set(collection of elems)
			data :- raw elements
			info :- element relationships (defined in terms of other elements)
		* probing - determine elements and list & detail the elements (data collection)
		* scoping - determine goals and order & associate elements to achieve the goals
	*/
	// [SCOPING] determine scope of system..
		Prober *pprober; //relevant data from: source(problem) and dest(solution) env's
		Collector *pcollector; //n&s-element-definitions ERROR: this is part of prober, elems must have disposition-field(used/dropped/phase/later/...)
		Scoper *pscoper; //conceptualized system
	
	// [PLANNING] 
		// n&s info to xlate sources into dests using scope
	Analyzer *panalyzer; //
	Designer *pdesigner;
	Producer *pproducer;
	
	Modeler();
	~Modeler();

	WaitCursor *pWaitCur;
	void OnDBBusy(bool b);
	
	void AddPinnedTabs();
	void ClosePinnedTabs();
	
	void FillProject();
	void change_project();
	
	bool edit_project(std::string &sn, std::string &sw, std::string &sd, bool bnew=true);
	
	void AddProject(); //+
	void EditProject(); //..
	void DeleteProject(); //-

	void update_modules();
	
	template<typename R, typename D> bool pin_tab(const std::string &sr, const std::string &sd, R &r, D &d)
	{
		std::string DBPath=path_append(ModConf.HoM_PATH, sd);
		bool bok, bX=file_exist(DBPath);
		if ((bok=d.Open(DBPath))) { if (!bX) { bok=d.ImplementSchema(); }}
		if (!bok) { tellerror("Database: '", DBPath, "' - failed: ", d.GetLastError()); }
		Tabs.Add(r.SetFrame(ThinInsetFrame()).HSizePosZ().VSizePosZ(), sr.c_str());
		if (bok) d.WhenDBBusy = [&](bool b){ OnDBBusy(b); };
		if (bok) bok=r.Initialize(this);
		return bok;
	}

	template<typename D> bool prepare_db(D &db, const std::string &sdb)
	{
		std::string DBPath=path_append(pcurProject->workspace, "modeler");
		if (!path_realize(DBPath)) { tellerror("Cannot access workdir: '", DBPath, "'"); return false; }
		DBPath=path_append(DBPath, sdb);
		bool bok, bX=file_exist(DBPath);
		if ((bok=db.Open(DBPath))) { if (!bX) { bok=db.ImplementSchema(); }}
		if (!bok) { tellerror("Database: '", DBPath, "' - failed: ", db.GetLastError()); }
		if (bok) db.WhenDBBusy = [&](bool b){ OnDBBusy(b); }; //for statusbar
		return bok;
	}
	
	template<typename P, typename D> bool add_tab(const std::string &pname, const std::string &dname, P **p, D &d)
	{
		if ((*p)) remove_tab(p, d);
		if (!(*p=new P)) return tellerror("Memory setup for ", pname, " (", dname, ") failed");;
		if (!prepare_db(d, dname)) { delete *p; return tellerror("Data setup for ", pname, " (", dname, ") failed"); }
		Tabs.Add((**p).SetFrame(ThinInsetFrame()).HSizePosZ().VSizePosZ(), pname.c_str());
		return true;
	}

	template<typename P, typename D> void remove_tab(P **p, D &d)
	{
		if (p)
		{
			(*p)->Terminate();
			int t=Tabs.Find((**p));
			if (t>=0) Tabs.Remove(t);
			delete (*p);
			*p=nullptr;
		}
		d.Close();
	}
		
	void setup_projectwordlist();
	void close_projectwordlist();

	void OpenProject(); //set tabs
	void CloseProject(); //remove'm

	bool IsDropWord(const std::string &w); //access to wordlist from sub-modules
	void AddDropWord(const std::string &w);
	void RemoveDropWord(const std::string &w);
	
};




#endif
