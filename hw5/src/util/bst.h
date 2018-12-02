/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   // TODO: design your own class!!
   BSTreeNode( const T& d, BSTreeNode<T>* p=0, BSTreeNode<T>* l=0, BSTreeNode<T>* r=0 ):
      _data(d), _parent(p), _left(l), _right(r) {}

   T _data;
   size_t _copy = 0;
   BSTreeNode<T>* _parent;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
   //implementation
   BSTreeNode<T>* min() 
   {
      if (this->_left!=0) return this->_left->min();
      else return this;
   }
   BSTreeNode<T>* max()
   {
      if (this->_right!=0) return this->_right->max();
      else return this;
   }
   BSTreeNode<T>* succ() const
   {
      if (this==0) return 0;
      if (this->_right!=0) return this->_right->min();
      else return 0;
   }
   BSTreeNode<T>* pred() const
   {
      if (this==0) return 0;
      if (this->_left!=0) return this->_left->max();
      else return 0;
   }
   BSTreeNode<T>* lparent() const
   {
      if (this==0) return 0;
      if (this->_parent==0)
      {
         return 0;
      }
      else if (this->_parent->_right==this) 
      {
         return this->_parent->lparent();
      }
      else
      {
         return this->_parent;
      }
   }
   BSTreeNode<T>* rparent() const
   {
      if (this==0) return 0;
      if (this->_parent==0)
      {
         return 0;
      }
      else if (this->_parent->_left==this) 
      {
         return this->_parent->rparent();
      }
      else
      {
         return this->_parent;
      }
   }
   BSTreeNode<T>* find(const T& x)
   {
      if (this==0) return 0;
      if (x<_data)
      {  
         return this->_left->find(x);
      }
      else if (x>_data)
      {
         return this->_right->find(x);
      }
      else {
         return this;
      }
   }
};


template <class T>
class BSTree
{
// TODO: design your own class!!
public:
   BSTree(): _root(0), _size(0)
   {
   }
   ~BSTree() {} 

   class iterator 
   { 
      friend class BSTree;

   public:
      iterator(BSTreeNode<T>* np, BSTreeNode<T>* rp ): _node(np), _root(rp), _copy(0){ if (np!=0) _copy=np->_copy; _number=1;}
      iterator(const iterator& i) : _node(i._node), _root(i._root), _copy(_node->_copy) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () 
      { 
         if (this->_node!=0)
         {
            if (_number==_copy)
            {
               if (_node->_right!=0)
               {
                  _node = _node->succ();
               }
               else 
               {
                  _node = _node->lparent();
               }
               _copy = _node->_copy;
               _number = 1;
            }
            else
            { 
               ++_number;
            }
         }
         return *this;
      }
      iterator operator ++ (int) { iterator tmp(_node,_root); ++(*(this)); return tmp; }
      iterator& operator -- ()
      { 
         if (this->_node!=0)
         {
            if (_number==0)
            {
               if (_node->_left!=0)
               {
                  _node = _node->pred();
               }
               else
               {
                  _node = _node->rparent();
               }
               _copy = _node->_copy;
               _number = _node->_copy;
            }
            else
            {
               --_number;
            }
         }
         else 
         {
            _node = _root->max();
            _copy = _node->_copy;
            _number = _node->_copy;
         }
         return *this;
      }
      iterator operator -- (int) { iterator tmp(_node,_root); --(*(this)); return tmp; }

      iterator& operator = (const iterator& i) { _node = i._node; _root = i._root; _copy=i._copy; _number=i._number; return *(this); }

      bool operator == (const iterator& i) const { return (this->_node==i._node&&this->_root==i._root); }
      bool operator != (const iterator& i) const { return !(*this==i); }

