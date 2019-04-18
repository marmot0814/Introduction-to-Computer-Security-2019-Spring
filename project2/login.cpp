#include <bits/stdc++.h>
using namespace std;
void cmd(string s) {
    system(s.c_str());
}
void cmd_on_victim(string ssh_prefix, string s) {
    cmd(ssh_prefix + " \'" + s + "\'");
}
int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <victim IP address> <username> <password>\n";
        return 0;
    }
    string ip_addr  = string(argv[1]);
    string username = string(argv[2]);
    string password = string(argv[3]);
    string ssh_prefix = "ssh -o IdentitiesOnly=yes -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null " + username + "@" + ip_addr;
    cmd(ssh_prefix);
}
