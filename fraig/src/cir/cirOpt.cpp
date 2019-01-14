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
void
CirMgr::sweep()
{
   /*
   notused.clear();
   vector<CirGate*> removeList;
   for (map<unsigned,CirGate*>::iterator it=gates.begin(); it!=gates.end(); ++it )
   {
      bool ifFindPout = false;
      for (unsigned ii=0; ii!=pouts.size(); ++ii)
      {
         CirGate::setRefMark();
         if (pouts[ii]->dfsSearch( it->second ) )
         {
           ifFindPout = true;
           break;
         } else {}
      }
      if ( !ifFindPout && it->second->getTypeStr()!="CONST" )
      {
         if ( it->second->getTypeStr()=="PI" )
         {
            notused.insert( it->second );
         }
         else
         {
            removeList.push_back( it->second );
         }
      }
   }
   for (unsigned i=0; i!=removeList.size(); ++i)
   {
      CirGate* remG = removeList[i];
      cout<<"Sweeping: "<<remG->getTypeStr()<<"("<<remG->getId()<<") removed..."<<endl;
      //remove fanin connection
      //for (vector<pair<bool,CirGate*>>::iterator it=remG->fins.begin(); it!=remG->fins.end())
      //{
      //}
      //remove fanout connection
      //remove gate
   }
   */
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
