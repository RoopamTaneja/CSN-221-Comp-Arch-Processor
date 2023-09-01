#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef vector<ll> vl;
typedef vector<int> vi;

void remcom(string &s)
{
    int size = s.size();
    if (s.front() == ',')
        s = s.substr(1, size - 1);
    if (s.back() == ',')
        s.pop_back();
}

int main()
{
    string s = "adsds";
    remcom(s);
    cout << s;

    return 0;
}