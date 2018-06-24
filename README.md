# RIOT_UDP_EchoServerExample
UDP Echo Server Example Implementation in the RIOT Linux Native
The server echos back the received UDP data with a delay of 1 second.

Steps to execute and test.

1. Copy the folders "udpechoserver" and "udptxrx" into the examples folder of the RIOT folder structure.
   (If copied somewhere else the Makefiles has to be adapted accordingly)
2. Execute make in both the directories. The native executeables for linux will be built.#
3. Create two tap devices with the command "RIOT/dist/tools/tapsetup/tapsetup -c 2" 
4. Execte first the "udpechoserver" using the command "make term PORT=tap0"
5. Then execute the "udptxrx" also using the command "make term PORT=tap1" from inside "udptxrx" folder.

If the following steps are followed correctly then the "udptxrx" will start sending UDP packets which will be echoed back by the "udpechoserver" RIOT instance with a 1 second delay which will also be recieved and printed for verification by the "udptxrx" instance of the RIOT. 
