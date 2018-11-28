/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList(): _isSorted{false} {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator tmp(_node); ++(*(this)); return tmp; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator tmp(_node); --(*(this)); return tmp; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator == (const iterator& i) const { return ((this->_node->_data==i._node->_data)&&(this->_node->_prev==i._node->_prev)&&(this->_node->_next&&i._node->_next)); }
      bool operator != (const iterator& i) const { return !(*(this)==i); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }
   bool empty() const { return (_head->_prev==_head&&_head->_next==_head); }
   size_t size() const 
   {  
      size_t count=0;
      for (iterator i=this->begin(); i!=this->end(); ++i){
        ++count;
      }
      return count; 
   }
   void push_back(const T& x)
   { 
      DListNode<T>* back = new DListNode<T>( x, _head->_prev, _head );
      _head->_prev->_next = back;
      _head->_prev = back;
      _isSorted = false;
   }
   void pop_front() 
   { 
      if (!empty())
      {
        DListNode<T>* front = _head->_next;
        _head->_next = front->_next;
        _head->_next->_prev = _head;
        delete front;
        _isSorted = false;
      }
      else {}
   }
   void pop_back() 
   { 
      if (!empty())
      {
        DListNode<T>* back = _head->_prev;
        _head->_prev = back->_prev;
        _head->_prev->_next = _head;
        delete back;
        _isSorted = false;
      }
      else {}
   }

   // return false if nothing to erase
   bool erase(iterator pos)
   { 
     if (!empty())
     {
        pos._node->_prev->_next = pos._node->_next;
        pos._node->_next->_prev = pos._node->_prev;
        delete pos._node;
        return true; 
     }
     else
     {
        return false; 
     }
   }
   bool erase(const T& x)
   {
      iterator i = find(x);
      if (i!=this->end())
      {
         erase(i);
         return true;
      }
      else 
      {
         return false;
      }
   }

   iterator find(const T& x)
   {  
      iterator i;
      for (i=this->begin(); i!=this->end(); ++i){
         if (i._node->_data==x){
           break;
         } else {}
      }
      return i; 
   }

   void clear() // delete all nodes except for the dummy node
   {
      if (!empty())
      {  
         for (iterator i=this->begin(); i!=this->end();)
         {
            ++i;
            delete i._node->_prev;
         }
         _head->_prev = _head->_next = _head;
      } else {}
   }

   void sort() 
   { 
      if (_isSorted)
      {
      }  
      else 
      {
         iterator min = this->begin();
         for (iterator i=this->begin(); i!=this->end(); ++i)
         {
            if(i._node->_data < min._node->_data)
            {
               min = i;
            }
         }
         push_back( min._node->_data );
         erase( min );
         iterator ref( _head->_prev );
         
         while (ref!=this->begin())
         {
            min = this->begin();
            for (iterator i=this->begin(); i!=ref; ++i)
            {
               if(i._node->_data < min._node->_data)
               {
                  min = i;
               }
            }
            push_back( min._node->_data );
            erase( min );
         }
          
         _isSorted = true;
      }
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
