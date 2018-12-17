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
compareGateId( const CirGate* g0, unsigned g1Id )
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
   //TODO
   //should read all gate and sort here
   cout<<"readCircuit, test"<<endl;
   gates.reserve(23);
   
   CirGate* tmp;
   tmp = new CirPiGate(2,1);
   gates.push_back( tmp );
   pins.push_back( tmp );

   tmp = new CirPiGate(3,2);
   gates.push_back( tmp );
   pins.push_back( tmp );

   tmp = new CirPiGate(4,6);
   gates.push_back( tmp );
   pins.push_back( tmp );

   tmp = new CirPiGate(5,7);
   gates.push_back( tmp );
   pins.push_back( tmp );

   tmp = new CirPoGate(6,24); //connect to 22
   gates.push_back( tmp );
   pouts.push_back( tmp );
   tmp = new CirPoGate(7,25); //connect to 23
   gates.push_back( tmp );
   pouts.push_back( tmp );

   
   tmp = new CirAigGate( 8,10);
   gates.push_back( tmp );
   aigs.push_back( tmp );
   tmp = new CirAigGate( 9,11);
   gates.push_back( tmp );
   aigs.push_back( tmp );
   tmp = new CirAigGate(10,16);
   gates.push_back( tmp );
   aigs.push_back( tmp );
   tmp = new CirAigGate(11,22);
   gates.push_back( tmp );
   aigs.push_back( tmp );
   tmp = new CirAigGate(12,19);
   gates.push_back( tmp );
   aigs.push_back( tmp );
   tmp = new CirAigGate(13,23);
   gates.push_back( tmp );
   aigs.push_back( tmp );
   
   tmp = new CirConGate(8);
   gates.push_back( tmp );

   tmp = new CirFloGate(12,15);
   gates.push_back( tmp );
   floats.push_back( tmp );

   sort( gates.begin(), gates.end(), compareGateGate );

   //start connecting
   //connecting from out
   getGate(22)->addFout(getGate(24));
   getGate(23)->addFout(getGate(25));
   getGate(24)->addFin(false,getGate(22));
   getGate(25)->addFin(false,getGate(23));
   //connecting and
   getGate(1)->addFout(getGate(10));
   getGate(0)->addFout(getGate(10));
   getGate(10)->addFin(true,getGate(1));
   getGate(10)->addFin(false,getGate(0));

   getGate(1)->addFout(getGate(11));
   getGate(6)->addFout(getGate(11));
   getGate(11)->addFin(false,getGate(1));
   getGate(11)->addFin(true,getGate(6));

   getGate(2)->addFout(getGate(16));
   getGate(11)->addFout(getGate(16));
   getGate(16)->addFin(true,getGate(2));
   getGate(16)->addFin(false,getGate(11));

   getGate(10)->addFout(getGate(22));
   getGate(16)->addFout(getGate(22));
   getGate(22)->addFin(false,getGate(10));
   getGate(22)->addFin(false,getGate(16));

   getGate(15)->addFout(getGate(19));
   getGate(7)->addFout(getGate(19));
   getGate(19)->addFin(false,getGate(15));
   getGate(19)->addFin(true,getGate(7));

   getGate(16)->addFout(getGate(23));
   getGate(19)->addFout(getGate(23));
   getGate(23)->addFin(false,getGate(16));
   getGate(23)->addFin(false,getGate(19));

   getGate(1)->setNameStr("1GAT");
   getGate(2)->setNameStr("2GAT");
   getGate(6)->setNameStr("6GAT");
   getGate(7)->setNameStr("7GAT");
   getGate(22)->setNameStr("22GAT$PO");
   getGate(23)->setNameStr("23GAT$PO");

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
   cout<<"Circuit Statistics"<<endl;
   cout<<"=================="<<endl;
   cout<<"  "<<setw(5)<<left<<"PI"<<setw(9)<<right<<pins.size()<<endl;
   cout<<"  "<<setw(5)<<left<<"PO"<<setw(9)<<right<<pouts.size()<<endl;
   cout<<"  "<<setw(5)<<left<<"AIG"<<setw(9)<<right<<aigs.size()<<endl;
   cout<<"------------------"<<endl;
   cout<<"  "<<setw(5)<<left<<"Total"<<setw(9)<<right<<(pins.size()+pouts.size()+aigs.size())<<endl;
}

//TODO non const iterator?
void
CirMgr::printNetlist() 
{
   for (vector<CirGate*>::iterator it=pouts.begin(); it!=pouts.end(); ++it)
   {
      (*it)->dfsTraverse();
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
}
