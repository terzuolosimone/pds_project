/*
 * BACK-END PART OF THE ESP32 IEEE 802.11 SNIFFING PROJECT
 * This part will be injected in the ESP32 card.
 * This part implements the NON-STANDARD library "tins.lib"
 *  developed by Matias Fontanini, that drastically simplify 
 *  our lives, so I'm gomma give this man the credits he deserves.
 * Anyway, here we capture the PROBE-REQUEST packets sent by
 *  various devices and we store them in the local mass memory.
 * The commenting game is pretty solid, so the code should be easy
 *  to understand.
 * 
 * NOTES:
 * The interface of the sniffer (ESP32) is set in promiscous mode
 *  so that the device is totally transparent for the lan;
 */

#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <tins/tins.h>
#include <thread>
#include <future>

#include "probe_req.h"
#include "filesr.h"

using std::string;
using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    /* In the definitive version argv[1] will be replaced with the name
     *  of the ESP32 interface.
     */
    string interface = argv[1];
    // Create an instance for the server side
    struct addrinfo *res;
    char filename[] = "test.txt";
    int sock;
    res = get_socket(sock);
    // Infinite loop bitches!
    while(true){
       /* FIRST PART    
        * In the first part the program generates two threads:
        *  - the first thread is a timer that will sleep for 60 seconds;
        *  - the second thread is the probe_request capturer that will
        *     last until the first thread times out and then will be killed;
        * As the timer runs out every packet captured is written into the
        *  output file test.txt, ready to be sent.
        */
	    std::promise<void> exitsig;
	    std::future<void> future_obj = exitsig.get_future();
	    std::thread probe_req_th(&probe_request_scanner, interface, std::move(future_obj));
	    int i = 0;
	    while(i<30){
	    	cout << ++i << endl;
	    	std::this_thread::sleep_for(std::chrono::seconds(1));
	    }
	    exitsig.set_value();
	    probe_req_th.join();
	    cout << "END OF PART 1 - probe_request sniffed" << endl;
	    /* SECOND PART
	     * In the second part the program simply send over the file
	     *  in the previous part.
	     */
	    file_send_recv(sock, res, filename);
        cout << "END OF PART 2 - file sent" << endl;
	}
    return 0;
}
