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
         for ( vector<pair<bool,CirGate*>>::const_iterator it = fouts.cbegin(); it != fouts.cend(); ++it )
         {
            it->second->reportFanout( level-1, offset+1, it->first );
         }
      } else {}
   }
}

bool
CirGate::hasFloatFin() const
{
   for ( vector<pair<bool,CirGate*>>::const_iterator it=fins.begin(); it!=fins.end(); ++it )
   {
      if ( it->second->getTypeStr()=="UNDEF" )
      {
         return true;
      }
   }
   return false;
}

void
CirGate::dfsTraverseToIn( vector<CirGate*> &dfsList )
{
   for ( vector<pair<bool,CirGate*>>::const_iterator it=fins.begin(); it!=fins.end(); ++it)
   {
      if (!(it->second->isMarked()))
      {
         it->second->dfsTraverseToIn( dfsList );
      } else {}
   }
   this->setMarked();
   dfsList.push_back( this );
}

string 
exclamation( bool ifNotVert )
{
   if ( ifNotVert )
   {  return "";
   }
   else
   {
      return "!";
   }
}

void
CirGate::replace( bool ifNotVert, CirGate* substitute )
{
   cout<<"Simplifying: "<< substitute->getId() <<" merging "<<exclamation(ifNotVert)<< this->getId() <<"..."<<endl;
   for ( vector<pair<bool,CirGate*>>::iterator it=this->fins.begin(); it!=this->fins.end(); ++it )
   {
      it->second->eraseAllFout( this );
   }
   for ( vector<pair<bool,CirGate*>>::iterator it=this->fouts.begin(); it!=this->fouts.end(); ++it )
   {
      bool newPhase = ( (!it->first)&&(!ifNotVert) ) || ( it->first && ifNotVert );
      it->second->eraseOneFin( it->first, this );
      it->second->addFin( newPhase, substitute );
      substitute->addFout( newPhase, it->second );
   }
   //delete this;
} 

void
CirGate::merge( CirGate* substitute )
{
   cout<<"Strashing: "<< substitute->getId() <<" merging "<< this->getId() <<"..."<<endl;
   for ( vector<pair<bool,CirGate*>>::iterator it=this->fins.begin(); it!=this->fins.end(); ++it )
   {
      it->second->eraseAllFout( this );
   }
   for ( vector<pair<bool,CirGate*>>::iterator it=this->fouts.begin(); it!=this->fouts.end(); ++it )
   {
      //bool newPhase = ( (!it->first)&&(!ifNotVert) ) || ( it->first && ifNotVert );
      it->second->eraseOneFin( it->first, this );
      it->second->addFin( it->first, substitute );
      substitute->addFout( it->first, it->second );
   }
   //delete this;
} 

CirGate*
CirGate::trivialOpt( CirGate* constGate )
{
   //check same fanins
   if ( this->fins.size() < 2)
   {
      //do nothing for PI, PO
      return 0;
   }
   else if ( this->fins[0].second == this->fins[1].second )
   {
      if (this->fins[0].first == this->fins[1].first)
      {
         this->replace( this->fins[0].first, this->fins[0].second );
      }
      else 
      {
         this->replace( true, constGate );
      }
      return this;
   }
   else if ( this->fins[0].second == constGate )
   {
      if ( this->fins[0].first == true )
      {
         this->fins[1].second->eraseAllFout( this );
         this->replace( true, constGate );
      }
      else
      {
         constGate->eraseAllFout( this );
         this->replace( this->fins[1].first, this->fins[1].second );
      }
      return this;
   } 
   else if ( this->fins[1].second == constGate )
   {
      if ( this->fins[1].first == true )
      {
         this->fins[0].second->eraseAllFout( this );
         this->replace( true, constGate );
      }
      else
      {
         constGate->eraseAllFout( this );
         this->replace( this->fins[0].first, this->fins[0].second );
      }
      return this;
   }
   else 
   {
      //no trivial optimization, do nothing
      return 0;
   }
}

void
CirGate::dfsTraverseToOut( bool ifprint, vector<unsigned> &pathIds ) const 
{
   for ( vector<pair<bool,CirGate*>>::const_iterator it=fouts.begin(); it!=fouts.end(); ++it)
   {
      if ( (!it->second->isMarked()) )
      {
         it->second->dfsTraverseToOut( ifprint, pathIds );
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
CirGate::eraseAllFin( CirGate* target )
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
CirGate::eraseAllFout( CirGate* target )
{
   for ( vector<pair<bool,CirGate*>>::iterator it=fouts.begin(); it!=fouts.end(); )//do nothing here )
   {
      if ( it->second==target)
      {
         it = fouts.erase(it);
      }
      else
      {
         ++it;
      }
   }
   //fouts.erase( target );
}

void
CirGate::eraseOneFin( bool ifNotVert, CirGate* target )
{
   for ( vector<pair<bool,CirGate*>>::iterator it=fins.begin(); it!=fins.end(); )//do nothing here )
   {
      if ( it->first==ifNotVert && it->second==target )
      {
         it = fins.erase(it);
         break;
      }
      else
      {
         ++it;
      }
   }
}

void
CirGate::eraseOneFout( bool ifNotVert, CirGate* target )
{
   for ( vector<pair<bool,CirGate*>>::iterator it=fouts.begin(); it!=fouts.end(); )//do nothing here )
   {
      if ( it->first==ifNotVert && it->second==target )
      {
         it = fouts.erase(it);
         break;
      }
      else
      {
         ++it;
      }
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
