
#include "prober.h"
#include "dbprober.h"
#include <utilfuncs/utilfuncs.h>
#include "globdefs.h"
#include "modeler.h"


//-------------------------------------------------------------------------------------------------SItem
void SItem::clear() { iditem=0; name.clear(); def.clear(); }

SItem::~SItem()	{ clear(); }
SItem::SItem()	{ clear(); }

SItem::SItem(const std::string &sname, const std::string &sdef)
{
	clear();
	name=sname;
	def=sdef;
}

SItem::SItem(const SItem &I) { *this=I; }

SItem& SItem::operator=(const SItem &I)
{
	iditem=I.iditem;
	name=I.name;
	def=I.def;
	return *this;
}

bool SItem::Save() { return tellifdberror(dbprober.Save(*this), &dbprober); }
bool SItem::Delete() { return tellifdberror(dbprober.Delete(*this), &dbprober); }

//-------------------------------------------------------------------------------------------------SItems
void SItems::clear()	{ list.clear(); }

void SItems::push(const SItem &I) { if (I.iditem>0)	list[I.iditem]=I; }
	
SItems::~SItems()	{ clear(); }
SItems::SItems()	{ clear(); }

SItems::iterator SItems::begin()				{ return list.begin(); }
SItems::iterator SItems::end()					{ return list.end(); }
SItems::const_iterator SItems::begin() const	{ return list.begin(); }
SItems::const_iterator SItems::end() const		{ return list.end(); }

bool SItems::empty() { return list.empty(); }
	
bool SItems::Load()	{ return tellifdberror(dbprober.Load(*this), &dbprober); }

SItem* SItems::Add(const SItem &I)
{
	std::string s=I.name; TRIM(s); if (s.empty()) return nullptr;
	SItem II(I);
	II.iditem=0;
	if (II.Save()) { list[II.iditem]=II; return &(list[II.iditem]); }
	return nullptr;
}

SItem* SItems::Add(const std::string &sname, const std::string &snote)
{
	SItem I(sname, snote);
	return (Add(I));
}

bool SItems::Delete(int itemid)
{
	SItem *p=Get(itemid);
	if (p) { list.erase(itemid); return p->Delete(); }
	return false;
}

SItem* SItems::Get(int itemid)
{
	if (list.find(itemid)!=list.end()) return &(list[itemid]);
	return nullptr;
}

//-------------------------------------------------------------------------------------------------edit item dialog
DlgItem::~DlgItem(){}
DlgItem::DlgItem(bool bshared)
{
	bok=false;
	id=0; // caller must preset this if editing a shared item
	oid=0;
	Sa.Clear();
	So.Clear();
	bs=bshared;

	CenterOwner();
	Title("Edit Item");
	Sizeable();
	SetRect(0,0,400,300);
	Add(lblName.SetLabel(t_("Name:")).LeftPosZ(4, 40).TopPosZ(4, 19));
	Add(ebName.LeftPosZ(48, 220).TopPosZ(4, 19));
	Add(btnSel.SetLabel(t_("..")).SetFont(StdFontZ(11).Bold()).LeftPosZ(272, 16).TopPosZ(4, 19));
	//Add(btnX.SetLabel(t_("X")).LeftPosZ(292, 16).TopPosZ(4, 19));
	//btnX.Enable(false);
	Add(lblNote.SetLabel(t_("Note:")).LeftPosZ(4, 40).TopPosZ(28, 19));
	//Add(lblShared.SetLabel(t_("(Shared Item!)")).SetAlign(ALIGN_CENTER).SetFont(StdFont().Bold()).LeftPosZ(52, 90).TopPosZ(28, 19));
	Add(ebNote.HSizePosZ(4, 4).VSizePosZ(48, 52));
	Add(btnOK.SetLabel(t_("OK")).RightPosZ(80, 60).BottomPosZ(16, 20));
	Add(btnCancel.SetLabel(t_("Cancel")).RightPosZ(12, 60).BottomPosZ(16, 20));
	//lblShared.Show(bshared);
	btnOK.WhenAction << [&]
	{
		std::string s=ebName.GetData().ToString().ToStd();TRIM(s);
		if (!s.empty()) { bok=true; Close(); } //caller must check id: !0=>shared (then get, update & save id)
		else telluser("Need a name");
	};
	btnCancel.WhenAction << [&]{ bok=false; Close(); };
	btnSel.WhenAction << [&]
	{
		if (oid==0) oid=id;
		if ((id==0)&&Sa.IsEmpty()&&So.IsEmpty())
		{
			String Sa=ebName.GetData().ToString();
			String So=ebNote.GetData().ToString();
		}
		DlgSelItem dlg;
		dlg.Execute();
		if (dlg.ret>0) //1=copy, 2=share
		{
			SItem I=dlg.GetSelItem();
			ebName.SetData(I.name.c_str());
			ebNote.SetData(I.def.c_str());
			//if (dlg.ret==2) { id=I.iditem; lblShared.Show((dlg.ret==2)); }
			//btnX.Enable(true);
		}
	};
	//btnX.WhenAction << [&]{ id=oid; ebName.SetData(Sa); ebNote.SetData(So); btnX.Enable(false); lblShared.Show(bs); };

}

//-------------------------------------------------------------------------------------------------select item dialog
DlgSelItem::~DlgSelItem(){}
DlgSelItem::DlgSelItem(int xcludeid)
{
	ret=0;
	xid=xcludeid;
	
	Title("Select Item");
	Sizeable();
	SetRect(0,0,400,400);
	
	Add(arItems.HSizePosZ(0, 0).VSizePosZ(0, 52));
	//Add(btnShare.SetLabel(t_("Share")).RightPosZ(152, 60).BottomPosZ(16, 20));
	//Add(btnCopy.SetLabel(t_("Copy")).RightPosZ(84, 60).BottomPosZ(16, 20));
	Add(btnUse.SetLabel(t_("Use copy")).RightPosZ(84, 60).BottomPosZ(16, 20));
	Add(btnCancel.SetLabel(t_("Cancel")).RightPosZ(16, 60).BottomPosZ(16, 20));

	//btnShare.Enable(false);
	//btnCopy.Enable(false);
	btnUse.Enable(false);
	
	arItems.Reset();
	arItems.AddColumn(); //idcol
	arItems.AddColumn("Name", 50);
	arItems.AddColumn("Note", 50);
	
	arItems.WhenAction << [&]{ int c=arItems.GetCursor(); btnUse.Enable(c>=0); }; //btnShare.Enable(c>=0); btnCopy.Enable(c>=0); };
	//btnShare.WhenAction << [&]{ ret=2; Close(); };
	//btnCopy.WhenAction << [&]{ ret=1; Close(); };
	btnUse.WhenAction << [&]{ ret=1; Close(); };
	btnCancel.WhenAction << [&]{ ret=0; Close(); };

	fillar();
	
}

void DlgSelItem::fillar()
{
	SItems L;
	L.Load();
	for (auto i:L)
	{
		if (i.first!=xid)
		{
			arItems.Add(i.first, i.second.name.c_str(), i.second.def.c_str());
		}
	}
}

