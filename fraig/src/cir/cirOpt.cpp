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
      vector<CirGate*> pathGates;
      CirGate::setRefMark();
      pouts[i]->dfsTraverseToIn(pathGates);
      for (unsigned ii=0; ii!=pathGates.size(); ++ii)
      {
         if (IFDEBUG) cerr<<" "<<pathGates[ii];
         tmpGates.erase(pathGates[ii]->getId());
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
         target->eraseAllFout( remG );
      }
      //remove fanout connection
      for (vector<pair<bool,CirGate*>>::iterator it=remG->fouts.begin(); it!=remG->fouts.end(); ++it )
      {
         CirGate* target = it->second;
         target->eraseAllFin( remG );
      }
      //remove gate
      gates.erase( remG->getId() );
      aigs.erase( remG );
      floats.erase( remG );
      floatfins.erase( remG );
   }

   updateDfsList( );
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   for (unsigned i=0; i!=dfsList.size(); ++i)
   {
      dfsList[i]->trivialOpt( gates.begin()->second );
   }
   updateDfsList( );
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
