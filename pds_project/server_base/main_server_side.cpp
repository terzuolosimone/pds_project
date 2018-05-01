
#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <tins/tins.h>
#include <thread>
#include <future>

#include "server.h"

int main(){
	std::thread file_writer(server);
	
	file_writer.join();
}