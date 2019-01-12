/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes

class CirGate
{
public:
   CirGate() {}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return lineNo; }
   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   //my
   unsigned getId() const { return gid; }
   void setNameStr( const string& nameIn ) { name = nameIn; }
   string getNameStr() const { return name; }
   void addFin(bool ifNotVert, CirGate* in) { fins.push_back( pair<bool,CirGate*> ( ifNotVert, in) ); }
   void addFout(bool ifNotVert, CirGate* out) { fouts.push_back( pair<bool,CirGate*> (ifNotVert, out)); }

   //void setMarked(bool flag) const {ifmarked=flag;}
   //bool getMarked() const {return ifmarked;}
   bool isMarked() const {return (mark==refMark);}
   void setMarked() const { mark = refMark; }
   static void setRefMark() { refMark++; }
   void dfsTraverse() const;
   bool dfsSearch(CirGate* target) const;
   static unsigned count;

private:
   unsigned lineNo;

   //my
   //mutable bool ifmarked=false;
   static unsigned refMark;
   mutable unsigned mark;
   void reportFanin(int level, unsigned offset, bool iftrue ) const;
   void reportFanout(int level, unsigned offset, bool iftrue ) const;

protected:
   //my
   //for derived class constructor
   CirGate(unsigned lineNoIn, unsigned gidIn ): lineNo(lineNoIn), gid(gidIn) {}
   unsigned gid;
   string name;
   //my uninitialized memebers
   vector<pair<bool,CirGate*>> fins;
   vector<pair<bool,CirGate*>> fouts;
};

class CirPiGate: public CirGate
{
public:
   CirPiGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn) {} 
   CirPiGate() {}
   ~CirPiGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "PI"; }
private:
};

class CirPoGate: public CirGate
{
public:
   CirPoGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn) { fins.reserve(1); }
   CirPoGate() {}
   ~CirPoGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "PO"; }
private:
};

class CirAigGate: public CirGate
{
public:
   CirAigGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn) { fins.reserve(2); }
   CirAigGate() {}
   ~CirAigGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "AIG"; }
private:
};

class CirConGate: public CirGate
{
public:
   CirConGate(): CirGate(0, 0) {}
   ~CirConGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "CONST"; }
private:
};

class CirFloGate: public CirGate
{
public:
   CirFloGate(unsigned lineNoIn,unsigned gidIn): CirGate(lineNoIn, gidIn) {}
   CirFloGate() {}
   ~CirFloGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "UNDEF"; }
private:
};

#endif // CIR_GATE_H
