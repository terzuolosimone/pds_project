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

int main() {
    /* In the definitive version "wlan0mon" will be replaced with the name
     *  of the ESP32 interface.
     */
    string interface = "wlan0mon";
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
	    cout << "END OF THIS PART 1" << endl;
	    /* SECOND PART
	     * In the second part the program simply send over the file
	     *  in the previous part.
	     */
	    file_send_recv(sock, res, filename);
	}
    return 0;
}
