#include "LinkedList.h"

template<class T>

class Stack:public LinkedList<T>{
    public:
      bool Delete(T &element){
         if(this->first==0){
             return false;
         }
         Node<T> *current = this->first;
        
         this->first=this->first->link;
   
         element=current->data;
    
         delete current;
 
         this->current_size--;
  
         return true;
         }
};