SItem DlgSelItem::GetSelItem()
{
	SItem I;
	int c=arItems.GetCursor();
	if (c>=0) { I.iditem=arItems.Get(c,0).To<int>(); I.name=arItems.Get(c,1).ToString().ToStd();  I.def=arItems.Get(c,2).ToString().ToStd(); }
	return I;
}

//-------------------------------------------------------------------------------------------------Seed
void Seed::clear() { idseed=idpage=iditem=x=y=disp=0; }

Seed::~Seed() {}
Seed::Seed() { clear(); }
Seed::Seed(const Seed &S) { *this=S; }

Seed& Seed::operator=(const Seed &S)
{
	idseed=S.idseed;
	idpage=S.idpage;
	iditem=S.iditem;
	x=S.x;
	y=S.y;
	disp=S.disp;
	return *this;
}

bool Seed::Save()
{
	if ((idpage==0)||(iditem==0)) return false;
	return tellifdberror(dbprober.Save(*this), &dbprober);
}

bool Seed::Delete() { return tellifdberror(dbprober.Delete(*this), &dbprober); }

//-------------------------------------------------------------------------------------------------Seeds
void Seeds::clear()	{ seeds.clear(); }

void Seeds::push(const Seed &S) { if (S.idseed>0)	seeds[S.idseed]=S; }
	
Seeds::~Seeds()	{ clear(); }
Seeds::Seeds() { clear(); }
Seeds::Seeds(const Seeds &S) { *this=S; }

Seeds& Seeds::operator=(const Seeds &S) { for (auto p:S) seeds[p.first]=p.second; return *this; }

Seeds::iterator Seeds::begin()				{ return seeds.begin(); }
Seeds::iterator Seeds::end()				{ return seeds.end(); }
Seeds::const_iterator Seeds::begin() const	{ return seeds.begin(); }
Seeds::const_iterator Seeds::end() const	{ return seeds.end(); }

bool Seeds::empty() { return seeds.empty(); }
	
bool Seeds::Load(int pageid)	{ return tellifdberror(dbprober.Load(*this, pageid), &dbprober); }

Seed* Seeds::Add(const Seed &S)
{
	if ((S.idpage==0)||(S.iditem==0)) return nullptr;
	Seed SS(S);
	SS.idseed=0;
	if (SS.Save()) { seeds[SS.idseed]=SS; return &(seeds[SS.idseed]); }
	return nullptr;
}

bool Seeds::Delete(int seedid)
{
	Seed *p=Get(seedid);
	if (p)
	{
		p->Delete();
		seeds.erase(seedid);
		return true;
	}
	return false;
}

Seed* Seeds::Get(int seedid)
{
	if (seeds.find(seedid)!=seeds.end()) return &(seeds[seedid]);
	return nullptr;
}

//-------------------------------------------------------------------------------------------------Assoc
void Assoc::clear() { idassoc=idpage=iditem=fidseed=tidseed=0; }
Assoc::~Assoc() {}
Assoc::Assoc() { clear(); }
Assoc::Assoc(const Assoc &A) { *this=A; }

Assoc& Assoc::operator=(const Assoc &A)
{
	idassoc=A.idassoc;
	idpage=A.idpage;
	iditem=A.iditem;
	fidseed=A.fidseed;
	tidseed=A.tidseed;
	return *this;
}

bool Assoc::Save()
{
	if ((idpage==0)||(iditem==0)) return false;
	return tellifdberror(dbprober.Save(*this), &dbprober);
}

bool Assoc::Delete() { return tellifdberror(dbprober.Delete(*this), &dbprober); }

//-------------------------------------------------------------------------------------------------Assocs
void Assocs::clear() { assocs.clear(); }

void Assocs::push(const Assoc &A) { if (A.idassoc>0) assocs[A.idassoc]=A; }

Assocs::~Assocs() {}
Assocs::Assocs() { clear(); }
Assocs::Assocs(const Assocs &A) { *this=A; }

Assocs& Assocs::operator=(const Assocs &A) { for (auto p:A) assocs[p.first]=p.second; return *this; }

Assocs::iterator Assocs::begin()				{ return assocs.begin(); }
Assocs::iterator Assocs::end()					{ return assocs.end(); }
Assocs::const_iterator Assocs::begin() const	{ return assocs.begin(); }
Assocs::const_iterator Assocs::end() const		{ return assocs.end(); }

bool Assocs::empty() { return assocs.empty(); }

bool Assocs::Load(int pageid) { return tellifdberror(dbprober.Load(*this, pageid), &dbprober); }

Assoc* Assocs::Add(const Assoc &A)
{
	if ((A.idpage==0)||(A.fidseed==0)||(A.tidseed==0)) return nullptr;
	Assoc aa(A);
	aa.idassoc=0;
	if (aa.Save()) { assocs[aa.idassoc]=aa; return &(assocs[aa.idassoc]); }
	return nullptr;
}

bool Assocs::Delete(int associd)
{
	Assoc *p=Get(associd);
	if (p)
	{
		p->Delete();
		assocs.erase(associd);
		return true;
	}
	return false;
}
	
Assoc* Assocs::Get(int associd)
{
	if (assocs.find(associd)!=assocs.end()) return &(assocs[associd]);
	return nullptr;
}

//-------------------------------------------------------------------------------------------------Tag
void Tag::clear() { idtag=idpage=0; tagdata.clear(); }
Tag::~Tag() {}
Tag::Tag() { clear(); }
Tag::Tag(const Tag &T) { *this=T; }

Tag& Tag::operator=(const Tag &T)
{
	idtag=T.idtag;
	idpage=T.idpage;
	tagdata=T.tagdata;
	return *this;
}

bool Tag::Save()
{
	if (idpage==0) return false;
	return tellifdberror(dbprober.Save(*this), &dbprober);
}

bool Tag::Delete() { return tellifdberror(dbprober.Delete(*this), &dbprober); }

bool Tag::UpdateData(const std::string &tdata)
{
	tagdata=tdata;
	return tellifdberror(dbprober.Save(*this), &dbprober);
}


//-------------------------------------------------------------------------------------------------Tags
void Tags::clear() { tags.clear(); }

void Tags::push(const Tag &T) { if (T.idtag>0) tags[T.idtag]=T; }

Tags::~Tags() {}
Tags::Tags() { clear(); }
Tags::Tags(const Tags &T) { *this=T; }

Tags& Tags::operator=(const Tags &T) { for (auto p:T) tags[p.first]=p.second; return *this; }

Tags::iterator Tags::begin()				{ return tags.begin(); }
Tags::iterator Tags::end()					{ return tags.end(); }
Tags::const_iterator Tags::begin() const	{ return tags.begin(); }
Tags::const_iterator Tags::end() const		{ return tags.end(); }

bool Tags::empty() { return tags.empty(); }

bool Tags::Load(int pageid) { return tellifdberror(dbprober.Load(*this, pageid), &dbprober); }

Tag* Tags::Add(const Tag &T)
{
	if (T.idpage==0) { tellerror("tag has no page"); return nullptr; }
	Tag t(T);
	t.idtag=0;
	if (t.Save()) { tags[t.idtag]=t; return &(tags[t.idtag]); }
	return nullptr;
}

