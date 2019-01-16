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
#include <set>
#include <stack>
#include <iostream>
#include "cirDef.h"
#include "sat.h"
#include <bitset>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.
size_t bitFlip( const size_t& val, bool ifnv );
size_t convertLastToOne( const size_t& val );

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
public:
   CirGate(){}
   virtual ~CirGate() {}

   // Basic access methods
   virtual string getTypeStr() const { return ""; }
   unsigned getLineNo() const { return lineNo; }
   virtual bool isAig() const { return false; }

   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   
   //my basic
   unsigned getId() const { return gid; }
   void setNameStr( const string& nameIn ) { name = nameIn; }
   string getNameStr() const { return name; }
   //my operation for fanins and fanouts
   vector<pair<bool,CirGate*>> fins;
   vector<pair<bool,CirGate*>> fouts;

   void addFin(bool ifNotVert, CirGate* in) { fins.push_back( pair<bool,CirGate*> (ifNotVert, in) ); }
   //void addFout( CirGate* out) { fouts.insert( out ); }
   void addFout(bool ifNotVert, CirGate* out) { fouts.push_back( pair<bool,CirGate*> (ifNotVert, out) ); }
   void eraseAllFin( CirGate* target );
   void eraseAllFout( CirGate* target );
   void eraseOneFin( bool ifNotVert, CirGate* target );
   void eraseOneFout( bool ifNotVert, CirGate* target );
   bool noFanout() { return fouts.empty(); }
   //for simulation
   virtual size_t simulate() const { return value; }
   size_t getValue() const { return value; }
   void setValue( size_t valueIn ) const { value = valueIn; }

   //CirGate* dfsTraverseInExp( bool& curStates, stack<unsigned>& curFin, stack<CirGate*>& refGates ); 
   void replace( bool ifNotVert, CirGate* substitute );
   void merge( CirGate* substitute );
   CirGate* trivialOpt( CirGate* constGate );
   bool isMarked() const {return (mark==refMark);}
   void setMarked() const { mark = refMark; }
   static void setRefMark() { refMark++; }
   void dfsTraverseToIn( vector<CirGate*> &dfsList );
   void dfsTraverseToOut( bool ifprint, vector<unsigned> &pathGates ) const;
   bool hasFloatFin( ) const;
   static unsigned count;

private:
   unsigned lineNo;
   
   //my
   static unsigned refMark;
   mutable unsigned mark;
   void reportFanin(int level, unsigned offset, bool iftrue ) const;
   void reportFanout(int level, unsigned offset, bool iftrue ) const;

protected:
   //my
   CirGate(unsigned lineNoIn, unsigned gidIn): lineNo(lineNoIn), gid(gidIn) {} //for derived class constructor
   unsigned gid;
   string name;
   mutable size_t value=0;
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
   virtual size_t simulate() const { value = bitFlip( fins[0].second->getValue(), fins[0].first ); return value; }
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
   virtual size_t simulate() const 
   { 
      value = ( bitFlip( fins[0].second->getValue(), fins[0].first ) )
             &( bitFlip( fins[1].second->getValue(), fins[1].first ) )
             ;
      return value; 
   }
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
   CirFloGate(unsigned lineNoIn, unsigned gidIn): CirGate(lineNoIn, gidIn) {}
   CirFloGate() {}
   ~CirFloGate() {}
   virtual void printGate() const;
   virtual string getTypeStr() const { return "UNDEF"; }
private:
};

#endif // CIR_GATE_H
