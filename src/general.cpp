#include "general.h"

#include <vector>
#include <iostream>

using namespace std;

void print_vector(const vector<int>& v)
{
    for(vector<int>::const_iterator it = v.begin(); it != v.end(); it++)
        cout << *it << ' ';
    cout << endl;
}

void print_vector(const vector<unsigned int>& v)
{
    for(vector<unsigned int>::const_iterator it = v.begin(); it != v.end(); it++)
        cout << *it << ' ';
    cout << endl;
}

void print_vector(const vector<unsigned long long>& v)
{
    for(vector<unsigned long long>::const_iterator it = v.begin(); it != v.end(); it++)
        cout << *it << ' ';
    cout << endl;
}

/*
int countr_zero(__uint128_t x)
{
    if (x == 0) return 128;

    uint64_t low = (uint64_t)x;
    if (low != 0) {
        return __builtin_ctzll(low);
    } else {
        uint64_t high = (uint64_t)(x >> 64);
        return 64 + __builtin_ctzll(high);
    }
}
*/
/*
vector<int> bitmask_filter(const unsigned int bitmask, const vector<int>& v)
{
    vector<int> filtered_vector;
    for(int i = 0; i<v.size(); i++)
        if(bitmask & (1<<i))
            filtered_vector.push_back(v[i]);
    return filtered_vector;
}
*/