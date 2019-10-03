#ifndef _modeler_prober_h_
#define _modeler_prober_h_

#include <CtrlLib/CtrlLib.h>
using namespace Upp;

#include "globdefs.h"
#include <map>
#include <utility>
#include <dbsqlite3/dbsqlite3types.h>
#include "uppglobs.h" //for panel
#include <functional>
#include <tuple>
#include <mapwidget/mapwidget.h>

struct Modeler;

//-------------------------------------------------------------------------------------------------SItem
struct SItem
{
	int iditem;
	std::string name;
	std::string def;
	void clear();
	~SItem();
	SItem();
	SItem(const std::string &sname, const std::string &sdef="");
	SItem(const SItem &I);
	SItem& operator=(const SItem &I);
	bool Save();
	bool Delete();
};

//-------------------------------------------------------------------------------------------------SItems
struct SItems
{
private:
	std::map<int, SItem> list;
	
public:
	using iterator=std::map<int, SItem>::iterator;
	using const_iterator=std::map<int, SItem>::const_iterator;
	void clear();
	void push(const SItem &I);
	~SItems();
	SItems();
	SItems(const SItems&)=delete; //only one set for seeder
	SItems& operator=(const SItems&)=delete;
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	bool empty();
	bool Load();
	SItem* Add(const SItem &S);
	SItem* Add(const std::string &sname, const std::string &snote="");
	bool Delete(int itemid);
	SItem* Get(int itemid);
};

//-------------------------------------------------------------------------------------------------item dialogs
struct DlgItem : public TopWindow
{
	using CLASSNAME=DlgItem;
	bool bok;
	bool bs;
	int id;
	int oid;
	String Sa;
	String So;
	Label lblName;
	EditString ebName;
	Button btnSel;
	//Button btnX;
	Label lblNote;
	//Label lblShared;
	DocEdit ebNote;
	Button btnOK;
	Button btnCancel;
	virtual ~DlgItem();
	DlgItem(bool bshared=false);
};

struct DlgSelItem : public TopWindow
{
	using CLASSNAME=DlgSelItem;
	int ret;
	int xid;
	ArrayCtrl arItems;
	//Button btnShare;
	//Button btnCopy;
	Button btnUse;
	Button btnCancel;
	virtual ~DlgSelItem();
	DlgSelItem(int xcludeid=0);
	void fillar();
	SItem GetSelItem();
};

//-------------------------------------------------------------------------------------------------Seed
struct Seed
{
	int idseed, idpage, iditem, x, y, disp;
	void clear();
	~Seed();
	Seed();
	Seed(const Seed &S);
	Seed& operator=(const Seed &S);
	bool Save();
	bool Delete();
};

//-------------------------------------------------------------------------------------------------Seeds
struct Seeds
{
private:
	std::map<int, Seed> seeds; //[idseed]=
	
public:
	using iterator=std::map<int, Seed>::iterator;
	using const_iterator=std::map<int, Seed>::const_iterator;
	
	void clear();
	void push(const Seed &S);
	
	~Seeds();
	Seeds();
	Seeds(const Seeds&);
	Seeds& operator=(const Seeds&);
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	size_t size() { return seeds.size(); }
	bool empty();
	bool Load(int pageid);
	
	Seed* Add(const Seed &S);
	bool Delete(int seedid);
	Seed* Get(int seedid);
	
};

//-------------------------------------------------------------------------------------------------Assoc
struct Assoc
{
	int idassoc, idpage, iditem, fidseed, tidseed;
	void clear();
	~Assoc();
	Assoc();
	Assoc(const Assoc &A);
	Assoc& operator=(const Assoc &A);
	bool Save();
	bool Delete();
};

//-------------------------------------------------------------------------------------------------Assocs
struct Assocs
{
private:
	std::map<int, Assoc> assocs; //[idassoc]=
	
public:
	using iterator=std::map<int, Assoc>::iterator;
	using const_iterator=std::map<int, Assoc>::const_iterator;

	void clear();
	void push(const Assoc &A);
	
	~Assocs();
	Assocs();
	Assocs(const Assocs&);
	Assocs& operator=(const Assocs&);
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	size_t size() { return assocs.size(); }
	bool empty();
	bool Load(int pageid);

