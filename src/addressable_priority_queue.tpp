#include <vector>
#include <functional>
#include <cassert>

using namespace std;

template<typename T, typename Compare, typename Adjust>
AddressablePriorityQueue<T, Compare, Adjust>::AddressablePriorityQueue(int n)
{
    p = vector<int>(n,-1);
}

template<typename T, typename Compare, typename Adjust>
void AddressablePriorityQueue<T, Compare, Adjust>::updateKey(int current, T value)
{
    //assert(p[current-1] != -1);
    //assert(p[current-1] < v.size());
    v[p[current-1]].second = adjust(v[p[current-1]].second, value);
    siftUp(p[current-1]);
}

template<typename T, typename Compare, typename Adjust>
void AddressablePriorityQueue<T, Compare, Adjust>::siftUp(int index)
{
    while(index>0)
    {
        int parentIndex = (index-1)/2;
        if(comp(v[index].second,v[parentIndex].second))
        {
            swap(v[index],v[parentIndex]);
            swap(
                 p[v[index].first - 1],
                 p[v[parentIndex].first - 1]
                 );
            index = parentIndex;
        }
        else
            break;
    }
}

template<typename T, typename Compare, typename Adjust>
void AddressablePriorityQueue<T, Compare, Adjust>::siftDown(int index)
{
    while(true)
    {
        int indexLeftChild = (index<<1) + 1;
        int indexRightChild = (index<<1) + 2;
        int indexBest = index;
        if(indexLeftChild<v.size() && comp(v[indexLeftChild].second,v[indexBest].second))
            indexBest = indexLeftChild;
        if(indexRightChild < v.size() && comp(v[indexRightChild].second,v[indexBest].second))
            indexBest = indexRightChild;
        if(indexBest == index)
            break;
        swap(v[indexBest],v[index]);
        swap(p[v[indexBest].first - 1], p[v[index].first - 1]);
        index = indexBest;
    }
}
    /*
    if(indexLeftChild<=v.size()-1)
    {
        if(indexRightChild<=v.size()-1)
        {
            if(comp(v[indexLeftChild].second,v[indexRightChild].second))
            {
                if(comp(v[indexLeftChild].second,v[index].second))
                {
                    swap(v[indexLeftChild],v[index]);
                    swap(
                         p[v[indexLeftChild].first - 1],
                         p[v[index].first - 1]
                         );
                    siftDown(indexLeftChild);
                }
            }
            else
            {
                if(comp(v[indexRightChild].second,v[index].second))
                {
                    swap(v[indexRightChild],v[index]);
                    swap(
                         p[v[indexRightChild].first - 1],
                         p[v[index].first - 1]
                         );
                    siftDown(indexRightChild);
                }
            }
        }
        else
        {
            if(comp(v[indexLeftChild].second,v[index].second))
            {
                swap(v[indexLeftChild],v[index]);
                swap(
                     p[v[indexLeftChild].first - 1],
                     p[v[index].first - 1]
                     );
                //siftDown(indexLeftChild);
            }
        }
    }

}
*/

template<typename T, typename Compare, typename Adjust>
void AddressablePriorityQueue<T, Compare, Adjust>::insertElement(int current, T key)
{
    v.push_back({current,key});
    p[current - 1] = v.size()-1;
    siftUp(v.size()-1);
}

template<typename T, typename Compare, typename Adjust>
int AddressablePriorityQueue<T, Compare, Adjust>::deleteRoot()
{
    assert(!v.empty());
    swap(v[0],v[v.size()-1]);
    swap(
        p[v[0].first - 1],
        p[v.back().first - 1]
    );
    int root = v.back().first;
    p[v.back().first - 1] = -1;
    v.pop_back();
    if(v.size()>0)
        siftDown(0);
    return root;
}

template<typename T, typename Compare, typename Adjust>
void AddressablePriorityQueue<T, Compare, Adjust>::print() const
{
    int line = 2;
    for(size_t i = 0; i < v.size(); i++)
    {
        cout << v[i].first << '(' << v[i].second << ')' << ' ';
        if(i == line-2 && v.size()-1 != i)
        {
            line*=2;
            cout << endl;
        }
    }
    cout << endl;
}