bool Tags::Delete(int tagid)
{
	Tag *p=Get(tagid);
	if (p) { p->Delete(); tags.erase(tagid); return true; } //?if p->delete fails?
	return false;
}
	
Tag* Tags::Get(int tagid)
{
	if (tags.find(tagid)!=tags.end()) return &(tags[tagid]);
	return nullptr;
}

//-------------------------------------------------------------------------------------------------SPage
void SPage::clear()
{
	idpage=0;
	X=Y=0;
	paperAn=4;
	pagePL=LANDSCAPE;
	seeds.clear();
	assocs.clear();
	tags.clear();
	
	if (!IsEmpty()) tellerror("(clear) page is not empty!");
//	seedtopics.clear();
//	assoclinks.clear();
//	tagshapes.clear();
}

SPage::~SPage() { clear(); }
SPage::SPage() { clear(); }
SPage::SPage(const SPage &P) { clear(); *this=P; }
	
SPage& SPage::operator=(const SPage &P)
{
	idpage=P.idpage;
	X=P.X; Y=P.Y;
	paperAn=P.paperAn;
	pagePL=P.pagePL;
	seeds=P.seeds;
	assocs=P.assocs;
	tags=P.tags;
	
	if (!IsEmpty()) tellerror("(=) page is not empty!");
	
	return *this;
}
	
bool SPage::Save() { return tellifdberror(dbprober.Save(*this), &dbprober); }

bool SPage::IsEmpty() { return (seedtopics.empty()&&assoclinks.empty()&&tagshapes.empty()); }

bool SPage::Delete()
{
	if (!IsEmpty()) return tellerror("cannot delete page - topics, links or shapes exist");
	seeds.clear();
	assocs.clear();
	tags.clear();
	return tellifdberror(dbprober.Delete(*this), &dbprober);
}

void SPage::restore_content()
{
	seeds.Load(idpage);
	assocs.Load(idpage);
	tags.Load(idpage);
}

Seed* SPage::AddSeed(const Seed &S)
{
	if (S.iditem==0) return nullptr;
	Seed ss(S);
	ss.idpage=idpage;
	ss.idseed=0;
	return seeds.Add(ss);
}

Seed* SPage::AddSeed(int itemid, int x, int y, int disp)
{
	Seed S;
	S.iditem=itemid;
	S.x=x;
	S.y=y;
	S.disp=disp;
	return AddSeed(S);
}

void SPage::RemoveSeed(Seed *pS)
{
	if (GetTopic(pS->idseed)) { tellerror("cannot remove seed - topic exist"); return; }
	seeds.Delete(pS->idseed);
}

Seed* SPage::GetSeed(int seedid) { return seeds.Get(seedid); }

Seed* SPage::GetSeed(TopicWidget *pT)
{
	for (auto p:seedtopics) { if (p.second==pT) return seeds.Get(p.first); }
	return nullptr;
}

TopicWidget* SPage::GetTopic(int seedid)
{
	if (seedtopics.find(seedid)!=seedtopics.end()) return seedtopics[seedid];
	return nullptr;
}
	
Assoc* SPage::AddAssoc(const Assoc &A)
{
	if (A.iditem==0) { tellerror("assoc has no item"); return nullptr; } //todo: write to logfile
	Assoc aa(A);
	aa.idpage=idpage;
	aa.idassoc=0;
	return assocs.Add(aa);
}

Assoc* SPage::AddAssoc(int fseedid, int tseedid, int itemid)
{
	Assoc A;
	A.iditem=itemid;
	A.fidseed=fseedid;
	A.tidseed=tseedid;
	return AddAssoc(A);
}

void SPage::RemoveAssoc(Assoc *pA)
{
	if (GetLink(pA->idassoc)) tellerror("cannot remove assoc - links exist"); //todo: write to logfile
	else assocs.Delete(pA->idassoc);
}

Assoc* SPage::GetAssoc(int associd) { return assocs.Get(associd); }

Assoc* SPage::GetAssoc(LinkWidget *pL)
{
	for (auto p:assoclinks) { if (p.second==pL) return assocs.Get(p.first); }
	return nullptr;
}

LinkWidget* SPage::GetLink(int associd)
{
	if (assoclinks.find(associd)!=assoclinks.end()) return assoclinks[associd];
	return nullptr;
}

bool SPage::AddTopic(Seed *pS, TopicWidget *pT)
{
	if (!(pS&&(pS->idseed)&&pT)) return tellerror("cannot add topic: seed=", (pS?"ok":"fail"), "), idseed=(", (pS->idseed?"ok":"fail"), "), topic=(", (pT?"ok)":"fail)"));
	seedtopics[pS->idseed]=pT;
	return true;
}

void SPage::DropTopic(TopicWidget *pT)
{
	int seedid=0;
	for (auto p:seedtopics) { if (p.second==pT) { seedid=p.first; break; } }
	if (seedid) seedtopics.erase(seedid);
}

bool SPage::AddLink(Assoc *pA, LinkWidget *pL)
{
	assoclinks[pA->idassoc]=pL;
	return true;
}

void SPage::DropLink(LinkWidget *pL)
{
	int associd=0;
	for (auto p:assoclinks) { if (p.second==pL) { associd=p.first; break; } }
	if (associd) assoclinks.erase(associd);
}

bool SPage::has_link(int fid, int tid)
{
	for (auto p:assocs)
	{
		if ((p.second.fidseed==fid)&&(p.second.tidseed==tid)) { return true; }
	}
	return false;
}

bool SPage::GetItemLinks(int itemid, std::vector<LinkWidget*> &v)
{
	v.clear();
	for (auto p:assocs) { if (p.second.iditem==itemid) v.push_back(GetLink(p.first)); }
	return (v.size()>0);
}

bool SPage::GetSeedLinks(Seed *pS, std::vector<LinkWidget*> &v)
{
	v.clear();
	for (auto p:assocs)
	{
		if ((p.second.fidseed==pS->idseed)||(p.second.tidseed==pS->idseed))
		{
			LinkWidget *pl=GetLink(p.first);
			if (pl) v.push_back(pl);
		}
	}
	return (v.size()>0);
}

bool SPage::GetItemTopics(int itemid, std::vector<TopicWidget*> &v)
{
	v.clear();
	for (auto p:seeds) { if (p.second.iditem==itemid) v.push_back(GetTopic(p.first)); }
	return (v.size()>0);
}

Tag* SPage::AddTag(const Tag &T)
{
	Tag t(T);
	t.idpage=idpage;
	t.idtag=0;
	return tags.Add(t);
}

Tag* SPage::AddTag(const std::string &tdata)
{
	Tag T;
	T.tagdata=tdata;
	return AddTag(T);
}

void SPage::RemoveTag(Tag *pT)
{
	if (GetShape(pT->idtag)) { tellerror("cannot remove tag - shapes exist"); return; }
	tags.Delete(pT->idtag);
}

Tag* SPage::GetTag(int tagid) { return tags.Get(tagid); }

Tag* SPage::GetTag(Shape *pS)
{
	for (auto p:tagshapes) { if (p.second==pS) return tags.Get(p.first); }
	return nullptr;
}

