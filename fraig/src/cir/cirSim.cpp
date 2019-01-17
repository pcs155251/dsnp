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
         if ( dfsList[i]->getTypeStr()!="PO"&&dfsList[i]->getTypeStr()!="PI" )
         {
            group0->insert( dfsList[i] );
         } else {}
      }
      group0->insert( gates[0] );
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
   ifsimulated  = true;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   if ( !ifsimulated )
   {
   //add all gates in dfsList into one fecGroups
      gateSet* group0 = new gateSet;
      for ( unsigned i=0; i!=dfsList.size(); ++i )
      {
         if ( dfsList[i]->getTypeStr()!="PO"&&dfsList[i]->getTypeStr()!="PI" )
         {
            group0->insert( dfsList[i] );
         } else {}
      }
      group0->insert( gates[0] );
      fecGroups.insert( group0 );
   } else {}
   //printFECPairs();

   //load patterns
   vector<string> patterns( pins.size() );
   char s;
   unsigned iii=0;
   while( patternFile.get(s) )
   {
      if ( s=='0'||s=='1')
      {
         patterns[iii].push_back(s);
         ++iii;
         if (iii==patterns.size())
         {
            iii=0;
         } else {}
      }
   }

   //simulate
   unsigned maxSim = patterns[0].size()/64;
   if ( patterns[0].size()%64!=0 )
   { 
      maxSim++;
   } else {}
   for ( unsigned isim=0; isim!=maxSim; isim++ )
   {
      for ( unsigned i=0; i!=pins.size(); ++i )
      {
         string tmpstr = patterns[i].substr( 64*isim, 64 );
         if ( tmpstr.size() < 64 )
         {
            tmpstr.append( '0', 64-tmpstr.size() );
         } else {}

         pins[i]->setValue( size_t(stoul(tmpstr, 0, 2)) );
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
            assert( newFecGroups.find( *onenewG )==newFecGroups.end() );
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
      printFECPairs();
   }

   cout<<patterns[0].size()<<" patterns simulated."<<endl;
   ifsimulated  = true;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
