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

unsigned CirGate::count = 0;
unsigned CirGate::refMark = 0;

void
CirGate::reportGate() const
{
   cout<<"=================================================="<<endl;
   string allName = (getNameStr()=="")? "" : "\""+getNameStr()+"\"";
   string tmp = "= "+getTypeStr()+"("+to_string(this->gid)+")"+allName+", line "+to_string(this->getLineNo());
   tmp = tmp + string(49-tmp.length(), ' ');
   tmp = tmp + "=\n";
   cout<<tmp;
   cout<<"=================================================="<<endl;
}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
   setRefMark();
   reportFanin( level, 0, true );
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   setRefMark();
   reportFanout( level, 0, true );
}

void
CirGate::reportFanin(int level, unsigned offset, bool iftrue ) const
{
   assert (level >= 0);
   cout<<string(2*offset,' ');
   if (!iftrue)
   {
      cout<<"!";
   } else {}

   cout<<this->getTypeStr()<<" "<<this->getId();

   if ( this->isMarked() )
   { 
      if (level > 0)
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
      cout<<endl;
      if ( level > 0 )
      {
         this->setMarked();
         for (unsigned i=0; i!=fins.size(); ++i)
         {  
            fins[i].second->reportFanin(level-1, offset+1, fins[i].first );
         }
      } else {}
   }
}

void
CirGate::reportFanout(int level, unsigned offset, bool iftrue ) const
{
   assert (level >= 0);
   cout<<string(2*offset,' ');
   if (!iftrue)
   {
      cout<<"!";
   } else {}
   cout<<this->getTypeStr()<<" "<<this->getId();

   if ( this->isMarked() )
   {
      if (level > 0)
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
      cout<<endl;
      if (level > 0)
      {
         this->setMarked();
         for (unsigned i=0; i!=fouts.size(); ++i)
         {  
            fouts[i].second->reportFanout(level-1, offset+1, fouts[i].first );
         }
      } else {}
   }
}


void
CirGate::dfsTraverse() 
{
   for ( vector<pair<bool,CirGate*>>::iterator it=fins.begin(); it!=fins.end(); ++it)
   {
      if (!(it->second->isMarked()))
      {
         it->second->dfsTraverse();
      } else {}
   }
   this->setMarked();
   if (this->getTypeStr()!="UNDEF")
   {
      this->printGate();
      ++count;
   } else {}
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
         this->setMarked();
      } 
      else
      {
         for ( vector<pair<bool,CirGate*>>::iterator it=fins.begin(); it!=fins.end(); ++it)
         {
            //check if next marked
            if (!(it->second->isMarked()))
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
         this->setMarked();
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
   cout<<"["<<count<<"] "<<this->getTypeStr()<<"  "<<gid;
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
   cout<<"["<<count<<"] "<<this->getTypeStr()<<"  "<<gid;
   for (unsigned i=0; i!=fins.size(); i++)
   {
      cout<<" ";
      if (fins[i].second->getTypeStr()=="UNDEF") 
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
   cout<<"["<<count<<"] "<<this->getTypeStr()<<" "<<gid;
   for (unsigned i=0; i!=fins.size(); i++)
   {
      cout<<" ";
      if (fins[i].second->getTypeStr()=="UNDEF") 
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
   cout<<"["<<count<<"] "<<this->getTypeStr()<<gid<<endl;
}

/**************************************/
/*   class CirFloGate member functions*/
/**************************************/
void
CirFloGate::printGate() const
{
}
