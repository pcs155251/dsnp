/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

typedef set<CirGate*,CompareGateId> gateSet;

struct CompareGateId
{
   bool operator()(const CirGate* lgate, const CirGate* rgate) const;
};

struct gateSetPHasher
{
   size_t
   operator() (const gateSet* in ) const;
};

struct gateSetPComper
{
   bool
   operator() (const gateSet* in0, const gateSet* in1 ) const;
};

typedef unordered_set<gateSet*,gateSetPHasher,gateSetPComper> fecgs;

class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;
   //my
   void printFECPairs( unsigned gid ) const;

private:
   ofstream           *_simLog;

   //my
   map<unsigned,CirGate*> gates;
   vector<CirGate*> pins;
   vector<CirGate*> pouts;
   gateSet aigs;
   gateSet floats;
   gateSet floatfins;
   gateSet notused;
   vector<CirGate*> dfsList;
   map<unsigned,gateSet*> fecGroups;
   //implementation
   void updateDfsList( );
   void updateFloatFins( );
   void updateNotUsed( );
   void removeGates( const vector<CirGate*>& removeList );
   bool ifsimulated=false;
};

#endif // CIR_MGR_H