	Assoc* Add(const Assoc &A);
	bool Delete(int associd);
	Assoc* Get(int associd);
};

//-------------------------------------------------------------------------------------------------Tag
struct Tag
{
	int idtag, idpage;
	std::string tagdata;
	void clear();
	~Tag();
	Tag();
	Tag(const Tag &T);
	Tag& operator=(const Tag &T);
	bool Save();
	bool Delete();
	bool UpdateData(const std::string &tdata);
};

//-------------------------------------------------------------------------------------------------Tags
struct Tags
{
private:
	std::map<int, Tag> tags; //[idtag]=
	
public:
	using iterator=std::map<int, Tag>::iterator;
	using const_iterator=std::map<int, Tag>::const_iterator;

	void clear();
	void push(const Tag &T);
	
	~Tags();
	Tags();
	Tags(const Tags&);
	Tags& operator=(const Tags&);
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	size_t size() { return tags.size(); }
	bool empty();
	bool Load(int pageid);

	Tag* Add(const Tag &T);
	bool Delete(int tagid);
	Tag* Get(int tagid);
};

//-------------------------------------------------------------------------------------------------SPage
struct SPage
{
	int idpage;
	int X; //scroll-pos
	int Y;
	int paperAn; //A-size of map (limited from 4 to 0)
	int pagePL; //Portrait/Landscape
	Seeds seeds;
	Assocs assocs;
	Tags tags; //"annotations"
	std::map<int, TopicWidget*> seedtopics; //[idseed]
	std::map<int, LinkWidget*> assoclinks; //[idassoc]
	std::map<int, Shape*> tagshapes; //[idtag]

	void clear();

	~SPage();
	SPage();
	SPage(const SPage &P);
	SPage& operator=(const SPage &P);
	
	bool Save();
	bool IsEmpty();
	bool Delete();
	void restore_content(); //clear & load sp* from db

	Seed* AddSeed(const Seed &S);
	Seed* AddSeed(int itemid, int xx, int yy, int disp);
	void RemoveSeed(Seed *pS);
	Seed* GetSeed(int seedid);
	Seed* GetSeed(TopicWidget *pT);
	TopicWidget* GetTopic(int seedid);
	
	Assoc* AddAssoc(const Assoc &A);
	Assoc* AddAssoc(int fseedid, int tseedid, int itemid=0);
	void RemoveAssoc(Assoc *pA);
	Assoc* GetAssoc(int associd);
	Assoc* GetAssoc(LinkWidget *pLD);
	LinkWidget* GetLink(int associd);
	
	bool AddTopic(Seed *pS, TopicWidget *pT);
	void DropTopic(TopicWidget *pT);

	bool AddLink(Assoc *pA, LinkWidget *pL);
	void DropLink(LinkWidget *pL);
	bool has_link(int fid, int tid);

	bool GetItemLinks(int itemid, std::vector<LinkWidget*> &vpl);
	bool GetSeedLinks(Seed *pS, std::vector<LinkWidget*> &v);
	bool GetItemTopics(int itemid, std::vector<TopicWidget*> &vpt);

	Tag* AddTag(const Tag &T);
	Tag* AddTag(const std::string &tdata);
	void RemoveTag(Tag *pT);
	Tag* GetTag(int tagid);
	Tag* GetTag(Shape *pS);
	Shape* GetShape(int tagid);

	bool AddShape(int tagid, Shape *pS);
	void DropShape(Shape *pS);
	
};

//-------------------------------------------------------------------------------------------------SPages
struct SPages
{
private:
	std::map<int, SPage> pages; //[idpage]=
	
public:
	using iterator=std::map<int, SPage>::iterator;
	using const_iterator=std::map<int, SPage>::const_iterator;

	void clear();
	void push(const SPage &P);
	
	virtual ~SPages();
	SPages();
	SPages(const SPages &PP);

	SPages& operator=(const SPages &PP);
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	bool empty();
	bool Load(); //all pages
	
	SPage* NewPage();
	bool DeletePage(SPage *pP);
	SPage* GetPage(int pageid);
	
};

//	RENAME SEEDER TO 'Prober'

//-------------------------------------------------------------------------------------------------Prober
struct Prober : public Ctrl
{
	using CLASSNAME=Prober;

	Modeler *pModeler;

