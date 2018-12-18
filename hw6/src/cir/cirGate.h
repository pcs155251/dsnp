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
   string getTypeStr() const { return ""; }//TODO
   unsigned getLineNo() const { return lineNo; }
   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   //my
   unsigned getId() const { return gid; }
   unsigned getTypeId() const { return typeId; }
   void setNameStr( const string& nameIn ) { name = nameIn; }
   string getNameStr() const 
   {
      if (name!="")
      {
         return "\""+name+"\"";
      }
      else 
      {
         return "";
      }
   }
   void addFin(bool ifNotVert, CirGate* in) { fins.push_back( pair<bool,CirGate*> ( ifNotVert, in) ); }
   void addFout(CirGate* out) { fouts.push_back(out); }
   void setMarked(bool flag) {ifmarked=flag;}
   bool getMarked() const {return ifmarked;}
   void dfsTraverse();
   static unsigned count;


private:
   unsigned lineNo;

   //my
   bool ifmarked=false;

protected:
   //my
   //for derived class constructor
   CirGate(unsigned lineNoIn, unsigned gidIn, unsigned typeIdIn): lineNo(lineNoIn), gid(gidIn), typeId(typeIdIn) {}
   unsigned gid;
   unsigned typeId=0;
   static vector<string> typeString;
   string name;
   //my uninitialized memebers
   vector<pair<bool,CirGate*>> fins;
   vector<CirGate*> fouts;
};

class CirPiGate: public CirGate
{
public:
   CirPiGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn, 1) {} 
   CirPiGate() {}
   ~CirPiGate() {}
   virtual void printGate() const;
private:
};

class CirPoGate: public CirGate
{
public:
   CirPoGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn, 2) { fins.reserve(1); }
   CirPoGate() {}
   ~CirPoGate() {}
   virtual void printGate() const;
private:
};

class CirAigGate: public CirGate
{
public:
   CirAigGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn, 3) { fins.reserve(2); }
   CirAigGate() {}
   ~CirAigGate() {}
   virtual void printGate() const;
private:
};

class CirConGate: public CirGate
{
public:
   CirConGate(unsigned lineNoIn): CirGate(lineNoIn, 0, 4) {}
   CirConGate() {}
   ~CirConGate() {}
   virtual void printGate() const;
private:
};

class CirFloGate: public CirGate
{
public:
   CirFloGate(unsigned lineNoIn,unsigned gidIn): CirGate(lineNoIn, gidIn, 0) {}
   CirFloGate() {}
   ~CirFloGate() {}
   virtual void printGate() const;
private:
};

#endif // CIR_GATE_H
