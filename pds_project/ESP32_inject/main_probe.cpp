#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include "probe_req.h"
#include <tins/tins.h>
#include <thread>
#include <future>

using std::string;
using std::cout;
using std::endl;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " <<* argv << " <interface>" << endl;
        return 1;
    }
    /* In the definitive version argc and argv[] will not be
     *  implemented and argv[1] will be replaced with the name
     *  of the ESP32 interface.
     */
    string interface = argv[1];
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
	    while(i<60){
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
	}
    return 0;
}
