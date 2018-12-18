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
unsigned CirGate::count = 0;

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
         this->printGate();
         ++count;
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
      this->printGate();
      ++count;
   }
}

/**************************************/
/*   class CirPiGate member functions */
/**************************************/
void
CirPiGate::printGate() const
{
   cout<<"["<<count<<"] "<<typeString[typeId]<<"  "<<gid;
   if (name!="")
   {
      cout<<" ("<<name<<")";
   } else {}
   cout<<endl;
}

/**************************************/
/*   class CirPoGate member functions */
/**************************************/
void
CirPoGate::printGate() const
{
   cout<<"["<<count<<"] "<<typeString[typeId]<<"  "<<gid;
   for (unsigned i=0; i!=fins.size(); i++)
   {
      cout<<" ";
      if (!(fins[i].second->getTypeId())) 
      { 
         cout<<"*";
      } else {}
      if (fins[i].first==false) 
      { 
         cout<<"!";
      } else {}
      cout<<fins[i].second->getId();
   }
   if (name!="")
   {
      cout<<" ("<<name<<")";
   } else {}
   cout<<endl;
}

/**************************************/
/*   class CirAigGate member functions*/
/**************************************/
void
CirAigGate::printGate() const
{
   cout<<"["<<count<<"] "<<typeString[typeId]<<" "<<gid<<" ";
   for (unsigned i=0; i!=fins.size(); i++)
   {
      if (!(fins[i].second->getTypeId())) 
      { 
         cout<<"*";
      } else {}
      if (fins[i].first==false) 
      { 
         cout<<"!";
      } else {}
      cout<<fins[i].second->getId()<<" ";
   }
   cout<<endl;
}

/**************************************/
/*   class CirConGate member functions*/
/**************************************/
void
CirConGate::printGate() const
{
   cout<<"["<<count<<"] "<<typeString[typeId]<<gid<<endl;
}

/**************************************/
/*   class CirFloGate member functions*/
/**************************************/
void
CirFloGate::printGate() const
{
}
