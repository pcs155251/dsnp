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


private:
   unsigned lineNo;

   //my
   unsigned gid;
   unsigned typeId=0;
   static vector<string> typeString;

protected:
   //my
   //for derived class constructor
   CirGate(unsigned lineNoIn, unsigned gidIn, unsigned typeIdIn, string nameIn): lineNo(lineNoIn), gid(gidIn), typeId(typeIdIn), name(nameIn) {}
   string name;
   //my uninitialized memebers
   vector<pair<bool,CirGate*>> fin;
   vector<CirGate*> fout;

};

class CirPiGate: public CirGate
{
public:
   CirPiGate(unsigned lineNoIn, unsigned gidIn, string nameIn): CirGate(lineNoIn, gidIn, 1, nameIn) {}
   CirPiGate() {}
   ~CirPiGate() {}
   virtual void printGate() const;
private:
};

class CirPoGate: public CirGate
{
public:
   CirPoGate() {}
   ~CirPoGate() {}
   virtual void printGate() const;
private:
};

class CirAigGate: public CirGate
{
public:
   CirAigGate() {}
   ~CirAigGate() {}
   virtual void printGate() const;
private:
};

#endif // CIR_GATE_H
