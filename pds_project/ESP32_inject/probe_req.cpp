 
#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>

#include "probe_req.h"
 
using std::set;
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

using namespace Tins;

std::future<void> future_obj_global;
 
void ProbereqSniffer::run(const std::string& iface) {
    SnifferConfiguration config;
    config.set_promisc_mode(true);
    config.set_filter("type mgt subtype probe-req");
    config.set_rfmon(true);
    Sniffer sniffer(iface, config);
    std::ofstream outfile ("test.txt");
    Timestamp ts_relative;
    int i = 0;
    /* Loop for probe-request packet-sniffing.
     * Exits when timeout expires.
	 */
	while(future_obj_global.wait_for(std::chrono::milliseconds(1))
    	== std::future_status::timeout){
		// NOTE: patched method Sniffer::next_packet() in sniffer.cpp
		Packet pkt = sniffer.next_packet();
	    // Get the PDU
	    PDU& pdu = *pkt.pdu();
		if(pkt.pdu() != nullptr){
		    // Get the Dot11 layer
		    const Dot11ProbeRequest& probereq = pdu.rfind_pdu<Dot11ProbeRequest>();
		    // Get the AP address
		    address_type addr = probereq.addr2();
		    // Get RadioTap parameters (signal dBm)
		    RadioTap radio = RadioTap() / Dot11ProbeRequest();
		    int sig = (int)radio.dbm_signal();
		    // Get SSID
		    string ssid = probereq.ssid();
		    if(ssid == "")
		    	ssid = "unknown";
		    // Get the relative timestamp
		    Timestamp ts = pkt.timestamp();
		    if(i == 0){
		    	i++;
		    	ts_relative = ts;
		    }
		    std::chrono::microseconds us = ts;
		    std::chrono::microseconds us_relative = ts_relative;
		    float time_rel = (float)(us.count()-us_relative.count())/MICRO_TO_SEC;
		    // Get the sequence number
		    int seqnum = (int)probereq.seq_num();
		    // Display what we got - only to be sure it's working
		    cout << "MAC addr: " << addr
		    << " - Signal: " << sig
		    << "(dBm) - SSID: " << ssid 
		    << " - Timestamp: " << std::fixed << std::setprecision(6) << time_rel
		    << " seconds - SeqNum: " << seqnum
		    << endl;
		    // Write to file
		    outfile << addr << " " 
		    << sig << " " << ssid
		    << " " << std::fixed << std::setprecision(6) << time_rel
		    << " " << seqnum << endl;
		    // NOTE: Gotta add those packet hash values, but I don't know what they are...
	    }
	}
}
 
int probe_request_scanner(string interface, std::future<void> future_obj_local) {

	future_obj_global = std::move(future_obj_local);
    ProbereqSniffer sniffer;
    sniffer.run(interface);
    return 0;
}