Shape* SPage::GetShape(int tagid)
{
	if (tagshapes.find(tagid)!=tagshapes.end()) return tagshapes[tagid];
	return nullptr;
}

bool SPage::AddShape(int tagid, Shape *pS)
{
	tagshapes[tagid]=pS;
	return true;
}

void SPage::DropShape(Shape *pS)
{
	int tagid=0;
	for (auto p:tagshapes) { if (p.second==pS) { tagid=p.first; break; } }
	if (tagid) tagshapes.erase(tagid);
}

//-------------------------------------------------------------------------------------------------SPages
void SPages::clear() { for (auto& p:pages) p.second.clear(); pages.clear(); }

void SPages::push(const SPage &P) { if (P.idpage>0) pages[P.idpage]=P; }
	
SPages::~SPages() {}
SPages::SPages() { clear(); }
SPages::SPages(const SPages &PP) { *this=PP; }

SPages& SPages::operator=(const SPages &PP) { pages=PP.pages; return *this; }
	
SPages::iterator SPages::begin() { return pages.begin(); }
SPages::iterator SPages::end() { return pages.end(); }
SPages::const_iterator SPages::begin() const { return pages.begin(); }
SPages::const_iterator SPages::end() const { return pages.end(); }

bool SPages::empty() { return pages.empty(); }

bool SPages::Load()
{
	if (tellifdberror(dbprober.Load(*this), &dbprober))
	{
		for (auto& p:pages) { p.second.restore_content(); }
	}
	else return false;
	return true;
}
	
SPage* SPages::NewPage()
{
	SPage P;
	if (P.Save()) { push(P); return &(pages[P.idpage]); }
	return nullptr;
}

bool SPages::DeletePage(SPage *pP)
{
	if (pP) { pages.erase(pP->idpage); return pP->Delete(); }
	return false;
}

SPage* SPages::GetPage(int pageid)
{
	if (pages.find(pageid)!=pages.end()) return &(pages[pageid]);
	return nullptr;
}

//-------------------------------------------------------------------------------------------------(get_ppmm())
void get_ppmm(double &dx, double &dy)
{
	struct W : public TopWindow
	{
		double x,y;
		W() { SetRect(0,0,10,10); }
		void Paint(Draw &drw) { Size spi=drw.GetPixelsPerInch(); x=(double)spi.cx/25.4; y=(double)spi.cy/25.4; }
	};
	W w; w.Open();
	dx=w.x; dy=w.y;
}

//-------------------------------------------------------------------------------------------------Prober
void Prober::clear()
{
	pages.clear();
	pPage=nullptr;
}

void Prober::ctor_init()
{
	clear();
	sitems.clear();
	pModeler=nullptr;
}

Prober::Prober()
{
	ctor_init();

	get_ppmm(ppmmX, ppmmY);

	panTools.Add(lblPage.SetLabel(t_("Page:")).LeftPosZ(2,30).TopPosZ(2, 20));
	cbPages.WhenAction << [&]{ DoCBPages(); };
	panTools.Add(cbPages.LeftPosZ(34, 180).TopPosZ(2, 20));
	panTools.Add(lblP.SetLabel(t_("Paper: A")).LeftPosZ(216, 44).TopPosZ(2, 20));
	for (int i=0;i<5; i++) { cbP.Add(i); } cbP.SetData(4);
	cbP.WhenAction << [&]{ int x=cbP.Get().To<int>(); SetPaperSize(x); };
	panTools.Add(cbP.LeftPosZ(262, 28).TopPosZ(2, 20));
	radLP=LANDSCAPE;
	radLP.WhenAction << [&]{ int x=~radLP; SetPaperOrientation(x); };
	panTools.Add(radLP.SetLabel(t_("Landscape\nPortrait")).LeftPosZ(292, 140).TopPosZ(2, 20));
	Add(panTools.SetFrame(ThinInsetFrame()).LeftPosZ(0,434).TopPosZ(0, 30));

	HS.SetLine(ppmmX*5); //5 millimeter
	VS.SetLine(ppmmY*5);
	panMap.AddFrame(HS);
	panMap.AddFrame(VS);
	
	map.SetMargins((5*ppmmX),(5*ppmmY),(5*ppmmX),(5*ppmmY));
	panMap.Add(map);
	Add(panMap.HSizePosZ().VSizePosZ(31));

	map.WhenIsDrawingChange << [&](bool b){ OnIsDrawingChange(b); }; ///???
	
	map.WhenNewTopic << [&](TopicWidget *pT)->bool{ return DoNewTopic(pT); };
	map.WhenNewLink << [&](LinkWidget *pL)->bool{ return DoNewLink(pL); };
	map.WhenNewShape << [&](Shape *pS)->bool{ return DoNewShape(pS); };
	
	map.WhenShapeChanged << [&](Shape *pS){ OnShapeChanged(pS); };
	map.WhenShapeDeleted << [&](Shape *pS){ OnShapeDeleted(pS); };

	map.WhenTopicRightClick = THISBACK(OnTopicRightClick);
	map.WhenTopicLeftDoubleClick = THISBACK(OnTopicLeftDoubleClick);
	map.WhenTopicMoved = THISBACK(OnTopicMoved);
	map.WhenLinkRightClick = THISBACK(OnLinkRightClick);
	map.WhenLinkLeftDoubleClick = THISBACK(OnLinkLeftDoubleClick);
	map.WhenMapRightClick = THISBACK(OnMapRightClick);
	//map.WhenMapLeftDoubleClick = THISBACK(OnMapLeftDoubleClick);
	//map.WhenTopicLinkTopic = THISBACK(OnTopicLinkTopic);
	//map.WhenTopicLinkNewTopic = THISBACK(OnTopicLinkNewTopic);

	map.WhenShapeRightClick = THISBACK(OnShapeRightClick);
	
	map.WhenMultiSelect = THISBACK(OnMultiSelect);

	//this stays!
	Add(map.toolpanel.AddFrame(ThinInsetFrame()).HSizePosZ(436).TopPosZ(0,30));
	map.toolpanel.SetWantFocus();
	
	HS.WhenScroll = THISBACK(DoHScroll);
	VS.WhenScroll = THISBACK(DoVScroll);

}

Prober::~Prober()
{
	if (pPage) close_page();
	clear();
}

bool Prober::Initialize(Modeler *pMod)
{
	ctor_init();
	pModeler=pMod;
	dbprober.Load(sitems);
	pPage=nullptr;
	pages.Load();
	if (pages.empty()) { if (!make_page_1()) { tellerror("Prober: initial page failed"); return false; }}
	FillCBPages();
	OpenPage(1);
	return true;
}

void Prober::Terminate()
{
	namepages.clear();
	if (pPage) close_page();
	pages.clear();
}

