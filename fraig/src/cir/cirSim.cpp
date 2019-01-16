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
   unsigned maxSim = 64;
   unsigned oldSize = 0;
   unsigned stopSize = 2;
   unsigned isim=0;
   for ( ; isim!=maxSim; isim++ )
   {
      unsigned newSize = fecGroups.size();
      cout<<"Total #FEC Group = "<<newSize<<endl;
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
      unordered_map<size_t,gateSet*> newFecGroups;
      for ( unordered_map<size_t,gateSet*>::iterator onegroup=fecGroups.begin(); onegroup!=fecGroups.end(); ++onegroup )
      {
         newFecGroups.insert( pair<size_t,gateSet*> ( (*onegroup->second->begin())->getValue(), onegroup->second ) );
      }
      fecGroups = newFecGroups;
      //detect fecgroups
      newFecGroups.clear();
      for ( unordered_map<size_t,gateSet*>::iterator onegroup=fecGroups.begin(); onegroup!=fecGroups.end(); ++onegroup )
      {
         unordered_map<size_t,gateSet*> tempFecGroups;
         for ( gateSet::iterator onegate = (*(onegroup->second)).begin(); onegate!=(*(onegroup->second)).end(); ++onegate )
         {
            unordered_map<size_t,gateSet*>::iterator resultGroup = tempFecGroups.find( (*onegate)->getValue() );
            if ( resultGroup != tempFecGroups.end() )
            {
               resultGroup->second->insert( *onegate );
            } 
            else 
            {
               gateSet* newGroup = new gateSet;
               newGroup->insert( *onegate );
               tempFecGroups.insert( pair<size_t,gateSet*> ( (*onegate)->getValue(), newGroup ) );
            }
         }
         //collect valid fec groups
         for ( unordered_map<size_t,gateSet*>::iterator onenewG=tempFecGroups.begin(); onenewG!=tempFecGroups.end(); ++onenewG )
         {
            if ( onenewG->second->size() > 1 )
            {
               newFecGroups.insert( *onenewG );
            } 
            else 
            {
               delete onenewG->second;
            }
         }
      }
      fecGroups = newFecGroups;
      if ( abs(oldSize-newSize) < stopSize )
      {
         break;
      } 
      else 
      {
         oldSize = newSize;
      }
   }
   cout<<64*isim<<" patterns simulated."<<endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
