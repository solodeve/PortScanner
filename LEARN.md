# What I learned

--- Problem encountered --- 

1. segementation fault: 11

means:
- code want to acces a memory location but couldn't do it
- a fct want to acces a memory location that it not have acces to it

problem with an acces to a vector while in mutli-threading context -> solution found was a mutex lock and unlock but didn't work

2. linker failed

- check the makefile -> it means the code don t have something (for example: I compiled main and PortScanner but not the Cli class)

--- Ports --- 
20,    // FTP Data
21,    // FTP
22,    // SSH
23,    // Telnet
25,    // SMTP
53,    // DNS
67,    // DHCP Server
68,    // DHCP Client
69,    // TFTP
80,    // HTTP
110,   // POP3
111,   // RPCbind
123,   // NTP
135,   // MS RPC
137,   // NetBIOS Name
138,   // NetBIOS Datagram
139,   // NetBIOS Session
143,   // IMAP
161,   // SNMP
162,   // SNMP Trap
179,   // BGP
389,   // LDAP
443,   // HTTPS
445,   // SMB
465,   // SMTPS
514,   // Syslog
587,   // SMTP Submission
636,   // LDAPS
873,   // rsync
993,   // IMAPS
995,   // POP3S
1433,  // Microsoft SQL Server
1521,  // Oracle
1723,  // PPTP
1883,  // MQTT
2049,  // NFS
2375,  // Docker (non sécurisé)
2376,  // Docker TLS
3000,  // Node.js / Grafana
3306,  // MySQL
3389,  // RDP
4000,
5000,
5432,  // PostgreSQL
5601,  // Kibana
5672,  // RabbitMQ
5900,  // VNC
5985,  // WinRM HTTP
5986,  // WinRM HTTPS
6379,  // Redis
6443,  // Kubernetes API
7001,  // WebLogic
8000,
8008,
8080,  // HTTP Alternate
8081,
8088,
8443,  // HTTPS Alternate
8888,
9000,
9090,
9200,  // Elasticsearch
9300,
9418,  // Git
10000, // Webmin
11211, // Memcached
27017, // MongoDB
50000