bool Prober::make_page_1()
{
	SPage *pP=pages.NewPage();
	if (pP)
	{
		SItem *pI1=sitems.Add(SItem(pModeler->pcurProject->name, pModeler->pcurProject->description));
		Seed *pS1=pP->AddSeed(pI1->iditem,50,50,(-1));
		SItem *pI2=sitems.Add(SItem("Detail", "Details about the Project"));
		Seed *pS2=pP->AddSeed(pI2->iditem,300,100,0);
		SItem *pI3=sitems.Add(SItem("info", "associating one seed with another"));
		Assoc *pA=pP->AddAssoc(pS1->idseed, pS2->idseed, pI3->iditem);
		if (pA) return true; //{  pPage=pP; return true; }
		else pages.DeletePage(pP);
		return tellerror("Prober: initial seeding failed");
	}
	else return tellerror("Prober: initialization failed");
}

void Prober::FillCBPages()
{
	cbPages.Clear();
	namepages.clear();
	namepages[sitems.Get(1)->name]=1;
	for (auto& p:pages)
	{
		for (auto& pp:p.second.seeds)
		{
			if (pp.second.disp>0) namepages[sitems.Get(pp.second.iditem)->name]=pp.second.disp;
		}
	}
	for (auto& p:namepages) cbPages.Add(p.first.c_str());
}

void Prober::ShowCurCBPage(int pageid)
{
	std::string s{};
	for (auto p:namepages)
	{
		if (p.second==pageid)
		{
			s=p.first;
			break;
		}
	}
	if (!s.empty()) cbPages.SetData(s.c_str());
}

void Prober::DoCBPages()
{
	std::string s=cbPages.Get().ToString().ToStd();
	if (!s.empty())
	{
		int pid=namepages[s];
		OpenPage(pid);
	}
}


void Prober::update_main_topic(const std::string &newname, const std::string &desc)
{
	//name-change update, called from modeler
	SItem *pI=sitems.Get(pages.GetPage(1)->GetSeed(1)->iditem);
	pI->name=newname;
	pI->def=desc;
	pI->Save();
	if (pPage&&(pPage->idpage==1)) OpenPage(1);
}

void Prober::SetPageTools()
{
	cbP.SetData(pPage->paperAn);
	radLP=pPage->pagePL;
	set_paper_page();
	HS.Set(pPage->X);
	VS.Set(pPage->Y);
	map.ResetToolpanel(false);
}

void Prober::OpenPage(int pageid) //creates/fills topics/links/tags
{
	if (pPage) close_page();
	pPage=pages.GetPage(pageid);
	if (pPage)
	{
		SetPageTools();
		for (auto& p:pPage->seeds)
		{
			Seed *pS=&(p.second);
			SItem *pI=sitems.Get(pS->iditem);
			if (pI)
			{
				if (pPage->AddTopic(pS, map.MakeTopic(pS->x, pS->y, pI->name, pS->disp)))
				{
					TopicWidget* pT=pPage->GetTopic(pS->idseed);
					std::string stip=pI->def;
					if (stip.empty()) stip="(no description)";
					else if (stip.size()>512) stip=spf(stip.substr(0,500), "\n(..more..)");
					pT->SetToolTip(stip);
				}
			}
		}
		for (auto& p:pPage->assocs)
		{
			SItem *pI=sitems.Get(p.second.iditem);
			TopicWidget *f=pPage->GetTopic(p.second.fidseed);
			TopicWidget *t=pPage->GetTopic(p.second.tidseed);
			if (f&&t)
			{
				if (pPage->AddLink(&(p.second), map.MakeLink(f, t, ((pI)?pI->name:"-?-"))))
				{
					//LinkWidget *pL=pPage->GetLink(p.second.idassoc);
					//pL->SetToolTip(pI->def);
				}
			}
		}
		for (auto& p:pPage->tags) { pPage->AddShape(p.first, map.MakeShape(p.second.tagdata)); }
		ShowCurCBPage(pageid);
	}
	map.Refresh();
}

void Prober::close_page()
{
	if (pPage)
	{
		while (pPage->seedtopics.begin()!=pPage->seedtopics.end())
		{
			map.DeleteTopic(pPage->seedtopics.begin()->second);
			pPage->DropTopic(pPage->seedtopics.begin()->second);
		}
		while (pPage->assoclinks.begin()!=pPage->assoclinks.end())
		{
			map.DeleteLink(pPage->assoclinks.begin()->second);
			pPage->DropLink(pPage->assoclinks.begin()->second);
		}
		while (pPage->tagshapes.begin()!=pPage->tagshapes.end())
		{
			map.DeleteShape(pPage->tagshapes.begin()->second);
			pPage->DropShape(pPage->tagshapes.begin()->second);
		}
		cbPages.SetData("");
		pPage=nullptr;
		map.Refresh();
	}
}

bool Prober::DeleteSeedPage(Seed *pS)
{
	if (pS->disp==1) return tellerror("cannot delete first page (1)");
	else if (has_content(pS->disp)) return tellerror("cannot delete page with content");
	pages.DeletePage(pages.GetPage(pS->disp));
	pS->disp=0;
	FillCBPages(); //cleanup cb
	OpenPage(pPage->idpage);
	return true;
}

bool Prober::has_content(int page)
{
	SPage *P=pages.GetPage(page);
	if (P) return !(P->assocs.empty() && P->tags.empty() && (P->seeds.size()==1)); //primary seed only
	return false;
}

void Prober::set_page_x(int x) { if (pPage) { pPage->X=x; }}
void Prober::set_page_y(int y) { if (pPage) { pPage->Y=y; }}

void Prober::Paint(Draw &drw)
{
	Size szmap=map.GetSize();
	map.SetRect(pPage->X, pPage->Y, szmap.cx, szmap.cy);
}

void Prober::Layout()
{
	Size sz=panMap.GetSize();
	VS.SetPage(sz.cy);
	HS.SetPage(sz.cx);
}

void Prober::DoHScroll() { set_page_x(-HS); Refresh(); }
void Prober::DoVScroll() { set_page_y(-VS); Refresh(); }
void Prober::MouseWheel(Point p, int z, dword d) { VS.Wheel(z); }

void Prober::set_paper_page()
{
	int w;
	int h;
	if (pPage->pagePL==LANDSCAPE) GetPaperSize(pPage->paperAn, h, w);
	else GetPaperSize(pPage->paperAn, w, h);
	w=(int)(double(w)*ppmmX); h=(int)(double(h)*ppmmY);
	papersize.cx=w; papersize.cy=h;
	map.SetRect(0,0,w,h);
	HS.SetTotal(w);
	VS.SetTotal(h);
	Refresh();
}

void Prober::SetPaperSize(int A)
{
	static bool bbusy=false;
	if (bbusy) return;
	if ((A>=0)&&(A<=4)&&!(pPage->paperAn==A))
	{
		int PAGEID=pPage->idpage;
		bbusy=true;
		int cw, ch, nw, nh;
		if (pPage->pagePL==LANDSCAPE) GetPaperSize(pPage->paperAn, ch, cw);
		else GetPaperSize(pPage->paperAn, cw, ch);
		if (pPage->pagePL==LANDSCAPE) GetPaperSize(A, nh, nw); else GetPaperSize(A, nw, nh);
		for (auto& p:pPage->seeds)
		{ //inflate or deflate
			p.second.x=((p.second.x * nw)/cw);
			p.second.y=((p.second.y * nh)/ch);
		}
		pPage->paperAn=A;
		cbP.SetData(pPage->paperAn);
		set_paper_page();

		map.flate_shapes(cw, ch, nw, nh);
		OpenPage(PAGEID); //reshow everything
		bbusy=false;
	}
}
	
