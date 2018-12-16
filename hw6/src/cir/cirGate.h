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
   CirGate() {}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return lineNo; }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   //my

private:
   unsigned lineNo;

   //my
   string name;

protected:
   //my
   vector<CirGate*> fin;
   vector<CirGate*> fout;

};

class CirPiGate: public CirGate
{
public:
   CirPiGate() {}
   ~CirPiGate() {}
   virtual void printGate(){};
private:
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
