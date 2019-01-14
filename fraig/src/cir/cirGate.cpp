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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

unsigned CirGate::count = 0;
unsigned CirGate::refMark = 0;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
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
      if (level>0 && fins.size()!=0)
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
      if (level > 0 && !fouts.empty() )
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
         for ( multiset<CirGate*>::const_iterator it = fouts.cbegin(); it != fouts.cend(); ++it )
         {
            bool ifNotVert; 
            for ( unsigned i=0; i!=(*it)->fins.size(); ++i )
            {
               if ( (*it)->fins[i].second == this )
               {  
                  ifNotVert = (*it)->fins[i].first;
                  break;
               }
            }
            //assert( finIt != (*it)->fins.cend() );
            (*it)->reportFanout( level-1, offset+1, ifNotVert );
         }
      } else {}
   }
}

/*
CirGate*
CirGate::dfsTraverseInExp( bool& curStates, stack<unsigned>& curFins, stack<CirGate*>& refGates ) 
{
   if ( curFins.top()<fins.size() && (!this->isMarked()) )
   {
      refGates.push( this );
      CirGate* child = fins[curFins.top()].second();
      curFins.push( 0 );
      curStates = false;
      return child;
   }
   else 
   {
      curStates = true; 
      if (this->isMarked()) curStates = false;
      this->setMarked();
      curFins.pop();
      if ( !curFins.empty() )
      {
         ++curFins.top();
         CirGate* parent = refGates.top();
         refGates.pop();
         return parent;
      }
      else
      {
         return 0;
      }
   }
}
*/

void
CirGate::dfsTraverseToIn( bool ifprint, vector<unsigned> &pathIds ) const 
{
   for ( vector<pair<bool,CirGate*>>::const_iterator it=fins.begin(); it!=fins.end(); ++it)
   {
      if (!(it->second->isMarked()))
      {
         it->second->dfsTraverseToIn( ifprint, pathIds );
      } else {}
   }
   this->setMarked();
   pathIds.push_back( this->getId() );

   if (ifprint && this->getTypeStr()!="UNDEF")
   {
      this->printGate();
      ++count;
   } else {}
}

void
CirGate::dfsTraverseToOut( bool ifprint, vector<unsigned> &pathIds ) const 
{
   for ( multiset<CirGate*>::const_iterator it=fouts.begin(); it!=fouts.end(); ++it)
   {
      if ( (!(*it)->isMarked()) )
      {
         (*it)->dfsTraverseToOut( ifprint, pathIds );
      } else {}
   }
   this->setMarked();
   pathIds.push_back( this->getId() );

   if (ifprint && this->getTypeStr()!="UNDEF")
   {
      this->printGate();
      ++count;
   } else {}
}

void
CirGate::eraseFin( CirGate* target )
{
   for ( vector<pair<bool,CirGate*>>::iterator it=fins.begin(); it!=fins.end(); )//do nothing here )
   {
      if ( it->second==target)
      {
         it = fins.erase(it);
      }
      else
      {
         ++it;
      }
   }
}

void
CirGate::eraseFout( CirGate* target )
{
   fouts.erase( target );
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
