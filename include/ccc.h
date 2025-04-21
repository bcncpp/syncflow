#ifndef __UDT_CCC_H__
#define __UDT_CCC_H__


#include "udt.h"
#include "packet.h"

class configurable_congestion_ctl
{
   friend class cudt;

public:
   configurable_congestion_ctl();
   virtual ~configurable_congestion_ctl();

private:
   configurable_congestion_ctl(const configurable_congestion_ctl&);
   configurable_congestion_ctl& operator=(const configurable_congestion_ctl&) { return *this; }

public:
   virtual void init() {}
   virtual void close() {}
   virtual void on_ack(int32_t) {}
   virtual void on_loss(const int32_t*, int) {}
   virtual void on_timeout() {}
   virtual void on_pkt_sent(const cpacket*) {}
   virtual void on_pkt_received(const cpacket*) {}
   virtual void process_custom_msg(const cpacket*) {}

protected:
   void set_ack_timer(int ms_int);
   void set_ack_interval(int pkt_int);
   void set_rto(int us_rto);
   void send_custom_msg(cpacket& pkt) const;
   const cperf_mon* get_perf_info();
   void set_user_param(const char* param, int size);

private:
   void set_mss(int mss);
   void set_max_cwnd_size(int cwnd);
   void set_bandwidth(int bw);
   void set_snd_curr_seq_no(int32_t seqno);
   void set_rcv_rate(int rcvrate);
   void set_rtt(int rtt);

protected:
   const int32_t& syn_interval;

   double pkt_snd_period;
   double cwnd_size;

   int bandwidth;
   double max_cwnd_size;

   int mss;
   int32_t snd_curr_seq_no;
   int rcv_rate;
   int rtt;

   char* user_param;
   int user_param_size;

private:
   udtsocket udt_socket;

   int ack_period;
   int ack_interval;

   bool user_defined_rto;
   int rto;

   cperf_mon perf_info;
};

class configurable_congestion_ctl_virtual_factory
{
public:
   virtual ~configurable_congestion_ctl_virtual_factory() {}

   virtual configurable_congestion_ctl* create() = 0;
   virtual configurable_congestion_ctl_virtual_factory* clone() = 0;
};

template <class T>
class configurable_congestion_ctl_factory : public configurable_congestion_ctl_virtual_factory
{
public:
   virtual ~configurable_congestion_ctl_factory() {}

   virtual configurable_congestion_ctl* create() { return new T; }
   virtual configurable_congestion_ctl_virtual_factory* clone() { return new configurable_congestion_ctl_factory<T>; }
};

class cudt_congestion_ctl : public configurable_congestion_ctl
{
public:
   cudt_congestion_ctl();

public:
   virtual void init();
   virtual void on_ack(int32_t);
   virtual void on_loss(const int32_t*, int);
   virtual void on_timeout();

private:
   int rc_interval;
   uint64_t last_rc_time;
   bool slow_start;
   int32_t last_ack;
   bool loss_happened;
   int32_t last_dec_seq;
   double last_dec_period;
   int nak_count;
   int dec_random;
   int avg_nak_num;
   int dec_count;
};
#endif