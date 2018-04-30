
#ifndef TINS_TINS_H
#define TINS_TINS_H

#define MICRO_TO_SEC 1000000
 
#include <tins/dns.h>
#include <tins/arp.h>
#include <tins/bootp.h>
#include <tins/dhcp.h>
#include <tins/eapol.h>
#include <tins/ethernetII.h>
#include <tins/ieee802_3.h>
#include <tins/llc.h>
#include <tins/icmp.h>
#include <tins/icmpv6.h>
#if defined(TINS_HAVE_DOT11)
#include <tins/dot11.h>
#endif
#include <tins/dot1q.h>
#include <tins/dot3.h>
#include <tins/ip.h>
#include <tins/ipv6.h>
#include <tins/mpls.h>
#include <tins/packet_sender.h>
#include <tins/packet_writer.h>
#include <tins/pdu.h>
#include <tins/radiotap.h>
#include <tins/rawpdu.h>
#include <tins/snap.h>
#include <tins/sniffer.h>
#include <tins/tcp.h>
#include <tins/udp.h>
#include <tins/utils.h>
#include <tins/tcp_stream.h>
#include <tins/crypto.h>
#include <tins/pdu_cacher.h>
#include <tins/rsn_information.h>
#include <tins/ipv6_address.h>
#include <tins/ip_address.h>
#include <tins/packet.h>
#include <tins/timestamp.h>
#include <tins/sll.h>
#include <tins/dhcpv6.h>
#include <tins/pppoe.h>
#include <tins/stp.h>
#include <tins/handshake_capturer.h>
#include <tins/address_range.h>
#include <tins/pdu_allocator.h>
#include <tins/ipsec.h>
#include <tins/ip_reassembler.h>
#include <tins/ppi.h>
#include <tins/pdu_iterator.h>
#include <tins/tins.h>
#include <thread>
#include <future>

using std::set;
using std::string;
using std::runtime_error;

using namespace Tins;

class ProbereqSniffer {
public:
    void run(const string& iface);
private:
    typedef Dot11::address_type address_type;
    typedef set<address_type> ssids_type;
     
    ssids_type ssids;
};

int probe_request_scanner(string interface, std::future<void> future_obj_local);

#endif // TINS_TINS_H