/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <sstream>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

const bool IFDEBUG = false;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*    for sorting gates                                       */
/**************************************************************/
bool
CompareGate::operator()(const CirGate* lgate, const CirGate* rgate) const
{
   return ( lgate->getId() < rgate->getId() );
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::~CirMgr()
{
   for ( map<unsigned,CirGate*>::iterator it=gates.begin(); it!=gates.end(); ++it)
   {
      delete it->second;
   }
}

CirGate* 
CirMgr::getGate(unsigned gid) const 
{ 
   //TODO make sure is sorted before getGate
   map<unsigned,CirGate*>::const_iterator pos = gates.find( gid );
   if ( pos==gates.end() )
   {
      return 0;
   }
   else
   {
      return pos->second;
   }
}

bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream inFile( fileName );
   if (inFile.fail())
   {
      cerr << "Cannot open design \""<<fileName<<"\"!!"<<endl;
      return false;
   } else {}

   string tmpstring;
   //header line
   getline( inFile, tmpstring );
   ++lineNo;
   istringstream iss(tmpstring );

   getline( iss, tmpstring, ' ');//aag
   unsigned maxN, inN, lN, outN, andN;
   getline( iss, tmpstring, ' ');//M
   maxN = stoi( tmpstring );
   getline( iss, tmpstring, ' ');//I
   inN = stoi( tmpstring );
   getline( iss, tmpstring, ' ');//L
   lN = stoi( tmpstring );
   getline( iss, tmpstring, ' ');//O
   outN = stoi( tmpstring );
   getline( iss, tmpstring, ' ');//A
   andN = stoi( tmpstring );

   if (IFDEBUG) cerr<<"adding const gate"<<endl;
   gates.insert( pair<unsigned,CirGate*> (0,new CirConGate()) );
   pins.reserve( inN );
   pouts.reserve( outN );
   aigs.reserve( andN );

   CirGate* tmp;
   //in gates
   for (unsigned i=0; i!=inN; i++)
   {
      unsigned lit;
      getline( inFile, tmpstring );
      lit = stoi( tmpstring );
      tmp = new CirPiGate(lineNo+1,lit/2);
      if (IFDEBUG) cerr<<"pi "<<lit/2<<endl;
      gates.insert( pair<unsigned,CirGate*> (lit/2,tmp) );
      pins.push_back( tmp );
      ++lineNo;
   }
   //out gates
   vector<unsigned> toPoLi(outN);
   for (unsigned i=0; i!=outN; i++)
   {
      unsigned lit;
      getline( inFile, tmpstring );
      lit = stoi( tmpstring );
      tmp = new CirPoGate(lineNo+1,maxN+i+1);
      if (IFDEBUG) cerr<<"po "<<maxN+i+1<<endl;
      gates.insert( pair<unsigned,CirGate*> (maxN+i+1,tmp) );
      pouts.push_back( tmp );
      toPoLi[i] = lit;
      ++lineNo;
   }
   //and gates
   vector<unsigned> in1Li(andN);
   vector<unsigned> in2Li(andN);
   for (unsigned i=0; i!=andN; i++)
   {
      unsigned lit;
      getline( inFile, tmpstring );
      istringstream isand(tmpstring);
      getline( isand, tmpstring, ' ');
      lit = stoi( tmpstring );
      getline( isand, tmpstring, ' ');
      in1Li[i] = stoi( tmpstring );
      getline( isand, tmpstring, ' ');
      in2Li[i] = stoi( tmpstring );
      tmp = new CirAigGate(lineNo+1,lit/2);
      if (IFDEBUG)cerr<<"aig "<<lit/2<<endl;
      gates.insert( pair<unsigned,CirGate*> (lit/2,tmp) );
      aigs.push_back( tmp );

      ++lineNo;
   }
   //set name
   while (getline( inFile, tmpstring) )
   {
      istringstream iname(tmpstring);
      getline( iname, tmpstring, ' ');
      if (tmpstring[0]=='i')
      {
         string posStr( tmpstring.begin()+1, tmpstring.end() );
         unsigned pos = stoi( posStr );
         getline( iname, tmpstring);
         pins[pos]->setNameStr( tmpstring );
      }
      else if (tmpstring[0]=='o')
      {
         string posStr( tmpstring.begin()+1, tmpstring.end() );
         unsigned pos = stoi( posStr );
         getline( iname, tmpstring);
         pouts[pos]->setNameStr( tmpstring );
      }
      else{}
   }

   //detected undefined gate
   if (IFDEBUG)
   {
      cerr<<"L1: ";
      for (unsigned i=0; i!=in1Li.size(); ++i) cerr<<in1Li[i]/2<<", ";
      cerr<<endl;
      cerr<<"L2: ";
      for (unsigned i=0; i!=in2Li.size(); ++i) cerr<<in2Li[i]/2<<", ";
      cerr<<endl;

      cerr<<"gate: ";
      for (auto it=gates.begin(); it!=gates.end(); ++it ) cerr<<it->first<<", ";
      cerr<<endl;
   }
      
   if (IFDEBUG) cerr<<"aig: ";
   for (unsigned iaig=0; iaig!=aigs.size(); ++iaig)
   { 
      if (IFDEBUG) cerr<<aigs[i]->getId()<<endl;
      bool ifFloatFin = false;

      unsigned id1 = in1Li[iaig]/2;
      CirGate* gate1 = getGate(id1);
      if (gate1==0)
      {
         tmp = new CirFloGate(0,id1);
         if (IFDEBUG) cerr<<"undef1 "<<id1<<endl;
         gates.insert( pair<unsigned,CirGate*> (id1,tmp) );
         floats.push_back( tmp );
         ifFloatFin = true;
      } 
      else if (gate1->getTypeStr()=="UNDEF")
      {
         ifFloatFin = true;
      } else {}

      unsigned id2 = in2Li[iaig]/2;
      CirGate* gate2 = getGate(id2);
      if (gate2==0)
      {
         tmp = new CirFloGate(0,id2);
         if (IFDEBUG) cerr<<"undef2 "<<id2<<endl;
         gates.insert( pair<unsigned,CirGate*> (id2,tmp) );
         floats.push_back( tmp );
         ifFloatFin = true;
      }
      else if (gate2->getTypeStr()=="UNDEF")
      {
         ifFloatFin = true;
      } else {}

      if (ifFloatFin)
      {
         floatfins.insert( aigs[i] );
      } else {}
   }

   //connection
   //connect po
   for (unsigned i=0; i!=pouts.size(); ++i)
   {
      unsigned addId = toPoLi[i]/2;
      bool ifnoRevert = !((toPoLi[i])%2);
      if ( getGate(addId)!= 0)
      {
         pouts[i]->addFin( ifnoRevert, getGate(addId) );
         getGate(addId)->addFout( getGate(pouts[i]->getId()) );
      }
      else
      {
         tmp = new CirFloGate(0,addId);
         gates.insert( pair<unsigned,CirGate*> (addId,tmp) );
         floats.push_back( tmp );
         pouts[i]->addFin( ifnoRevert, tmp );
         tmp->addFout( getGate(pouts[i]->getId()) );
      }
   }
   //connect and gates
   for (unsigned i=0; i!=aigs.size(); ++i)
   {
      unsigned f1Id = in1Li[i]/2;
      bool f1nonRevert = !((in1Li[i])%2);
      unsigned f2Id = in2Li[i]/2;
      bool f2nonRevert = !((in2Li[i])%2);
      aigs[i]->addFin( f1nonRevert, getGate(f1Id) );
      aigs[i]->addFin( f2nonRevert, getGate(f2Id) );
      getGate(f1Id)->addFout( getGate(aigs[i]->getId()) );
      getGate(f2Id)->addFout( getGate(aigs[i]->getId()) );
   }

   //find bad gates
   for ( map<unsigned,CirGate*>::iterator it=gates.begin(); it!=gates.end(); ++it )
   {
      bool ifFindPin = false;
      for (unsigned ii=0; ii!=pins.size(); ++ii)
      {
        CirGate::setRefMark();
        if (it->second->dfsSearch(pins[ii]))
        {
          ifFindPin = true;
          break;
        } else{}
      }
      if ( !ifFindPin && it->second->getTypeStr()!="UNDEF" )
      {
         CirGate::setRefMark();
         if ( !(it->second->dfsSearch(gates.begin()->second)) )//const can serve as pin here
         {
            floatfins.insert(it->second);
         } else {}
      } else {}
   }

   for (unsigned i=0; i!=pins.size(); ++i)
   {
      if (pins[i]->noFanout())
      {
         notused.insert(pins[i]);
      } else {}
   }
   for (unsigned i=0; i!=aigs.size(); ++i)
   {
      if (aigs[i]->noFanout())
      {
         notused.insert(aigs[i]);
      } else {}
   }

   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout<<endl<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  "<<setw(5)<<left<<"PI"<<setw(9)<<right<<pins.size()<<endl;
   cout<<"  "<<setw(5)<<left<<"PO"<<setw(9)<<right<<pouts.size()<<endl;
   cout<<"  "<<setw(5)<<left<<"AIG"<<setw(9)<<right<<aigs.size()<<endl;
   cout<<"------------------"<<endl;
   cout<<"  "<<setw(5)<<left<<"Total"<<setw(9)<<right<<(pins.size()+pouts.size()+aigs.size())<<endl;
}

void
CirMgr::printNetlist() const
{
   //reset count and mark first;
   CirGate::count=0;
   CirGate::setRefMark();

   cout<<endl;
   for (unsigned i=0; i!=pouts.size(); ++i)
   {
      pouts[i]->dfsTraverse();
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for (unsigned i=0; i!=pins.size(); ++i)
   {
      cout<<" "<<pins[i]->getId();
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for (unsigned i=0; i!=pouts.size(); ++i)
   {
      cout<<" "<<pouts[i]->getId();
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   if (!floatfins.empty())
   {
      cout << "Gates with floating fanin(s):";
      for (set<CirGate*,CompareGate>::iterator it=floatfins.begin(); it!=floatfins.end(); ++it )
      {
         cout<<" "<<(*it)->getId();
      }
      cout << endl;
   } else {}

   if (!notused.empty())
   {
      cout << "Gates defined but not used  :";
      for (set<CirGate*,CompareGate>::iterator it=notused.begin(); it!=notused.end(); ++it )
      {
         cout<<" "<<(*it)->getId();
      }
      cout << endl;
   } else {}
}

void
CirMgr::printFECPairs() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

