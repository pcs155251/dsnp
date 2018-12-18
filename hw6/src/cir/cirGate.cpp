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
   cout<<"=================================================="<<endl;
   string tmp = "= "+typeString[typeId]+"("+to_string(this->gid)+")"+getNameStr()+", line "+to_string(this->getLineNo());
   tmp = tmp + string(49-tmp.length(), ' ');
   tmp = tmp + "=\n";
   cout<<tmp;
   cout<<"=================================================="<<endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   reportFanin( level, 0, true, false );
   reportFanin( level, 0, true, true );
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   reportFanout( level, 0, true, false );
   reportFanout( level, 0, true, true );
}

void
CirGate::reportFanin(int level, unsigned offset, bool iftrue, bool ifprint) const
{
   assert (level >= 0);
   if (ifprint)
   {
      cout<<string(2*offset,' ');
      if (!iftrue)
      {
         cout<<"!";
      } else {}
      cout<<typeString[typeId]<<" "<<this->getId();

      if (this->getMarked())
      {
         cout<<" (*)"<<endl;
      }
      else 
      {
         cout<<endl;
      }
   } 
   else 
   {
      this->setMarked(false);
   }

   if (level > 0)
   {
      if (ifprint)
      {
         this->setMarked(true);
      } else {}

      for (unsigned i=0; i!=fins.size(); ++i)
      {  
         fins[i].second->reportFanin(level-1, offset+1, fins[i].first, ifprint);
      }
   } else {}
}

void
CirGate::reportFanout(int level, unsigned offset, bool iftrue, bool ifprint) const
{
   assert (level >= 0);
   if (ifprint)
   {
      cout<<string(2*offset,' ');
      if (!iftrue)
      {
         cout<<"!";
      } else {}
      cout<<typeString[typeId]<<" "<<this->getId();

      if (this->getMarked())
      {
         cout<<" (*)"<<endl;
      }
      else 
      {
         cout<<endl;
      }
   }
   else 
   {
      this->setMarked(false);
   }

   if (level > 0)
   {
      if (ifprint)
      {
         this->setMarked(true);
      } else {}

      for (unsigned i=0; i!=fouts.size(); ++i)
      {  
         fouts[i].second->reportFanout(level-1, offset+1, fouts[i].first,ifprint);
      }
   } else {}
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

bool
CirGate::dfsSearch( CirGate* target ) 
{
   if (this==target)
   {
      return true;
   }
   else
   {
      if (fins.size()==0)
      {
         this->setMarked(true);
      } 
      else
      {
         for ( vector<pair<bool,CirGate*>>::iterator it=fins.begin(); it!=fins.end(); ++it)
         {
            //check if next marked
            if (!(it->second->getMarked()))
            {
               if( it->second->dfsSearch(target) )
               {
                  return true;
               } else {}
            }
            else 
            {
            }
         }
         this->setMarked(true);
      }
      return false;
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
   cout<<"["<<count<<"] "<<typeString[typeId]<<" "<<gid;
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
