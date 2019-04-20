#include <bits/stdc++.h>
using namespace std;
void cmd(string s) {
    cout << "\x1b[4" << (system(s.c_str()) ? "1mRE" : "2mAC") << "\x1b[0m : " << s << '\n';
}

