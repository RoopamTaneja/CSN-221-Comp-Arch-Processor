#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef vector<ll> vl;
typedef vector<int> vi;

string binary_enc(int n, int len)
{
    string binary;
    for (int i = 0; n != 0; i++)
    {
        binary.push_back(n % 2 + 48);
        n = n / 2;
    }
    while (binary.length() < len)
        binary.push_back('0');
    reverse(binary.begin(), binary.end());
    return binary;
}

int main()
{

    cout << binary_enc(32, 7);
    return 0;
}