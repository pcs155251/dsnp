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
   if ( !ifsimulated )
   {
   //add all gates in dfsList into one fecGroups
      gateSet* group0 = new gateSet;
      for ( unsigned i=0; i!=dfsList.size(); ++i )
      {
         group0->insert( dfsList[i] );
      }
      fecGroups.insert( group0 );
   } else {}

   //simulate
   unsigned maxSim = 64;
   unsigned oldSize = 0;
   unsigned stopSize = 0;
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

      //detect fecgroups
      fecgs newFecGroups;
      for ( fecgs::iterator onegroup=fecGroups.begin(); onegroup!=fecGroups.end(); ++onegroup )
      {
         fecgs tempFecGroups;
         for ( gateSet::iterator onegate = (*onegroup)->begin(); onegate!=(*onegroup)->end(); ++onegate )
         {
            gateSet tempGroup;
            tempGroup.insert( *onegate );
            fecgs::iterator resultGroup = tempFecGroups.find( &tempGroup );
            if ( resultGroup != tempFecGroups.end() )
            {
               (*resultGroup)->insert( *onegate );
            } 
            else 
            {
               gateSet* newGroup = new gateSet;
               newGroup->insert( *onegate );
               tempFecGroups.insert( newGroup );
            }
         }
         //collect valid fec groups
         for ( fecgs::iterator onenewG=tempFecGroups.begin(); onenewG!=tempFecGroups.end(); ++onenewG )
         {
            if ( (*onenewG)->size() > 1 )
            {
               newFecGroups.insert( *onenewG );
            } 
            else 
            {
               delete (*onenewG);
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
