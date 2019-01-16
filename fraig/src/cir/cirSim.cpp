/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

#include <bitset>

using namespace std;

size_t randomSizet()
{
   //RandomNumGen rng;
   int a = rnGen(INT_MAX);
   int b = rnGen(INT_MAX);
   return (size_t(a)<<32) + b;
}

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/

void
CirMgr::randomSim()
{
   //unordered_map<size_t,gateSet*> fecGroups;

   if ( !ifsimulated )
   {
   //add all gates in dfsList into one fecGroups
      gateSet* group0 = new gateSet;
      for ( unsigned i=0; i!=dfsList.size(); ++i )
      {
         group0->insert( dfsList[i] );
      }
      fecGroups.insert( pair<size_t,gateSet*> ( 0, group0 ) );
   } else {}

   //simulate
   unsigned maxSim = 128;
   for ( unsigned isim=0; isim!=maxSim; isim++ )
   {
      cout<<"Total #FEC Group = "<<fecGroups.size()<<endl;
      for ( unsigned i=0; i!=pins.size(); ++i )
      {
         size_t rn = randomSizet();
         pins[i]->setValue( rn );
      }
      for ( unsigned i=0; i!=dfsList.size(); ++i )
      {
         dfsList[i]->simulate();
      }
      //update value of fec groups
      //detect fecgroups
      for ( unordered_map<size_t,gateSet*>::iterator onegroup=fecGroups.begin(); onegroup!=fecGroups.end(); ++onegroup )
      {
         unordered_map<size_t,gateSet*> newFecGroups;
         for ( gateSet::iterator onegate = (*(onegroup->second)).begin(); onegate!=(*(onegroup->second)).end(); ++onegate )
         {
            unordered_map<size_t,gateSet*>::iterator resultGroup = newFecGroups.find( (*onegate)->getValue() );
            if ( resultGroup != newFecGroups.end() )
            {
               resultGroup->second->insert( *onegate );
            } 
            else 
            {
               gateSet* newGroup = new gateSet;
               newGroup->insert( *onegate );
               newFecGroups.insert( pair<size_t,gateSet*> ( (*onegate)->getValue(), newGroup ) );
            }
         }
         //collect valid fec groups
         for ( unordered_map<size_t,gateSet*>::iterator onenewG=newFecGroups.begin(); onenewG!=newFecGroups.end(); ++onenewG )
         {
            if ( onenewG->second->size() > 1 )
            {
               fecGroups.insert( *onenewG );
            } else {}
         }
      }
   }
   cout<<64*maxSim<<" patterns simulated."<<endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