void Prober::SetPaperOrientation(int O)
{
	static bool bbusy=false;
	if (bbusy) return;
	if (((O==LANDSCAPE)||(O==PORTRAIT))&&!(pPage->pagePL==O))
	{
		bbusy=true;
		pPage->pagePL=O;
		radLP=pPage->pagePL;
		for (auto& p:pPage->seeds)
		{ //like a page in a binder
			int t=p.second.x;
			if (O==PORTRAIT) { p.second.x=p.second.y; p.second.y=(papersize.cx-t); }
			else { p.second.x=(papersize.cy-p.second.y); p.second.y=t; }
		}
		set_paper_page();

		int wh=((O==LANDSCAPE)?papersize.cx:papersize.cy);
		map.flip_shapes((O==LANDSCAPE)?MapWidget::LP_LTOP:MapWidget::LP_PTOL, wh);
		bbusy=false;
		OpenPage(pPage->idpage);
	}
}

void Prober::add_menu_map(Bar &bar, Point p, dword d)
{
	if (map.has_multi_sel())
	{
		bar.Add("Copy selection", [&]{ telluser("todo copy"); }); //copy vsel(seed-pos, item-name&note) to local sel-list for pasting
		bar.Add("Cut selection", [&]{ telluser("todo cut"); }); //copy vsel(seed-pos, item-name&note) to local sel-list for pasting
		bar.Separator(); //'Cut' - user must copy/paste & then go back & delete - (later: mark sel-list as copy/cut & cut/delete orig on paste)
		bar.Add("Delete selection", [&]{ telluser("todo delete"); });
		bar.Separator();
	}
	bar.Add("New Topic", [&,p,d]{ map.clear_multi_sel(); NewSeed(p, d); });
}

/*
//move...
//void Prober::
	
	do copy/cut/paste to-from (deferred) anyway? - store: list of [x,y,name,note]'s
	//(deferred) to have 'cleanup_ccp_buffer()'?
	
void copycut_sel(bool bcut=false) //cannot copy/cut cross-project - use (rename deferred) as intermediary
{
	if (map.has_multi_sel())
	{
		cur_sel_list.clear();
		cur_sel_list.bcut=bcut;
		for (auto pt:map.vmultisel)
		{
			get seed
			cur_sel_list.add(pageid, seedid); //use seed.x/y for relative-positions
		}
	}
}

void paste_cur_sel(Point p)
{
}
*/

void Prober::add_menu_topic(Bar &bar, TopicWidget *pTW, Point p, dword d)
{
	Seed *pS=pPage->GetSeed(pTW);
	bar.Add("Edit topic", [&,pS]{ edit_seed(pS); });
	if (pS->disp==0) bar.Add("Expand topic..", [&,pS]{ expand_seed(pS);});
	else if (pS->disp<0) bar.Add("Go to Origin..", [&,pS]{ OpenPage(-(pS->disp)); });
	else if (pS->disp>0)
	{
		bar.Add("Go to Expansion..", [&,pS]{ OpenPage(pS->disp); });
		bar.Separator();
		bar.Add("Delete Expansion..", [&,pS]{ DeleteSeedPage(pS); });
	}
	bar.Separator();
	bar.Add("Delete topic", [&,pTW]{ DeleteSeed(pTW); });
}

void Prober::add_menu_link(Bar &bar, LinkWidget *pLD, Point p, dword d)
{
	bar.Add("Edit association", [&,pLD,p,d]{ EditAssoc(pLD); });
	bar.Add("Convert association to topic", [&,pLD]{ telluser("convert assoc to topic - todo"); });
	bar.Separator();
	bar.Add("Delete association", [&,pLD]{ DeleteLinkAssoc(pLD); });
}

void Prober::add_menu_shape(Bar &bar, Shape *pS, Point p, dword d)
{
	if (pS->isa()==ISA_JOT)
	{
		bar.Add("Edit Jot..", [&,pS]{ EditJot(pS); });
		bar.Separator();
	}
	std::string sn=spf("Delete ", pS->shapename());
	bar.Add(sn.c_str(), [&,pS]{ DeleteShape(pS); });
}

void Prober::OnIsDrawingChange(bool b)
{
	for (auto& t:pPage->seedtopics) t.second->SetShowFocus(!b);
}

void Prober::OnShapeChanged(Shape *pS)
{
	Tag *pT=pPage->GetTag(pS);
	pT->tagdata=pS->ShapeData();
	pT->Save();
}

void Prober::OnShapeDeleted(Shape *pS)
{
	Tag *pT=pPage->GetTag(pS);
	pPage->DropShape(pS);
	pPage->RemoveTag(pT);
}

void Prober::DeleteShape(Shape *pS)
{
	Tag *pT=pPage->GetTag(pS);
	pPage->DropShape(pS);
	pPage->RemoveTag(pT);
	map.DeleteShape(pS);
}

void Prober::EditJot(Shape *pS)
{
	if (((Jot*)pS)->Edit())
	{
		Tag *pT=pPage->GetTag(pS);
		pT->UpdateData(pS->ShapeData());
		Refresh();
	}
}

bool Prober::DoNewTopic(TopicWidget *pT)
{
	SItem I;
	I.name=pT->GetText();
	if (EditItem(&I))
	{
		SItem *pI=sitems.Add(I);
		if (!pI) return false;
		Rect r=pT->GetRect();
		Seed *pS=pPage->AddSeed(pI->iditem, r.left, r.top, 0);
		if (pS)
		{
			pT->SetText(pI->name);
			if (pPage->AddTopic(pS, pT)) { return true; }
			else
			{
				pPage->RemoveSeed(pS);
				sitems.Delete(pI->iditem);
			}
		}
	}
	return false;
}

bool Prober::DoNewLink(LinkWidget *pL)
{
	Seed *pF=pPage->GetSeed(pL->pTF);
	Seed *pT=pPage->GetSeed(pL->pTT);
	if ((pF&&pT)&&!pPage->has_link(pF->idseed, pT->idseed))
	{
		SItem I;
		I.name=pL->name;
		if (EditItem(&I))
		{
			SItem *pI=sitems.Add(I);
			if (!pI) return false;
			Assoc *pA=pPage->AddAssoc(pF->idseed, pT->idseed, pI->iditem);
			if (pA)
			{
				if (pPage->AddLink(pA, pL)) { pL->name=pI->name; return true; }
				else
				{
					pPage->RemoveAssoc(pA);
					sitems.Delete(pI->iditem);
				}
			}
		}
	}
	return false;
}

bool Prober::DoNewShape(Shape *pS)
{
	Tag *pT=pPage->AddTag(pS->ShapeData());
	if (pT) return pPage->AddShape(pT->idtag, pS);
	return false;
}

void Prober::OnMultiSelect(std::vector<TopicWidget*> vsel)
{
	//..
	
	///telluser("Got ", vsel.size(), " topics in multi-selection"); tested passing selection here
	
}

