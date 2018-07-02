# Background: 
Caesar cipher, is one of the simplest and most widely known encryption techniques. During encryption, each letter in the plaintext is replaced by a letter some fixed number of positions down the alphabet. In this problem, we set the number=3.

For example,

-Plain: ABCDEFGHIJKLMNOPQRSTUVWXYZ

-Cipher: DEFGHIJKLMNOPQRSTUVWXYZABC

-Plain: abcdefghijklmnopqrstuvwxyz

-Cipher: defghijklmnopqrstuvwxyzabc

Please develop a Caesar Encryption Server, which receives plaintext from clients and sends the corresponding ciphertext to clients. Only the letters need to be encrypted, e.g. How are you? → Krz duh brx? The Server can serve at most 2 clients concurrently, more clients coming have to wait. The server-side program must be concurrent multi-threaded. Client input :q to end the service. For simplicity, you can execute one server and multiple clients in one host.

# Implement: 
This problem contains a server program and a clinet program. In server program, we use thread to give service to each client. Each thread connect to one client. We use a counter to make sure that only 2 threads can give Caesar Encryption service at the same time. Also, in order to make sure that client can know whether the Server is busy or not, for each thread before we start to give Caesar Encryption service, we send a '!' to client means that the server is ready. So, as for client, if we receive the '!', we can jump from busy state to on service state.

# Detail:
These 2 program has Android.mk inside, we can also use NDK to build them and run them on AVD.