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
    int dec_val = 0, base = 1;
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

void readAndParse(string codeLine)
{
    string label, opcode, arg0, arg1, arg2;
    std::vector<string> tokens;
    std::stringstream ss(codeLine);
    string token;

    while (getline(ss, token, ' ') && token[0] != '#')
    {
        tokens.push_back(token);
    }

    if (tokens.empty() || tokens[0][0] == '#')
        return;
    // if (tokens[0].back() == ':')
    // {
    //     tokens[0].pop_back();
    //     label = tokens[0];
    //     tokens.size() >= 2 ? opcode = tokens[1] : opcode = "";
    //     tokens.size() >= 3 ? arg0 = tokens[2] : arg0 = "";
    //     tokens.size() >= 4 ? arg1 = tokens[3] : arg1 = "";
    //     tokens.size() >= 5 ? arg2 = tokens[4] : arg2 = "";
    // }
    // else
    // {
    opcode = tokens[0];
    tokens.size() >= 2 ? arg0 = tokens[1] : arg0 = "";
    tokens.size() >= 3 ? arg1 = tokens[2] : arg1 = "";
    tokens.size() >= 4 ? arg2 = tokens[3] : arg2 = "";
    if (arg1.back() == ')')
    {
        arg2 = arg1;
        int cnt = 0, i = arg2.length() - 2;
        while (arg2[i] != '(')
            cnt++, i--;
        arg1 = arg2.substr(i + 1, cnt);
        arg2 = arg2.substr(0, i);
        cout << opcode << " " << arg1 << " " << arg2;
    }

    // }
}

int main()
{
    long long ans = stoll("0xDEADBEEF", 0, 16);
    cout << ans;
    // string s = "lw a1, 0x00000004(zero)";
    // readAndParse(s);
    // cout << bin_sign_imm(8192, 12) << "\n";
    // cout << bin_sign_imm(-7, 12) << "\n";
    // cout << dec_sign_imm(bin_sign_imm(8192, 12));
    return 0;
}