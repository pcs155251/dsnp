/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);


//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
   if (_dofile.is_open()) {
      readCmdInt(_dofile);
      _dofile.close();
   }
   else
      readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
   resetBufAndPrintPrompt();

   while (1) {
      ParseChar pch = getChar(istr);
      if (pch == INPUT_END_KEY) break;
      switch (pch) {
         case LINE_BEGIN_KEY :
         case HOME_KEY       : moveBufPtr(_readBuf); break;
         case LINE_END_KEY   :
         case END_KEY        : moveBufPtr(_readBufEnd); break;
         case BACK_SPACE_KEY : /* TODO done */ 
                               if (_readBufPtr!=_readBuf){
                                  moveBufPtr(_readBufPtr-1); deleteChar(); 
                               }
                               else {
                                  mybeep();
                               }
                               break;
         case DELETE_KEY     : deleteChar(); break;
         case NEWLINE_KEY    : addHistory();
                               cout << char(NEWLINE_KEY);
                               resetBufAndPrintPrompt(); break;
         case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
         case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
         case ARROW_RIGHT_KEY: /* TODO done */ 
                               moveBufPtr(_readBufPtr+1); break;
         case ARROW_LEFT_KEY : /* TODO done */ 
                               moveBufPtr(_readBufPtr-1); break;
         case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
         case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
         case TAB_KEY        : /* TODO done */ 
                               {
                               int res = (_readBufPtr-_readBuf)%TAB_POSITION;
                               for (int i=0; i!=TAB_POSITION-res; i++)
                                  insertChar(' ');
                               }
                               break;
         case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
         case UNDEFINED_KEY  : mybeep(); break;
         default:  // printable character
            insertChar(char(pch)); break;
      }
      #ifdef TA_KB_SETTING
      taTestOnly();
      #endif
   }
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
   // TODO done...
   if ( (_readBuf<=ptr)&&(ptr<=_readBufEnd) ){
      if ( ptr<=_readBufPtr ){
         for ( char* itmptr=_readBufPtr; itmptr!=ptr; itmptr-- ){
            cout<<"\b";
         }
      }
      else if ( ptr==_readBufEnd ){
         for ( char* itmptr=_readBufPtr; itmptr!=_readBufEnd; itmptr++ ){
            cout<<*itmptr;
         }
      }
      else if ( ptr>_readBufPtr ){
         moveBufPtr( _readBufEnd );
         moveBufPtr( ptr );
      }
      _readBufPtr = ptr;
      return true;
   }
   else {
      mybeep();
      return false;
   }
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar()
{
   // TODO done...
   if (_readBufPtr==_readBufEnd){
      mybeep();
      return false;
   }
   else {
      for (char* itmptr=_readBufPtr; itmptr<_readBufEnd; itmptr++){
         *itmptr=*(itmptr+1);
      }
      char* tmp=_readBufPtr;
      _readBufEnd--;
      moveBufPtr(_readBufEnd);
      cout<<" \b";
      moveBufPtr(tmp);
      return true;
   }
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat)
{
   // TODO...
   assert(repeat >= 1);
   // here is for repeat==1 case
   if ( repeat>1 ){
     insertChar( ch );
     insertChar( ch, repeat-1 );
   }
   else {
     for ( char* i=_readBufEnd; i>=_readBufPtr; i--){
        *(i+1)=*(i);
     }
     _readBufEnd += 1;
     *_readBufPtr = ch;
     moveBufPtr( _readBufPtr+1 );
     //cout<<" "<<_readBufEnd-_readBuf<<" ";
   }
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine()
{
   // TODO...
   int length = _readBufEnd-_readBuf;
   moveBufPtr( _readBuf );
   if (length!=0){
     insertChar( ' ', length );
     moveBufPtr( _readBuf );
     _readBufEnd = _readBuf;
     *_readBufEnd = 0;
   } else {}
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index)
{
   // TODO...
   if (index < _historyIdx){
      if (_historyIdx==0){
         mybeep();
      }
      else {
         if (_historyIdx==_history.size()){
           addHistory();
           _tempCmdStored = true;
         } else {}
         if (index<0){
            index=0;
         } else {}
         deleteLine();
         cout<<_history[index];
         _historyIdx = index;
      }
   }
   else {
      if (_historyIdx==_history.size()){
         mybeep();
      }
      else {
         if (index>=_history.size()){
            index=_history.size()-1;
         } else {}
         deleteLine();
         cout<<_history[index];
         _historyIdx = index;
      }
   }
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory()
{
   // TODO...
   char *hstart=NULL, *hend;
   for (char* i=_readBuf; i!=_readBufEnd; i++){
      if (*i==' '){}
      else {
         hstart=i;
         break;
      }
   }
   if (hstart!=NULL){
      for (char* i=_readBufEnd-1; i>=_readBuf; i--){
         if (*i==' '){}
         else {
            hend=i;
            break;
         }
      }
      _history.push_back( string(hstart,hend+1) );
   }
   else {}
   _historyIdx=_history.size();
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory()
{
   deleteLine();
   strcpy(_readBuf, _history[_historyIdx].c_str());
   cout << _readBuf;
   _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
