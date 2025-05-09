#ifndef __UDT_API_H__
#define __UDT_API_H__
#include <map>
#include <vector>
#include <memory>
#include "udt.h"
#include "packet.h"
#include "queue.h"
#include "cache.h"
#include "epoll.h"

class udt;

#pragma once

#include <cstdint>
#include <set>
#include <mutex>
#include <condition_variable>

class cudt; // forward declaration
using udtsocket_t = int; 

enum class udt_status {
    init,
    open,
    connecting,
    connected,
    closed,
    broken
};

class udt_socket {
public:
    udt_socket() noexcept;
    ~udt_socket();
    udt_socket(const udt_socket&) = delete;
    udt_socket& operator=(const udt_socket&) = delete; 

private:
    udt_status status_ {};                      // current socket state
    uint64_t timestamp_ {};                     // time when the socket is closed
    int ip_version_ {};                         // IP version (IPv4 or IPv6)
    sockaddr* self_addr_ {nullptr};  // pointer to the local address
    sockaddr* peer_addr_ {nullptr};             // pointer to the peer address

    udtsocket_t socket_id_ {};                    // this socket's ID
    udtsocket_t listen_socket_ {};                // listener socket ID (0 if independent)

    udtsocket_t peer_id_ {};                      // peer socket ID
    int32_t initial_seq_num_ {};                // initial sequence number

    cudt* udt_entity_ {nullptr};                // pointer to the UDT protocol logic

    std::unique_ptr<std::set<udtsocket_t>> queued_sockets_;    // sockets waiting for accept()
    std::unique_ptr<std::set<udtsocket_t>> accepted_sockets_ {nullptr};  // sockets already accepted
    std::condition_variable accept_cond_;       // used to block accept()
    std::mutex accept_mutex_;                   // mutex for accept_cond_
    unsigned int backlog_ {};                   // max queued connections
    int mux_id_ {};                             // multiplexer ID
    std::mutex control_mutex_;                  // used for control operations (bind/listen/connect)
};


////////////////////////////////////////////////////////////////////////////////

class CUDTUnited
{
friend class CUDT;
friend class CRendezvousQueue;

public:
   CUDTUnited();
   ~CUDTUnited();

public:

      // Functionality:
      //    initialize the UDT library.
      // Parameters:
      //    None.
      // Returned value:
      //    0 if success, otherwise -1 is returned.

   int startup();

      // Functionality:
      //    release the UDT library.
      // Parameters:
      //    None.
      // Returned value:
      //    0 if success, otherwise -1 is returned.

   int cleanup();

      // Functionality:
      //    Create a new UDT socket.
      // Parameters:
      //    0) [in] af: IP version, IPv4 (AF_INET) or IPv6 (AF_INET6).
      //    1) [in] type: socket type, SOCK_STREAM or SOCK_DGRAM
      // Returned value:
      //    The new UDT socket ID, or INVALID_SOCK.

   UDTSOCKET newSocket(int af, int type);

      // Functionality:
      //    Create a new UDT connection.
      // Parameters:
      //    0) [in] listen: the listening UDT socket;
      //    1) [in] peer: peer address.
      //    2) [in/out] hs: handshake information from peer side (in), negotiated value (out);
      // Returned value:
      //    If the new connection is successfully created: 1 success, 0 already exist, -1 error.

   int newConnection(const UDTSOCKET listen, const sockaddr* peer, CHandShake* hs);

      // Functionality:
      //    look up the UDT entity according to its ID.
      // Parameters:
      //    0) [in] u: the UDT socket ID.
      // Returned value:
      //    Pointer to the UDT entity.

   CUDT* lookup(const UDTSOCKET u);

      // Functionality:
      //    Check the status of the UDT socket.
      // Parameters:
      //    0) [in] u: the UDT socket ID.
      // Returned value:
      //    UDT socket status, or NONEXIST if not found.

   UDTSTATUS getStatus(const UDTSOCKET u);

      // socket APIs