void Prober::OnTopicRightClick(TopicWidget *pTW, Point p, dword d) { RTMenu.Clear(); add_menu_topic(RTMenu, pTW, p, d); RTMenu.PopUp(); }
void Prober::OnLinkRightClick(LinkWidget *pLW, Point p, dword d) { RTMenu.Clear(); add_menu_link(RTMenu, pLW, p, d); RTMenu.PopUp(); }
void Prober::OnMapRightClick(Point p, dword d) { RTMenu.Clear(); add_menu_map(RTMenu, p, d); RTMenu.PopUp(); }
void Prober::OnShapeRightClick(Shape *pS, Point p, dword d) { RTMenu.Clear(); add_menu_shape(RTMenu, pS, p, d); RTMenu.PopUp(); }

void Prober::OnTopicLeftDoubleClick(TopicWidget *pTW, Point p, dword d) { edit_seed(pPage->GetSeed(pTW)); }

void Prober::OnTopicMoved(TopicWidget *pTW)
{
	Seed *pS=pPage->GetSeed(pTW);
	Point pt=map.GetTopicPos(pTW);
	pS->x=pt.x; pS->y=pt.y;
	pS->Save();
}

void Prober::OnLinkLeftDoubleClick(LinkWidget *pLD, Point p, dword d) { EditAssoc(pLD); }

//void Prober::OnMapLeftDoubleClick(Point p, dword d) { NewSeed(p,d); }

//void Prober::OnTopicLinkTopic(TopicWidget *pTWF, TopicWidget *pTWT) { DoTopicLinkTopic(pTWF, pTWT); }

//void Prober::DoTopicLinkTopic(TopicWidget *pTWF, TopicWidget *pTWT, const std::string &assoctitle, const std::string &assocnote)
//{
//	Seed *pF=pPage->GetSeed(pTWF);
//	Seed *pT=pPage->GetSeed(pTWT);
//	if (!pPage->has_link(pF->idseed, pT->idseed))
//	{
//		SItem *pI=sitems.Add(assoctitle, assocnote);
//		if (pI)
//		{
//			Assoc *pA=pPage->AddAssoc(pF->idseed, pT->idseed, pI->iditem);
//			LinkWidget *pLD=map.MakeLink(pTWF, pTWT, pI->name);
//			if (pA && pLD && pPage->AddLink(pA, pLD))
//			{
//				map.Refresh();
//			}
//			else
//			{
//				if (pLD) map.DeleteLink(pLD);
//				pPage->DropLink(pLD);
//				pPage->RemoveAssoc(pA);
//				tellerror("cannot create association (pA-", (pA?"OK":"Fail"), ", pLD-", (pLD?"OK":"Fail"), ")");
//			}
//		}
//		else
//		{
//			tellerror("cannot create association item");
//		}
//	}
//	
//	Refresh();
//}

//void Prober::OnTopicLinkNewTopic(TopicWidget *pTWF, Point p)
//{
//	Seed *pS=NewSeed(p,0);
//	if (pS) { OnTopicLinkTopic(pTWF, pPage->GetTopic(pS->idseed)); }
//}

void Prober::LinkSeeds(Seed &from, Seed &to, const std::string &assoctitle)
{
	//..
}

bool Prober::edit_seed(Seed *pS)
{
	SItem *pI=sitems.Get(pS->iditem);
	if (pI) return EditItem(pI, (pS->disp<0));
	return false;
}

bool Prober::edit_assoc(Assoc &A)
{
	SItem *pI=sitems.Get(A.iditem);
	if (pI) return EditItem(pI);
	return false;
	
}

Seed* Prober::NewSeed(Point p, dword d)
{
	Seed *pS=nullptr;
	SItem I;
	if (EditItem(&I))
	{
		pS=pPage->AddSeed(I.iditem, p.x, p.y, 0);
		if (pS)
		{
			if (pPage->AddTopic(pS, map.MakeTopic(pS->x, pS->y, I.name, pS->disp)))
			{
				TopicWidget* pT=pPage->GetTopic(pS->idseed);
				std::string stip=I.def;
				if (stip.empty()) stip=I.name;
				else if (stip.size()>512) stip=spf(stip.substr(0,500), "\n(..more..)");
				pT->SetToolTip(stip);
			}
		}
		map.Refresh();
	}
	return pS;
}

bool Prober::EditItem(SItem *pI, bool bprimary)
{
	if (bprimary) pModeler->MSB.ShowText("Primary seed must be edited at origin");
	DlgItem dlg;
	std::string oldname=pI->name;
	dlg.ebName.SetData(pI->name.c_str());
	dlg.ebNote.SetData(pI->def.c_str());
	dlg.ebName.Enable(!bprimary);
	dlg.ebNote.SetEditable(!bprimary);
	dlg.Execute();
	if (bprimary) dlg.bok=false;
	if (dlg.bok)
	{
		pI->name=dlg.ebName.GetData().ToString().ToStd();
		pI->def=dlg.ebNote.GetData().ToString().ToStd();
		pI->Save();
		sitems.push(*pI);
		
		if (pI->iditem)
		{
			auto it=namepages.find(oldname);
			if (it!=namepages.end())
			{
				int pg=(*it).second;
				namepages.erase(it);
				namepages[pI->name]=pg;
				std::string s=cbPages.Get().ToString().ToStd();
				if (s==oldname) s=pI->name;
				cbPages.Clear();
				for (auto p:namepages) cbPages.Add(p.first.c_str());
				cbPages.SetData(s.c_str());
			}
		}
		UpdatedItem(pI->iditem);
		//ParsexSxexexdxNote(*pS); **********
	}
	pModeler->MSB.ClearText();
	this->SetFocus(); //if called in seq for topic & then link, it loses "who's ur daddy"
	return dlg.bok;
}

void Prober::UpdatedItem(int itemid)
{
	std::vector<LinkWidget*> vpl;
	std::vector<TopicWidget*> vpt;
	SItem *pI=sitems.Get(itemid);
	if (pPage->GetItemLinks(itemid, vpl)) { for (auto& pL:vpl) pL->name=pI->name; }
	if (pPage->GetItemTopics(itemid, vpt)) { for (auto& pT:vpt) { pT->SetText(pI->name); pT->SetToolTip(pI->def); }}
	map.Refresh();
}

void Prober::DeleteSeed(TopicWidget *pTW)
{
	Seed *pS=pPage->GetSeed(pTW);
	if (pS) DeleteSeedAssocs(pS);
	map.DeleteTopic(pTW);
	pPage->DropTopic(pTW);
	pPage->RemoveSeed(pS);
	Refresh();
}

void Prober::EditAssoc(LinkWidget *pLD)
{
	Assoc *pA=pPage->GetAssoc(pLD);
	SItem *pI=sitems.Get(pA->iditem);
	EditItem(pI);
}

void Prober::DeleteSeedAssocs(Seed *pS)
{
	std::vector<LinkWidget*> vpl;
	if (pPage->GetSeedLinks(pS, vpl)) { for (auto& pl:vpl) DeleteLinkAssoc(pl); }
}

