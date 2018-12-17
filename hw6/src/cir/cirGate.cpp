/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/

vector<string> CirGate::typeString = vector<string> { "UNDEF", "PI", "PO", "AIG", "CONST" };

void
CirGate::reportGate() const
{
   //TODO add right =
   cout<<"================================================== "<<endl;
   cout<<"= "<<typeString[typeId]<<"("<<this->gid<<")"<<getNameStr()<<", line "<<this->getLineNo()<<endl;
   cout<<"================================================== "<<endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   //for (int current=0; current<=level; ++current)
   //{
   //cout<<typeString[typeId]<<" "<<getLineNo<<endl;
   //}
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
}

void
CirGate::dfsTraverse() 
{
   if (fins.size()==0)
   {
      this->setMarked(true);
      if (this->typeId!=0)
      {
         cout<<this->getId()<<endl;
      }
      else {}
   } 
   else
   {
      for ( vector<pair<bool,CirGate*>>::iterator it=fins.begin(); it!=fins.end(); ++it)
      {
         //check if next marked
         if (!(it->second->getMarked()))
         {
            it->second->dfsTraverse();
         }
         else 
         {
         }
      }
      this->setMarked(true);
      cout<<this->getId()<<endl;
   }
}

/**************************************/
/*   class CirPiGate member functions */
/**************************************/
void
CirPiGate::printGate() const
{
}

/**************************************/
/*   class CirPoGate member functions */
/**************************************/
void
CirPoGate::printGate() const
{
}

/**************************************/
/*   class CirAigGate member functions*/
/**************************************/
void
CirAigGate::printGate() const
{
}

/**************************************/
/*   class CirConGate member functions*/
/**************************************/
void
CirConGate::printGate() const
{
}

/**************************************/
/*   class CirFloGate member functions*/
/**************************************/
void
CirFloGate::printGate() const
{
}
