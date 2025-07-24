#include "rootedtree.h"

#include <vector>

using namespace std;

template<typename SetupFunction, typename DiscoverFunction, typename FinishFunction, typename CleanupFunction>
void RootedTree::df_traversal(SetupFunction setup, DiscoverFunction discover, FinishFunction finish, CleanupFunction cleanup)
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
    for(size_t i = 0; i<N.size(); i++)          //RESET state of RootedTree,
        neighbourIterators[i] = N[i].begin();
}