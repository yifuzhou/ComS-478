@Author Yifu Zhou

I have already compile Manager.java and Client.c files, so you just run it.

First, run the Manager.
java -jar Manager

Then, use different consoles run the Client
./Client <Client_ID> <Client_cmdPort>

For example:
./Client 27 8888
./Client 25 8899
...

I set beacons will be sent each 10 seconds, which is easy to test.
Case 1:
If you close one client, so after 20 seconds(2 beacons), the manager will detect and output the client is dead.
Case 2:
If you close one client, you open another client and set its ID the same as the one you just closed, the manager will detect and output the client restart.

Enjoy it!
Thanks
