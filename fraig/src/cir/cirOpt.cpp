/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
bool IFDEBUG = false;

void
CirMgr::sweep()
{
   //find bad gates
   notused.clear();
   vector<CirGate*> removeList;
   map<unsigned,CirGate*> tmpGates = gates;
   tmpGates.erase(0);//remove CONST gate
   for (unsigned i=0; i!=pouts.size(); ++i )
   {
      if (IFDEBUG) cerr<<"pin: "<<pouts[i]->getId()<<endl<<"erase:";
      vector<unsigned> pathIds;
      CirGate::setRefMark();
      pouts[i]->dfsTraverseToIn(false,pathIds);
      for (unsigned ii=0; ii!=pathIds.size(); ++ii)
      {
         if (IFDEBUG) cerr<<" "<<pathIds[ii];
         tmpGates.erase(pathIds[ii]);
      }
      if (IFDEBUG) cerr<<endl;
   }
   for (map<unsigned,CirGate*>::iterator it=tmpGates.begin(); it!=tmpGates.end(); ++it)
   {
      string gateType = it->second->getTypeStr();
      if ( gateType=="UNDEF" || gateType=="AIG" )
      { 
         removeList.push_back( it->second );
      }
      else if ( gateType=="PI" )
      {
         notused.insert( it->second );
      } else {} //will never be PO
   }

   for (unsigned i=0; i!=removeList.size(); ++i)
   {
      CirGate* remG = removeList[i];
      cout<<"Sweeping: "<<remG->getTypeStr()<<"("<<remG->getId()<<") removed..."<<endl;
      //remove fanin connection
      for (vector<pair<bool,CirGate*>>::iterator it=remG->fins.begin(); it!=remG->fins.end(); ++it )
      {
         CirGate* target = it->second;
         target->eraseFout( remG );
      }
      //remove fanout connection
      for (multiset<CirGate*>::iterator it=remG->fouts.begin(); it!=remG->fouts.end(); ++it )
      {
         CirGate* target = *it;
         target->eraseFin( remG );
      }
      //remove gate
      gates.erase( remG->getId() );
      aigs.erase( remG );
      floats.erase( remG );
      floatfins.erase( remG );
   }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