   int bind(const UDTSOCKET u, const sockaddr* name, int namelen);
   int bind(const UDTSOCKET u, UDPSOCKET udpsock);
   int listen(const UDTSOCKET u, int backlog);
   UDTSOCKET accept(const UDTSOCKET listen, sockaddr* addr, int* addrlen);
   int connect(const UDTSOCKET u, const sockaddr* name, int namelen);
   int close(const UDTSOCKET u);
   int getpeername(const UDTSOCKET u, sockaddr* name, int* namelen);
   int getsockname(const UDTSOCKET u, sockaddr* name, int* namelen);
   int select(ud_set* readfds, ud_set* writefds, ud_set* exceptfds, const timeval* timeout);
   int selectEx(const std::vector<UDTSOCKET>& fds, std::vector<UDTSOCKET>* readfds, std::vector<UDTSOCKET>* writefds, std::vector<UDTSOCKET>* exceptfds, int64_t msTimeOut);
   int epoll_create();
   int epoll_add_usock(const int eid, const UDTSOCKET u, const int* events = NULL);
   int epoll_add_ssock(const int eid, const SYSSOCKET s, const int* events = NULL);
   int epoll_remove_usock(const int eid, const UDTSOCKET u);
   int epoll_remove_ssock(const int eid, const SYSSOCKET s);
   int epoll_wait(const int eid, std::set<UDTSOCKET>* readfds, std::set<UDTSOCKET>* writefds, int64_t msTimeOut, std::set<SYSSOCKET>* lrfds = NULL, std::set<SYSSOCKET>* lwfds = NULL);
   int epoll_release(const int eid);

      // Functionality:
      //    record the UDT exception.
      // Parameters:
      //    0) [in] e: pointer to a UDT exception instance.
      // Returned value:
      //    None.

   void setError(CUDTException* e);

      // Functionality:
      //    look up the most recent UDT exception.
      // Parameters:
      //    None.
      // Returned value:
      //    pointer to a UDT exception instance.

   CUDTException* getError();

private:
//   void init();

private:
   std::map<UDTSOCKET, CUDTSocket*> m_Sockets;       // stores all the socket structures

   pthread_mutex_t m_ControlLock;                    // used to synchronize UDT API

   pthread_mutex_t m_IDLock;                         // used to synchronize ID generation
   UDTSOCKET m_SocketID;                             // seed to generate a new unique socket ID

   std::map<int64_t, std::set<UDTSOCKET> > m_PeerRec;// record sockets from peers to avoid repeated connection request, int64_t = (socker_id << 30) + isn

private:
   pthread_key_t m_TLSError;                         // thread local error record (last error)
   #ifndef WIN32
      static void TLSDestroy(void* e) {if (NULL != e) delete (CUDTException*)e;}
   #else
      std::map<DWORD, CUDTException*> m_mTLSRecord;
      void checkTLSValue();
      pthread_mutex_t m_TLSLock;
   #endif

private:
   void connect_complete(const UDTSOCKET u);
   CUDTSocket* locate(const UDTSOCKET u);
   CUDTSocket* locate(const sockaddr* peer, const UDTSOCKET id, int32_t isn);
   void updateMux(CUDTSocket* s, const sockaddr* addr = NULL, const UDPSOCKET* = NULL);
   void updateMux(CUDTSocket* s, const CUDTSocket* ls);

private:
   std::map<int, CMultiplexer> m_mMultiplexer;		// UDP multiplexer
   pthread_mutex_t m_MultiplexerLock;

private:
   CCache<CInfoBlock>* m_pCache;			// UDT network information cache

private:
   volatile bool m_bClosing;
   pthread_mutex_t m_GCStopLock;
   pthread_cond_t m_GCStopCond;

   pthread_mutex_t m_InitLock;
   int m_iInstanceCount;				// number of startup() called by application
   bool m_bGCStatus;					// if the GC thread is working (true)

   pthread_t m_GCThread;
   #ifndef WIN32
      static void* garbageCollect(void*);
   #else
      static DWORD WINAPI garbageCollect(LPVOID);
   #endif

   std::map<UDTSOCKET, CUDTSocket*> m_ClosedSockets;   // temporarily store closed sockets

   void checkBrokenSockets();
   void removeSocket(const UDTSOCKET u);

private:
   CEPoll m_EPoll;                                     // handling epoll data structures and events

private:
   CUDTUnited(const CUDTUnited&);
   CUDTUnited& operator=(const CUDTUnited&);
};

#endif
