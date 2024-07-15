#include "device.h"
#include "track.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef WIN32
 #include <direct.h>
 #define S_ISDIR(m) (((m)& S_IFMT) == S_IFDIR)
 #define mkdir(pathname, mode) _mkdir(pathname)
#elif defined(GEKKO)
 #include <network.h>
#endif

static int find_track(struct sync_device *d, const char *name)
{
    int i;
    for (i = 0; i < (int)d->num_tracks; ++i)
        if (!strcmp(name, d->tracks[i]->name))
            return i;
    return -1; /* not found */
}

static int valid_path_char(char ch)
{
    switch (ch) {
    case '.':
    case '_':
    case '/':
        return 1;

    default:
        return isalnum((unsigned char)ch);
    }
}

static const char *path_encode(const char *path)
{
    static char temp[FILENAME_MAX];
    int i, pos = 0;
    int path_len = (int)strlen(path);
    for (i = 0; i < path_len; ++i) {
        int ch = (unsigned char)path[i];
        if (valid_path_char(ch)) {
            if (pos >= sizeof(temp) - 1)
                break;

            temp[pos++] = (char)ch;
        } else {
            if (pos >= sizeof(temp) - 3)
                break;

            temp[pos++] = '-';
            temp[pos++] = "0123456789ABCDEF"[(ch >> 4) & 0xF];
            temp[pos++] = "0123456789ABCDEF"[ch & 0xF];
        }
    }

    temp[pos] = '\0';
    return temp;
}

static const char *sync_track_path(const char *base, const char *name)
{
    static char temp[FILENAME_MAX];
    strncpy(temp, base, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    strncat(temp, "_", sizeof(temp) - strlen(temp) - 1);
    strncat(temp, path_encode(name), sizeof(temp) - strlen(temp) - 1);
    strncat(temp, ".track", sizeof(temp) - strlen(temp) - 1);
    return temp;
}

#ifndef SYNC_PLAYER

#define CLIENT_GREET "hello, synctracker!"
#define SERVER_GREET "hello, demo!"

enum {
    SET_KEY = 0,
    DELETE_KEY = 1,
    GET_TRACK = 2,
    SET_ROW = 3,
    PAUSE = 4,
    SAVE_TRACKS = 5
};

static inline int socket_poll(SOCKET socket)
{
#ifdef GEKKO
    // libogc doesn't implement select()...
    struct pollsd sds[1];
    sds[0].socket  = socket;
    sds[0].events  = POLLIN;
    sds[0].revents = 0;
    if (net_poll(sds, 1, 0) < 0) return 0;
    return (sds[0].revents & POLLIN) && !(sds[0].revents & (POLLERR|POLLHUP|POLLNVAL));
#else
    struct timeval to = { 0, 0 };
    fd_set fds;

    FD_ZERO(&fds);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127)
#endif
    FD_SET(socket, &fds);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

    return select((int)socket + 1, &fds, NULL, NULL, &to) > 0;
#endif
}

static inline int xsend(SOCKET s, const void *buf, size_t len, int flags)
{
#ifdef WIN32
    assert(len <= INT_MAX);
    return send(s, (const char *)buf, (int)len, flags) != (int)len;
#else
    return send(s, (const char *)buf, len, flags) != (int)len;
#endif
}

static inline int xrecv(SOCKET s, void *buf, size_t len, int flags)
{
#ifdef WIN32
    assert(len <= INT_MAX);
    return recv(s, (char *)buf, (int)len, flags) != (int)len;
#else
    return recv(s, (char *)buf, len, flags) != (int)len;
#endif
}

#ifdef USE_AMITCP
static struct Library *socket_base = NULL;
#endif

static SOCKET server_connect(const char *host, unsigned short nport)
{
    SOCKET sock = INVALID_SOCKET;
#ifdef USE_GETADDRINFO
    struct addrinfo *addr, *curr;
    char port[6];
#else
    struct hostent *he;
    char **ap;
#endif

#ifdef WIN32
    static int need_init = 1;
    if (need_init) {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 0), &wsa))
            return INVALID_SOCKET;
        need_init = 0;
    }
#elif defined(USE_AMITCP)
    if (!socket_base) {
        socket_base = OpenLibrary("bsdsocket.library", 4);
        if (!socket_base)
            return INVALID_SOCKET;
    }
#endif

#ifdef USE_GETADDRINFO

    snprintf(port, sizeof(port), "%u", nport);
    if (getaddrinfo(host, port, 0, &addr) != 0)
        return INVALID_SOCKET;

    for (curr = addr; curr; curr = curr->ai_next) {
        int family = curr->ai_family;
        struct sockaddr *sa = curr->ai_addr;
        int sa_len = (int)curr->ai_addrlen;

#else

    he = gethostbyname(host);
    if (!he)
        return INVALID_SOCKET;

    for (ap = he->h_addr_list; *ap; ++ap) {
        int family = he->h_addrtype;
        struct sockaddr_in sin;
        struct sockaddr *sa = (struct sockaddr *)&sin;
        int sa_len = sizeof(*sa);

        sin.sin_family = he->h_addrtype;
        sin.sin_port = htons(nport);
        memcpy(&sin.sin_addr, *ap, he->h_length);
        memset(&sin.sin_zero, 0, sizeof(sin.sin_zero));

#endif

        sock = socket(family, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
            continue;

        if (connect(sock, sa, sa_len) >= 0) {
            char greet[128];

#ifdef USE_NODELAY
            int yes = 1;

            /* Try disabling Nagle since we're latency-sensitive, UDP would
             * really be more appropriate but that's a much bigger change.
             */
            (void) setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&yes, sizeof(yes));
#endif

            if (xsend(sock, CLIENT_GREET, strlen(CLIENT_GREET), 0) ||
                xrecv(sock, greet, strlen(SERVER_GREET), 0)) {
                closesocket(sock);
                sock = INVALID_SOCKET;
                continue;
            }

            if (!strncmp(SERVER_GREET, greet, strlen(SERVER_GREET)))
                break;
        }

        closesocket(sock);
        sock = INVALID_SOCKET;
    }

#ifdef USE_GETADDRINFO
    freeaddrinfo(addr);
#endif

    return sock;
}

#else

void sync_set_io_cb(struct sync_cb *cb)
{
    io_cb = *cb;
}
#endif /* !defined(SYNC_PLAYER) */

