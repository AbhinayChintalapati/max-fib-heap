#include <iostream>
#include <unordered_map>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
//Declaring and initializing node
struct node
{
  int   val ;
  int   degree = 0;
  node* parent = NULL;
  node* child = NULL;
  node* left =  NULL;
  node* right = NULL;
  bool  ccut = 0;
};

//HEAD node
struct node* HEAD = NULL;

// all the functions which will be used to perform heap operations
node* createnode(int key);
void  insertnode(node* x);
void  pairwisemerge(unordered_map<int,node*> &dmap);
void  meld(node* x, node* y,unordered_map<int,node*> &dmap);
node* extractmax();
void  increasekey(int pluskey, node* x);
node* cut(node* x);

//creating node
struct node* createnode(int key)
{
  node* n = new node;
  n->val = key;
  n->parent = NULL;
  n->child= NULL;
  n->left = NULL;
  n->right = NULL;
  n->ccut = 0;

  return n;
}
//inserting a new node to the current heap and inserting it to the circular list
void insertnode(node* x)
{
  if(HEAD != NULL)
  {
    (HEAD->right)->left = x; // inserting the new node to the right of HEAD
    x->right = HEAD->right;
    x->left = HEAD;
    HEAD->right = x;
  }
  else
  {
    HEAD = x;
    x->right = x;
    x->left = x;
  }
  if(HEAD->val < x->val)
  {
    HEAD = x;              //Updating the value of HEAD
  }
}
//increasing the key of a specific node by the given value
void increasekey(int pluskey, node* x)
{
  node* y;
  if(x->parent == NULL)
  {
    x->val=x->val+pluskey;
    if(x->val>HEAD->val)
        HEAD = x; // updating HEAD
  }
  else
 {
  y = x->parent;
  x->val = x->val + pluskey;
  if(x->val > y->val)
  {
    while(x->parent!=NULL){
         cut(x); // remove nodes whose childcut values are true and inserting them in the root level
         if(x->ccut==0){
             x->ccut=1;      // making childcut values true if they are false when one of their children is cut
             break;
         }
     }
  }

}

}

node* cut(node* x)
{
  node *P=x->parent;

  if(x->parent->degree==1)        //if the parent of the node to be cut has only one child
      x->parent->child=NULL;
  else{                          //if the parent has multiple children
      x->right->left=x->left;
      x->left->right=x->right;
      if(x->parent->child->val==x->val)   //if the parent has multiple children and the node which
          x->parent->child=x->right;      // is being cut is the child pointed by child pointer
  }
  x->parent->degree--;
  node* y = x->parent;
  x->left=NULL;                        // Reintializing the pointers of the node which is being cut
  x->right=NULL;
  x->parent=NULL;
  x->ccut=0;
  insertnode(x);                       //Cutting it from the tree and inserting it back in the top level
  return P;
}

struct node* extractmax()
{
  unordered_map<int , node* > dmap;  // declraing the degree map
  node* h;
  h = HEAD;
  node* x;
  x = HEAD->child;
  if(HEAD->child == NULL)             //If HEAD has no children, directly removing it from the circular list
  {
    (HEAD->right)->left = HEAD->left;
    (HEAD->left)->right = HEAD->right;
  }
  else
  {
                node* temp = x;              // Unmarking parent pointers of the children of HEAD
                while(temp->parent != NULL)
                {
                  temp->parent = NULL;
                  temp = temp->right;
                }
                (x->left)->right = HEAD->right; // Introducing the children of HEAD in the top level circular list
                (HEAD->right)->left = x->left;
                x->left = HEAD->left;
                (HEAD->left)->right = x;
  }

  pairwisemerge(dmap);                         //Calling a function to pair wise combine the nodes of same degree
  HEAD=NULL;
  for(auto& itr: dmap)                         // Updating the HEAD
  {
    if(HEAD != NULL)
    {
      if((itr.second)->val > HEAD -> val)
      {
      HEAD = itr.second;
      }
    }
    else
    {
      HEAD = itr.second;
    }
  }
  h->degree=0;            //deintializing the current max
  h->left=NULL;
  h->right=NULL;
  h->ccut=0;
  h->child=NULL;
  h->parent=NULL;
  return h;              // returing the max
}

