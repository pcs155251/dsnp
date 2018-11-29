/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0), _isSorted{false} {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { ++_node; return (*this); }
      iterator operator ++ (int) { iterator tmp(*this); ++(*this); return tmp; }
      iterator& operator -- () { --_node; return (*this); }
      iterator operator -- (int) { iterator tmp(*this); --(*this); return tmp; }

      iterator operator + (int i) const { iterator tmp(this->_node+i); return tmp; }
      iterator& operator += (int i) { (*this) = (*this)+i; return (*this); }

      iterator& operator = (const iterator& i) { (*this) = i; return (*this); }

      bool operator != (const iterator& i) const { return !(*this==i); }
      bool operator == (const iterator& i) const { return *(this->_node)==*(i._node); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data+_size); }
   bool empty() const { return (_size==0); }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x)
   { 
      if (_capacity==0)
      {
         _data = new T[1];
         ++_capacity;

         *_data = x;
         ++_size;
      }
      else if (_size!=_capacity)
      {  
         *(_data+_size) = x;
         ++_size;
      }
      else //enlarge _capacity
      {
         T* tmp = new T[2*_size];
         for (size_t i=0; i!=_size; ++i)
         {
            tmp[i] = _data[i];
         }
         delete [] _data;
         _data = tmp;
         _capacity *= 2;
         *(_data+_size) = x;
         ++_size;
      }
      _isSorted = false;
   }
   void pop_front()
   { 
      if (!empty())
      {
         _data[0] = _data[_size-1];
         --_size;
      } else {}
   }
   void pop_back()
   { 
      if (!empty())
      {
         --_size;
      } else {}
   }

   bool erase(iterator pos) 
   { 
     if (empty())
     {
        return false; 
     }
     else
     {
        *(pos._node) = *(_data+_size-1);
        --_size;
        return true;
     }
   }
   bool erase(const T& x) 
   {  
      iterator pos=find(x);
      if (pos==this->end())
      {
         return false; 
      }
      else  
      {
         *(pos._node) = *(_data+_size-1);
         --_size;
         return true;
      }
   }

   iterator find(const T& x) 
   { 
      iterator i=this->begin();
      for (; i!=this->end(); ++i)
      {
          if (*(i._node)==x)
          {
             break;
          }  else {}
      }
      return i;
   }

   void clear() 
   { 
      delete [] _data;
      _data = 0;
      _size = 0;
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const 
   { 
     if (!empty()) {}
     else if (!_isSorted)
     {
       ::sort(_data, _data+_size); 
       _isSorted=true;
     }
     else {}
   }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
