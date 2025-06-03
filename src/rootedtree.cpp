#include "rootedtree.h"

#include <vector>

using namespace std;

//Order of initialization list relevant, therefore odd naming, something about -Wreorder
RootedTree::RootedTree(const vector<vector<int>>& ADJ, int root) : N(ADJ), root(root), number_of_nodes(ADJ.size())
{
                                                                                    //setup(*this);
    parents.resize(number_of_nodes);
    neighbourIterators.resize(number_of_nodes);
    for(int index = 0; index<number_of_nodes; index++)
        neighbourIterators[index] = N[index].begin();

    //Initialize
    int current = root;
    int parent = root;
    parents[current-1] = parent;                                                    //discover(current, *this);

        //Start traversal
    while(current != parent || neighbourIterators[current-1] != N[current-1].end())
    {   //I'm NOT ROOT or NOT DONE

            //Case NOT DONE
        if(neighbourIterators[current-1] != N[current-1].end())
        {
            if(*neighbourIterators[current-1]==parent)
                neighbourIterators[current-1]++;
            else
            {
                parent = current;
                current = *neighbourIterators[current-1];
                parents[current-1] = parent;                                          // discover(current, *this);
            }
        }
        //Case DONE and NOT ROOT
        else
        {
                                                                                    //finish(current,*this), does nothing
            neighbourIterators[parent-1]++;
            current = parent;
            parent = parents[current-1];
        }
    }
    for(int i = 0; i<number_of_nodes; i++)                                              //cleanup(*this);
        neighbourIterators[i] = N[i].begin();
}

void RootedTree::df_traversal(function<void(const RootedTree&)> setup,function<void(const int&, const RootedTree&)> discover,function<void(const int&, const RootedTree&)> finish,function<void(const RootedTree&)> cleanup)
{
    setup(*this);

    //Initialize
    int current = root;
    int parent = root;

    discover(current,*this);    //

    //Start traversal
    while(current != parent || neighbourIterators[current-1] != N[current-1].end())
    {   //I'm NOT ROOT or NOT DONE

        //Case NOT DONE
        if(neighbourIterators[current-1] != N[current-1].end())
        {
            if(*neighbourIterators[current-1]==parent)
            {
                neighbourIterators[current-1]++;
            }
            else
            {
                parent = current;
                current = *neighbourIterators[current-1];
                discover(current,*this);
            }
        }
        //Case DONE and NOT ROOT
        else
        {
            finish(current,*this);
            neighbourIterators[parent-1]++;
            current = parent;
            parent = parents[current-1];
        }
    }

    //Finish the root or whatever that means
    finish(current,*this);  //
    cleanup(*this);
    for(int i = 0; i<number_of_nodes; i++)          //RESET state of RootedTree,
        neighbourIterators[i] = N[i].begin();
}
