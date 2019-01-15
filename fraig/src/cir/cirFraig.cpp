/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <unordered_map>
#include <utility>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
//
/*
struct pair_gate
{
   size_t operator() (const pair<CirGate*,CirGate*> &twoGates) const
   {
      uintptr_t i1 = reinterpret_cast<uintptr_t> (twoGates.first);
      uintptr_t i2 = reinterpret_cast<uintptr_t> (twoGates.second);
      return i1+i2;
   }
};

typedef unordered_map<pair<CirGate*,CirGate*>,CirGate*,pair_gate> stlhash;
*/
typedef unordered_map<HashKey,CirGate*,HashKey> stlhash;

void
CirMgr::strash()
{
   //stlhash hashlist;
   stlhash hashlist;
   vector<CirGate*> removeList;
   for ( vector<CirGate*>::iterator it = dfsList.begin(); it!=dfsList.end(); ++it )
   {
      if ( (*it)->fins.size()==2 )
      {
         HashKey akey( (*it)->fins[0].second, (*it)->fins[1].second, (*it)->fins[0].first, (*it)->fins[1].first );
         const pair<HashKey,CirGate*> obj( akey, *it );
         pair<stlhash::iterator,bool> result = hashlist.insert( obj );
         if ( result.second == true )
         {
         }
         else
         {
            (*it)->merge( result.first->second );
            removeList.push_back( *it );
         }
      } else {}
   }

   removeGates( removeList );
   updateNotUsed( );
   updateFloatFins( );
   updateDfsList( );
}

void
CirMgr::fraig()
{
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
