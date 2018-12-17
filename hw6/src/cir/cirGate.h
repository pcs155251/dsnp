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
   CirGate(unsigned lineNoIn, unsigned idIn): gid(idIn), lineNo(lineNoIn) {}
   CirGate() {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return lineNo; }
   //my
   unsigned getId() const { return gid; }

   // Printing functions
   virtual void printGate() const = 0;
   virtual void reportGate() const;
   virtual void reportFanin(int level) const;
   virtual void reportFanout(int level) const;

private:
   unsigned lineNo;

   //my

protected:
   //my
   unsigned gid;

};

class CirPiGate: public CirGate
{
public:
   CirPiGate(unsigned lineNoIn, unsigned gidIn, string nameIn): CirGate(lineNoIn, gidIn), name(nameIn) {}
   CirPiGate() {}
   ~CirPiGate() {}
   virtual void printGate() const;
   virtual void reportGate() const;
   virtual void reportFanin(int level) const;
   virtual void reportFanout(int level) const;
private:
   string name;
   vector<CirGate*> fout;
};

/*
class CirPoGate: public cirGate
{
public:
private:
}

class CirAigGate: public cirGate
{
public:
private:
}
*/

#endif // CIR_GATE_H