void pairwisemerge(unordered_map<int,node*> &dmap)
{
  node* x;
  node* y;
  x = HEAD->right;
  node* temp;
  temp = x;
  int count=0;
  y=x;
  x=x->right;
  while(y!=x){     //couting the number of nodes in the top level after removing head
                   // and inserting its children in the first level
    count++;
    x=x->right;
    if(x==x->right)
      break;
  }
  while(count>-1)    // Running the loop and entering entries in the degree hash map
  {
  auto itr = dmap.find(temp->degree);
  if(itr == dmap.end()) // if the entry for the current degree dosent exist
  {
    pair<int, node*> newpair(temp->degree,temp); // making a new pair and entering it in the degree hasp map
    dmap.insert(newpair);
    temp = temp->right;
  }
  else                                         //if a pair for the current degree already exists
  {
    node* temp2 = temp;
    temp = temp->right;
    meld(temp2,itr->second,dmap);             // calling the 2 nodes into a single one
  }
count--;
}

}

void meld(node* x, node* y,unordered_map<int,node*> &dmap)
{
     dmap.erase(x->degree);                  //Erasing the enrtry with the current degree as
   if ( x->val<y->val)                       // we are merging it to produce a node with degree+1
   {
      node* t;                               //swapping the nodes so that x > y
      t = x;
      x =y;
      y =t;

   }
   y->parent = x;                            //melding
   y->left->right = y->right;
   (y->right)->left = y->left;

   if(x->child == NULL)                     //If x node has no children
   {
     x->child = y;
     y->right=y;
     y->left=y;
   }
   else{
         node* a;                          //If x node has children, linking the y node in the circulaer linked list of children
         a = x->child;
         (a->right)->left = y;
         y->right = a->right;
         y->left  = a;
         a->right = y;
      }
      x->degree = x->degree+1;            //increasing the degree of x after adding y as child


   auto itr1 = dmap.find(x->degree);      // checking for a pair in the degree map with degree = new degree
   if(itr1 == dmap.end())
   {
   pair<int, node*> npair(x->degree,x);  // if nothing is found, make a new entry in the hashmap
   dmap.insert(npair);
   }
   else
   {
     meld(x,itr1->second,dmap);          // if an entry is found, meld with this
   }


}

//main
int main(int argc, char **argv)
{
    ifstream inFile(argv[1]);  //talking input file
    ofstream file;             // creating ofstream object
    file.open("output.txt");   // opening output file
    string line, key;
    string start;
    int val=0;
    HEAD = NULL;
    unordered_map<string, node*> hashmap;   // hashmap to link strings and nodes
    unordered_map<node*, string> rhash;     // reverse hash map linking nodes and strings
    vector<node*> list;
    int i;
    while (getline(inFile,line))            //As long as there is in stream
    {
        cout << i++ << endl;
        if(line  == "stop") cout << "successful" << endl;
        size_t pos = line.find(" ");        //splitting the string into keyword and frequency
        if(line.substr(0,1) == "#"){
            start = "#";
        }
        else{
            start = line;
        }
        if(start == "#")                     //if its a keyword
        {
            key = line.substr (1,pos);
            string value = line.substr(pos+1);
            stringstream geek(value);
            geek >> val;
            unordered_map<string, node*>:: iterator itr;
            itr = hashmap.find(key);
            if (itr != hashmap.end())
            {
                increasekey(val,itr->second);
            }
            else
            {
                node* temp = createnode(val);
                hashmap[key] = temp;
                rhash[temp] = key;
                insertnode(temp);
            }

        }
        else{
            stringstream geek(start);        // if the lines only has an integer
            int op=0;
            geek >> op;
            while(op>0){
                node* rem = extractmax();   // extracting max multiple times and printing it out to outfil
                list.push_back(rem);
                unordered_map<node*,string>::const_iterator find = rhash.find (rem);
                        file << find->second << ",";
            op--;
            }
            file << endl;
            for(auto it = list.begin(); it != list.end(); it++){
                insertnode(*it);             //inserting max to list
            }
            list.clear();

        }


    }
    return 0;
}