   private:
      BSTreeNode<T>* _node;
      BSTreeNode<T>* _root;
      size_t _copy = 0;
      size_t _number;
   };
   // TODO: implement these functions
   iterator begin() const { if (_root!=0) return iterator(_root->min(),_root); else return iterator(0,_root); }//todo
   iterator end() const { return iterator(0,_root); }//todo
   bool empty() const { return (_root==0); }
   size_t size() const { return _size; }
   void insert (const T& x) 
   { 
      insert(_root,x);
      ++_size;
   }
   void pop_front() 
   { 
      if (_root!=0)
      {
         erase( iterator(_root->min(),_root ) );
      }
      else {}
   }
   void pop_back() 
   { 
      if (_root!=0)
      {
         erase( iterator(_root->max(),_root ) );
      }
      else {}
   }
   void print() const
   { 
   }

   // return false if nothing to erase
   bool erase(iterator pos) 
   { 
      if (pos == iterator(0,_root)) return false;
      if (_root!=0)
      {
         if (pos._node->_left!=0&&pos._node->_right!=0)//two child
         {
            BSTreeNode<T>* successor = pos._node->succ();
            pos._node->_data = successor->_data;
            erase( iterator(successor,_root) );
         }
         else if (pos._node->_left==0&&pos._node->_right==0)//leaf
         {
            --(pos._node->_copy);
            if (pos._node->_copy==0)
            {
               if (pos._node->_parent!=0)
               {
                  if (pos._node->_parent->_left==pos._node)
                  {
                     pos._node->_parent->_left=0;
                  }
                  else
                  {
                     pos._node->_parent->_right=0;
                  }  
                  delete pos._node;
               }
               else //only one node, root
               {
                  delete _root;
                  _root = 0;
               }
            } else {}
            --_size;
         }
         else if (pos._node->_right==0)
         {
            --(pos._node->_copy);
            if (pos._node->_copy==0)
            {
               pos._node->_left->_parent = pos._node->_parent;
               if (pos._node->_parent!=0)
               {
                  if (pos._node->_parent->_left==pos._node)
                  {
                     pos._node->_parent->_left=pos._node->_left;
                  }
                  else
                  {
                     pos._node->_parent->_right=pos._node->_left;
                  }  
               }  
               else 
               {
                  _root = pos._node->_left;
               }
               delete pos._node;
            } else {}
            --_size;
         }
         else
         {
            --(pos._node->_copy);
            if (pos._node->_copy==0)
            {
               pos._node->_right->_parent = pos._node->_parent;
               if (pos._node->_parent!=0)
               {
                  if (pos._node->_parent->_left==pos._node)
                  {
                     pos._node->_parent->_left=pos._node->_right;
                  }
                  else
                  {
                     pos._node->_parent->_right=pos._node->_right;
                  }  
               }  
               else
               {
                  _root = pos._node->_right;
               }
               delete pos._node;
            } else {}
            --_size;
         }
         return true;
      }
      else 
      {
         return false; 
      }
   }
   bool erase(const T& x)
   {
      return erase( find(x) );
   }

   iterator find(const T& x)
   {  
      return iterator(_root->find(x),_root); 
   }

   void clear() 
   {
      for (iterator i=this->begin(); i!=this->end();)
      {
         iterator tmp = i;
         ++i;
         erase( tmp );
      }
      _root=0;
      _size=0;
   }

   void sort() 
   { 
   }


private:
   BSTreeNode<T>*  _root;     // = dummy node if list is empty
   size_t _size;
   void insert(BSTreeNode<T>*& node, const T& x)
   {
      if (node==0)
      {
         node = new BSTreeNode<T>(x);
         ++(node->_copy);
      }
      else if (x==node->_data)
      {
         BSTreeNode<T>* oldnode = node;
         node = new BSTreeNode<T>(x,node->_parent,oldnode,0);
         oldnode->_parent=node;
         ++(node->_copy);
      }
      else if (x<node->_data)
      {
         insert(node->_left,x);
         node->_left->_parent = node;
      }
      else
      {
         insert(node->_right,x);
         node->_right->_parent = node;
      }
   }
};

#endif // BST_H
