/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <algorithm>
#include <sstream>

using namespace std;

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
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
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

//for sorting gate
bool
compareGateId( const CirGate* g0, const unsigned g1Id )
{
   return ( g0->getId() < g1Id );
}

bool
compareGateGate( const CirGate* g0, const CirGate* g1 )
{
   return ( g0->getId() < g1->getId() );
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
CirMgr::~CirMgr()
{
   for (unsigned i=0; i!=gates.size(); ++i)
   {
      delete gates[i];
   }
}

CirGate* 
CirMgr::getGate(unsigned gid) 
{ 
   //TODO make sure is sorted before getGate
   //TODO may have problem when declaring as const functions
   vector<CirGate*>::iterator pos = lower_bound( gates.begin(), gates.end(), gid, compareGateId);
   if ( pos==gates.end() || (*pos)->getId()!=gid )
   {
      return 0;
   }
   else
   {
      return (*pos);
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

   gates.reserve( maxN );
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
      gates.push_back( tmp );
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
      gates.push_back( tmp );
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
      gates.push_back( tmp );
      aigs.push_back( tmp );

      //detect constant gate
      if (in1Li[i]/2==0||in2Li[i]/2==0)
      {
         gates.push_back( new CirConGate() );
      } else {}
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
      else
      {
      }
   }

   //detected undefined gate
   sort( gates.begin(), gates.end(), compareGateGate );
   for (unsigned i=0; i!=aigs.size(); ++i)
   { 
      if (getGate(in1Li[i]/2)==0)
      {
         tmp = new CirFloGate(0,in1Li[i]/2);
         gates.push_back( tmp );
         floats.push_back( tmp );
      }
      else {}
      if (getGate(in2Li[i]/2)==0)
      {
         tmp = new CirFloGate(0,in2Li[i]/2);
         gates.push_back( tmp );
         floats.push_back( tmp );
      }
      else {}
      if (getGate(in1Li[i]/2==0||in2Li[i]/2==0))
      {
         floatfins.push_back( aigs[i] );
      } else {}
   }
    

   //connection
   sort( gates.begin(), gates.end(), compareGateGate );
   //connect po
   for (unsigned i=0; i!=pouts.size(); ++i)
   {
      unsigned addId = toPoLi[i]/2;
      bool ifnoRevert = !((toPoLi[i])%2);
      pouts[i]->addFin( ifnoRevert, getGate(addId) );
      getGate(addId)->addFout( ifnoRevert, getGate(pouts[i]->getId()) );
   }
   //connect and
   for (unsigned i=0; i!=aigs.size(); ++i)
   {
      unsigned f1Id = in1Li[i]/2;
      bool f1nonRevert = !((in1Li[i])%2);
      unsigned f2Id = in2Li[i]/2;
      bool f2nonRevert = !((in2Li[i])%2);
      aigs[i]->addFin( f1nonRevert, getGate(f1Id) );
      aigs[i]->addFin( f2nonRevert, getGate(f2Id) );
      getGate(f1Id)->addFout( f1nonRevert, getGate(aigs[i]->getId()) );
      getGate(f2Id)->addFout( f2nonRevert, getGate(aigs[i]->getId()) );
   }

   //find bad gates
   /*
   for (size_t i=0; i!=gates.size(); ++i)
   {
      gates[i]->setMarked(false);
   }
   for (unsigned i=0; i!=gates.size(); ++i)
   {
      for (unsigned ii=0; ii!=pins.size(); ++ii)
      {
        //cout<<!(gates[i]->dfsSearch( pins[ii]))<<endl;
        if ( (gates[i]->dfsSearch( pins[ii]))==false )
        {
           floatfins.push_back(gates[i]);
        } else {}
      }
   }
   sort( floatfins.begin(), floatfins.end(), compareGateGate );

   for (size_t i=0; i!=gates.size(); ++i)
   {
      gates[i]->setMarked(false);
   }
   for (unsigned i=0; i!=gates.size(); ++i)
   {
      for (unsigned ii=0; ii!=pouts.size(); ++ii)
      {
        //cout<<!(pouts[ii]->dfsSearch( gates[i]))<<endl;
        if ( (pouts[ii]->dfsSearch( gates[i]))==false )
        {
           notused.push_back(gates[i]);
        } else {}
      }
   }
   sort( notused.begin(), notused.end(), compareGateGate );
   */

   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************







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

//TODO non const iterator?
void
CirMgr::printNetlist() const 
{
   //reset count and mark first;
   CirGate::count=0;
   if ( gates.size()!=0 )
   {
      gates[0]->setRefMark();
   } else {}

   cout<<endl;
   for (size_t i=0; i!=pouts.size(); ++i)
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
   cout << "Gates with floating fanin(s):";
   for (unsigned i=0; i!=floatfins.size(); ++i)
   {
      cout<<" "<<floatfins[i]->getId();
   }
   cout << endl;

   cout << "Gates defined but not used :";
   for (unsigned i=0; i!=notused.size(); ++i)
   {
      cout<<" "<<notused[i]->getId();
   }
   cout << endl;
}

void
CirMgr::writeAag(ostream& outfile) const
{
}