void Prober::DeleteLinkAssoc(LinkWidget *pL)
{
	if (!pL) { tellerror("cannot delete invalid linkdata"); return; } //todo: write to logfile...
	Assoc *pA=pPage->GetAssoc(pL);
	if (!pA) { tellerror("cannot find assoc for linkdata"); return; } //todo: write to logfile...
	map.DeleteLink(pL);
	pPage->DropLink(pL);
	pPage->RemoveAssoc(pA);
}

void Prober::expand_seed(Seed *pS)
{
	auto pP=pages.NewPage();
	if (pP)
	{
		if (!pP->AddSeed(pS->iditem, 50, 50, -(pPage->idpage)))
		{
			pages.DeletePage(pP);
			tellerror("Prober: cannot prime page");
			return;
		}
		pS->disp=pP->idpage; pS->Save();
		SItem *pI=sitems.Get(pS->iditem);
		if (pI) { namepages[pI->name]=pP->idpage; cbPages.Add(pI->name.c_str()); } //for droplist
		else { tellerror("Prober: cannot access item"); return; }
		pP->restore_content(); //initializes all
		OpenPage(pP->idpage);
	}
	else tellerror("Prober: cannot create page");
}

//=================================================================================================
//void Prober::do_discard(VTokens &vt)
//{
//	VTokens v{};
//	for (auto t:vt)
//	{
//		if (((t.Type==TT_TEXT)||(t.Type==TT_QSTR))&&!pModeler->wordlist.hasword("fillers", t.Tok)) v.push_back(t);
//	}
//	vt.clear();
//	vt=v;
//}

void Prober::ParseSeedNote(Seed &SN)
{
/*
	// This is just a quick&dirty for testing the app - fix todo


	//if not a discardable word or existing topic then add as a topic
	//if existing topic then link to it
	
	WaitCursor();

	if (SN.note.empty()) return;
	
	struct NounVerbs
	{
		std::map<std::string, WordListing> nvs; //a 'noun' can have many association-reasons(=='verbs')
		void clear() { nvs.clear(); }
		~NounVerbs() { clear(); }
		NounVerbs() { clear(); }
		NounVerbs(const NounVerbs &N) { *this=N; }
		NounVerbs& operator=(const NounVerbs &N) { nvs=N.nvs; return *this; }
		void Add(const std::string &n, const std::string &v) { nvs[n][v]=WD_USE; }
	};

	NounVerbs NV;

	auto doparse=[&](const std::string sraw, NounVerbs &NV)
				{
					VTokens vt;
					TToken t;
					tokenizestring(vt, sraw, WSD_REDUCE);
					std::string n{};
					std::string v{};
					std::map<std::string, int> mn{};
					
					while (vt.next(t))
					{
						if (((t.Type==TT_TEXT)||(t.Type==TT_QSTR))&&(t.Tok.size()>=3))
						{
							if (v.empty()&&pModeler->pwordlist->hasword("actions", t.Tok)) //nouns can masquerade as verbs (e.g.: 'house'->the building, or as-in: 'house him in a tent'
							{
								if (v.empty()) v=lcase(t.Tok);
								else v+=","+lcase(t.Tok);
							}
							else if (pModeler->pwordlist->hasword("fillers", t.Tok)) continue;
							else if (n.empty()) n=lcase(t.Tok);
							else { v="?"; vt.backup(); } //poss. new noun so verb is unk & redo for new noun
						}
						if (!n.empty()&&!v.empty())
						{
							
							// //need to figure out which is which?!? noun or verb, both? use nat.lang or specify some syntax? todo
							
							if (v=="?") mn[n]=0;
							else
							{
								mn.erase(n); //if there
								NV.Add(n, v);
							}
							n.clear(); v.clear();
						}
					}
					if (!mn.empty()) { for (auto p:mn) NV.Add(p.first, "(?)"); }
					mn.clear();
				};

	doparse(SN.note, NV);
	if (!NV.nvs.empty())
	{
		int nx=2;
		int x=(papersize.cx-(nx*50)); //start placing at right
		int y=30;
		std::string sname;
		std::string snote{};
		for (auto p:NV.nvs)
		{
			pModeler->pwordlist->addword("things", p.first, WD_USE);
			if (p.second.size()==1) sname=p.second.begin()->first;
			else
			{
				sname="(see note)";
				for (auto pp:p.second) snote+=pp.first+"\n";
			}
			add_note_seedling(SN, p.first, x, y, sname, snote);
			//just to get all to stay visible on page
			y+=30; if (y>(papersize.cy-50)) { y=50; nx++; x=(papersize.cx-(nx*50)); if (x<30) { nx=2; x=(papersize.cx-(nx*50)); }}
		}
	}
*/
}

bool Prober::add_note_seedling(Seed &SF, std::string sn, int x, int y, std::string slname, std::string slnote)
{
//	
//	if (check_existing_topic_link(SF, sn)) return true;
//	
//	Seed S;
//	S.name=sn;
//	S.pos=Point(x,y);
//	if (dbprober.Save(S))
//	{
//		seeds[S.id]=S;
//		maptopics[S.id]=map.MakeTopic(S.name, x, y);
//		DoTopicLinkTopic(maptopics[SF.id], maptopics[S.id], slname, slnote);
//		return true;
//	}
	return false;
}

bool Prober::check_existing_topic_link(Seed &SF, std::string sn)
{
//	if (sieqs(sn, SF.name)) return true;
//	for (auto p:seeds) { if (sieqs(p.second.name, sn)) { OnTopicLinkTopic(maptopics[SF.id], maptopics[p.first]); return true; }}
	return false;
}

//================================================================================================={{DEBUG}}

//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG
//const std::string showpages(SPages &pgs)
//{
//	auto shnm=[](int e)->std::string{ return ((e==1)?"SHA":(e==2)?"SKE":(e==3)?"LIN":(e==4)?"BOX":(e==5)?"OVL":(e==6)?"JOT":"UNK"); };
//
//	std::string s{};
//	
//	for (auto& p:pgs)
//	{
//		SPage *pP=&(p.second);
//		std::string ss{};
//		int ns=0;
//		for (auto& p:pP->shapes)
//		{
//			ns++;
//			ss+=spf("\n\t",ns, " : ", shnm(p->isa()));
//		}
//		s=spf("\npageid=", pP->idpage, "\nshape-count=", ns, "\nshapes:", ss);
//	}
//	return s;
//}
//
//void showpage(SPage *pP, const std::string &context)
//{
//	auto shnm=[](int e)->std::string{ return ((e==1)?"SHA":(e==2)?"SKE":(e==3)?"LIN":(e==4)?"BOX":(e==5)?"OVL":(e==6)?"JOT":"UNK"); };
//	
//	std::string s;
//	std::string ss{};
//	int ns=0;
//	for (auto& p:pP->shapes)
//	{
//		ns++;
//		ss+=spf("\n\t",ns, " : ", shnm(p->isa()));
//	}
//	s=spf("\npageid=", pP->idpage, "\nshape-count=", ns, "\nshapes:", ss);
//	telluser(context, "\nPage: ", s);
//}
//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG


