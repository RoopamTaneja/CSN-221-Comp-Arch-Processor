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
        binary.push_back(n % 2 + '0');
        n = n / 2;
    }
    while (binary.length() < len)
        binary.push_back('0');
    binary = binary.substr(0, len);
    reverse(binary.begin(), binary.end());
    return binary;
}

string bin_sign_imm(int n, int len)
{
    string imm = binary_enc(abs(n), len);
    if (n < 0)
    {
        int k = imm.find_last_of('1');
        for (int i = k - 1; i >= 0; i--)
            imm[i] == '0' ? imm[i] = '1' : imm[i] = '0';
    }
    return imm;
}

int dec_sign_imm(string s)
{
    int dec_val = 0;
    int base = 1;
    bool neg = (s[0] == '1');
    if (neg)
    {
        int k = s.find_last_of('1');
        for (int i = k - 1; i >= 0; i--)
            s[i] == '0' ? s[i] = '1' : s[i] = '0';
    }
    int len = s.length();
    for (int i = len - 1; i >= 0; i--)
    {
        if (s[i] == '1')
            dec_val += base;
        base = base * 2;
    }
    if (neg)
        return -1 * dec_val;
    return dec_val;
}

int main()
{
    cout << bin_sign_imm(8192, 12) << "\n";
    cout << dec_sign_imm(bin_sign_imm(8192, 12));
    return 0;
}