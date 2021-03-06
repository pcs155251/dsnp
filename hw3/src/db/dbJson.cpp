/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO done: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   string tmp;
   is>>tmp;
   while (is>>tmp){
     if (tmp[0]=='"'){
       string key( tmp.begin()+1, tmp.end()-1 );
       is>>tmp;
       is>>tmp;
       int value = stoi( tmp );
       j.add( DBJsonElem( key, value ) );
     }
     else {
       break;
     }
   }

   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO done
   os << "{" << endl;
   for ( size_t i=0; i!=j._obj.size()-1; i++){
     os << "  " << j._obj[i] << "," << endl;
   }
   os << "  " << j._obj[j._obj.size()-1] << endl;
   os << "}" << endl;
   os << "Total JSON elements: " << j.size() << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO done
   _obj.clear();
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO done
   if (find(elm.key())!=_obj.size()){
     return false;
   }
   else {
     _obj.push_back( elm );
   }
   return true;
}

// return the position of the key, if not found return size of vector
size_t 
DBJson::find(const string &key) const
{
   //TODO done
   for ( size_t i=0; i!=_obj.size(); i++){
     if (_obj[i].key()==key ){
       return i;
     }
   }
   return _obj.size();
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO done
   if (_obj.empty()){
     return NAN;
   }
   else {
     return float(sum())/float(_obj.size());
   }
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO done
   int maxN = INT_MIN;
   if (_obj.size()==0){
     idx = _obj.size();
   }
   else {
     for (size_t i=0; i!=_obj.size(); i++){
       if (_obj[i].value()>maxN){
         maxN = _obj[i].value();
         idx = i;
       }
       else {}
     }
   }
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO done
   int minN = INT_MAX;
   if (_obj.empty()){
     idx = _obj.size();
   }
   else {
     for (size_t i=0; i!=_obj.size(); i++){
       if (_obj[i].value()<minN){
         minN = _obj[i].value();
         idx = i;
       }
       else {}
     }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   for (size_t i=0; i!=_obj.size(); i++){
     s += _obj[i].value();
   }
   return s;
}