	SItems sitems;
	SPages pages;
	SPage *pPage;
	std::map<std::string, int> namepages;
	Size papersize; //for Paint()
	HScrollBar HS;
	VScrollBar VS;
	Image prevcursor;
	MapWidget map;
	MenuBar RTMenu;

	//page-size/orient-panel
	Label lblPage;
	DropList cbPages;
	Label lblP;
	DropList cbP;
	Switch radLP;
	Panel panTools;
	Panel panMap;
	double ppmmX; //pixels per millimeter
	double ppmmY;

	void clear();
	void ctor_init();

	Prober();
	~Prober();
	
	bool Initialize(Modeler *pMod);
	void Terminate();
	
	void FillCBPages();
	void ShowCurCBPage(int pageid);
	void DoCBPages();

	bool make_page_1();
	void update_main_topic(const std::string &newname, const std::string &desc);

	void SetPageTools();
	
	void OpenPage(int page); //NB: new pages can only be created by expanding a seed, see: expand_seed()
	void close_page();
	bool DeleteSeedPage(Seed *pS);

	bool has_content(int page);
	
	void set_page_x(int x);
	void set_page_y(int x);
	
	void Paint(Draw &drw);
	void Layout();

	void DoHScroll();
	void DoVScroll();
	void MouseWheel(Point, int zdelta, dword);

	void set_paper_page();
	void SetPaperSize(int A);
	void SetPaperOrientation(int O);

	//popup menu fillers..
	void add_menu_map(Bar &bar, Point p, dword d);
	void add_menu_topic(Bar &bar, TopicWidget *pT, Point p, dword d);
	void add_menu_link(Bar &bar, LinkWidget *pL, Point p, dword d);
	void add_menu_shape(Bar &bar, Shape *pS, Point p, dword d);
	
	void OnIsDrawingChange(bool b);
	void OnShapeChanged(Shape *pS);
	void OnShapeDeleted(Shape *pS);
	void DeleteShape(Shape *pS);
	void EditJot(Shape *pS);
	
	bool DoNewTopic(TopicWidget *pT);
	bool DoNewLink(LinkWidget *pL);
	bool DoNewShape(Shape *pS);
	
	void OnMultiSelect(std::vector<TopicWidget*> vsel);
	
	//map-callback-handlers..
	void OnTopicRightClick(TopicWidget*, Point, dword);
	void OnTopicLeftDoubleClick(TopicWidget*, Point, dword);
	void OnTopicMoved(TopicWidget*);
	void OnLinkRightClick(LinkWidget*, Point, dword);
	void OnLinkLeftDoubleClick(LinkWidget*, Point, dword);
	void OnMapRightClick(Point, dword);
	//void OnMapLeftDoubleClick(Point, dword);
	void OnShapeRightClick(Shape *pS, Point p, dword d);
	
	//void OnTopicLinkTopic(TopicWidget*, TopicWidget*);
	//void DoTopicLinkTopic(TopicWidget*, TopicWidget*, const std::string &assoctitle="-?-", const std::string &assocnote="");
	
	//void OnTopicLinkNewTopic(TopicWidget*, Point);

	void LinkSeeds(Seed &from, Seed &to, const std::string &assoctitle);
	
	bool edit_seed(Seed *pS);
	bool edit_assoc(Assoc &A);
	
	bool EditItem(SItem *pI, bool bprimary=false);
	void UpdatedItem(int itemid);

	Seed* NewSeed(Point p, dword d);
	void EditSeed(TopicWidget *pTW);
	void DeleteSeed(TopicWidget *pTW); // --> must also delete links...
	void EditAssoc(LinkWidget *pLD);
	void DeleteSeedAssocs(Seed *pS);
	void DeleteLinkAssoc(LinkWidget *pLD);

	void expand_seed(Seed *pS);

	
//	void do_discard(VTokens &vt);

	void ParseSeedNote(Seed &SN);
	bool add_note_seedling(Seed &SF, std::string sn, int x, int y, std::string slname, std::string slnote);
	bool check_existing_topic_link(Seed &SF, std::string sn);

	virtual bool Key(dword key, int count) { /*if (map.IsDrawing())*/ return map.Key(key, count); }

};

//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG
//const std::string showpages(SPages &pgs);
//void showpage(SPage *pP, const std::string &context="");
//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG//DEBUG



#endif
