/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) 
   { 
      _data.push_back( d );
      size_t current_index = _data.size()-1;
      size_t mother_index = (current_index-1)/2;
      while ( (current_index>0)&&( d < _data[mother_index]) )
      {
         _data[current_index] = _data[mother_index];
         current_index = mother_index;
         mother_index = (current_index-1)/2;
      }
      _data[current_index] = d;
   }
   void delMin() { delData(0); }
   void delData(size_t i) 
   {
      Data last = _data[_data.size()-1];
      _data.pop_back();
      size_t current_index = i;
      size_t smlchild_index = 2*current_index+1; //point to left
      while ( smlchild_index < _data.size() )
      {
         if ( smlchild_index+1 < _data.size() )   
            if ( _data[smlchild_index+1] < _data[smlchild_index] )
               ++smlchild_index;
         if ( last < _data[smlchild_index] )
            break;
         _data[current_index] = _data[smlchild_index];
         current_index = smlchild_index;
         smlchild_index = 2*current_index+1;
      }
      _data[current_index] = last;
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
