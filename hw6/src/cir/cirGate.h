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
   virtual string getTypeStr() const { return ""; }//TODO
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

   void setMarked(bool flag) const {ifmarked=flag;}
   bool getMarked() const {return ifmarked;}
   void dfsTraverse();
   bool dfsSearch(CirGate* target);
   static unsigned count;

private:
   unsigned lineNo;

   //my
   mutable bool ifmarked=false;
   void reportFanin(int level, unsigned offset, bool iftrue, bool ifprint) const;
   void reportFanout(int level, unsigned offset, bool iftrue, bool ifprint) const;

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
   virtual string getTypeStr() const { return "PI"; }//TODO
private:
};

class CirPoGate: public CirGate
{
public:
   CirPoGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn) { fins.reserve(1); }
   CirPoGate() {}
   ~CirPoGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "PO"; }//TODO
private:
};

class CirAigGate: public CirGate
{
public:
   CirAigGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn) { fins.reserve(2); }
   CirAigGate() {}
   ~CirAigGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "AIG"; }//TODO
private:
};

class CirConGate: public CirGate
{
public:
   CirConGate(): CirGate(0, 0) {}
   ~CirConGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "CONST"; }//TODO
private:
};

class CirFloGate: public CirGate
{
public:
   CirFloGate(unsigned lineNoIn,unsigned gidIn): CirGate(lineNoIn, gidIn) {}
   CirFloGate() {}
   ~CirFloGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "UNDEF"; }//TODO
private:
};

#endif // CIR_GATE_H
