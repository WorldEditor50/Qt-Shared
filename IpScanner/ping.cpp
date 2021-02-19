#include "ping.h"
/*
The response is an IP packet. We must decode the IP header to locate
the ICMP data
*/
int decode_resp(char *buf, int bytes, struct sockaddr_in *from)
{
    IpHeader *iphdr;
    IcmpHeader *icmphdr;
    unsigned short iphdrlen;
    iphdr = (IpHeader *)buf;
    iphdrlen = (iphdr->h_len) * 4; // number of 32-bit words *4 = bytes
    if (bytes < iphdrlen + ICMP_MIN) {
        printf("Too few bytes from %s\n", inet_ntoa(from->sin_addr));
    }
    icmphdr = (IcmpHeader*)(buf + iphdrlen);
    if (icmphdr->i_type != ICMP_ECHOREPLY) {
        fprintf(stderr,"non-echo type %d recvd\n",icmphdr->i_type);
        return 1;
    }
    if (icmphdr->i_id != (USHORT)GetCurrentProcessId()) {
        fprintf(stderr,"someone else's packet!\n");
        return 1;
    }
    printf("%d bytes from %s:",bytes, inet_ntoa(from->sin_addr));
    printf(" icmp_seq = %d. ",icmphdr->i_seq);
    printf(" time: %d ms ", GetTickCount()-icmphdr->timestamp);
    printf("\n");
    return 0;
}

USHORT checksum(USHORT *buffer, int size)
{
    unsigned long cksum=0;
    while (size >1) {
        cksum+=*buffer++;
        size -=sizeof(USHORT);
    }
    if (size) {
        cksum += *(UCHAR*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (USHORT)(~cksum);
}

/* Helper function to fill in various stuff in our ICMP request. */
void fill_icmp_data(char *icmp_data, int datasize)
{
    IcmpHeader *icmp_hdr;
    char *datapart;
    icmp_hdr = (IcmpHeader*)icmp_data;
    icmp_hdr->i_type = ICMP_ECHO;
    icmp_hdr->i_code = 0;
    icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
    icmp_hdr->i_cksum = 0;
    icmp_hdr->i_seq = 0;
    datapart = icmp_data + sizeof(IcmpHeader);
    /* Place some junk in the buffer. */
    memset(datapart, 'E', datasize - sizeof(IcmpHeader));
    return;
}

int ping(const char* pcHostName, int times, int dataSize)
{
    WSADATA wsaData;
    SOCKET sockRaw;
    struct sockaddr_in stDest;
    struct sockaddr_in stSrc;
    struct hostent *pstHostent = NULL;
    int ret;
    int fromlen = sizeof(stSrc);
    int timeout = 1000;
    int statistic = 0;
    char *dest_ip;
    char *icmp_data;
    char *recvbuf;
    unsigned int addr = 0;
    USHORT seq_no = 0;
    if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0) {
        fprintf(stderr,"WSAStartup failed: %d\n", GetLastError());
        return STATUS_FAILED;
    }
    /* create raw socket */
    sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
    //注：为了使用发送接收超时设置(即设置SO_RCVTIMEO, SO_SNDTIMEO)，必须将标志位设为WSA_FLAG_OVERLAPPED !
    /* set socket option */
    if (sockRaw == INVALID_SOCKET) {
        fprintf(stderr,"WSASocket() failed: %d\n", WSAGetLastError());
        return STATUS_FAILED;
    }
    ret = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    if (ret == SOCKET_ERROR) {
        fprintf(stderr,"failed to set recv timeout: %d\n",WSAGetLastError());
        return STATUS_FAILED;
    }
    timeout = 1000;
    ret = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
    if(ret == SOCKET_ERROR) {
        fprintf(stderr,"failed to set send timeout: %d\n",WSAGetLastError());
        return STATUS_FAILED;
    }
    memset(&stDest, 0, sizeof(stDest));
    /* get destinating address */
    pstHostent = gethostbyname(pcHostName);
    if (!pstHostent) {
        addr = inet_addr(pcHostName);
    }
    if ((!pstHostent) && (addr == INADDR_NONE)) {
        fprintf(stderr,"Unable to resolve %s\n", pcHostName);
        return STATUS_FAILED;
    }
    if (pstHostent != NULL) {
        memcpy(&(stDest.sin_addr), pstHostent->h_addr, pstHostent->h_length);
    } else {
        stDest.sin_addr.s_addr = addr;
    }
    if (pstHostent != NULL) {
        stDest.sin_family = pstHostent->h_addrtype;
    } else {
        stDest.sin_family = AF_INET;
    }
    dest_ip = inet_ntoa(stDest.sin_addr);
    /* get times */
    if (times == 0) {
        times = DEF_PACKET_NUMBER;
    }
    /* get datasize */
    if (dataSize == 0) {
        dataSize = DEF_PACKET_SIZE;
    }
    if (dataSize > 1024) {   /* 用户给出的数据包大小太大 */
        fprintf(stderr, "WARNING : data_size is too large !\n");
        dataSize = DEF_PACKET_SIZE;
    }
    dataSize += sizeof(IcmpHeader);
    icmp_data = (char*)xmalloc(MAX_PACKET);
    recvbuf = (char*)xmalloc(MAX_PACKET);
    if (icmp_data == NULL) {
        fprintf(stderr, "HeapAlloc failed %d\n", GetLastError());
        return STATUS_FAILED;
    }
    /* fill icmp data */
    memset(icmp_data, 0, MAX_PACKET);
    fill_icmp_data(icmp_data, dataSize);
    fprintf(stdout, "\nPinging %s .\n\n", dest_ip);
    for (int i = 0; i < times; ++i) {
        ((IcmpHeader*)icmp_data)->i_cksum = 0;
        ((IcmpHeader*)icmp_data)->timestamp = GetTickCount();
        ((IcmpHeader*)icmp_data)->i_seq = seq_no++;
        ((IcmpHeader*)icmp_data)->i_cksum = checksum((USHORT*)icmp_data, dataSize);
        /* send icmp packet */
        ret = sendto(sockRaw, icmp_data, dataSize, 0, (struct sockaddr*)&stDest, sizeof(stDest));
        if (ret == SOCKET_ERROR){
            if (WSAGetLastError() == WSAETIMEDOUT) {
                printf("Request timed out.\n");
                continue;
            }
            fprintf(stderr, "sendto failed: %d\n", WSAGetLastError());
            ExitProcess(STATUS_FAILED);
        }
        if (ret < dataSize ) {
            fprintf(stdout, "Wrote %d bytes\n", ret);
        }
        /* receive packet */
        ret = recvfrom(sockRaw, recvbuf, MAX_PACKET, 0, (struct sockaddr*)&stSrc, &fromlen);
        if (ret == SOCKET_ERROR) {
            if (WSAGetLastError() == WSAETIMEDOUT) {
                printf("Request timed out.\n");
                continue;
            }
            fprintf(stderr,"recvfrom failed: %d\n", WSAGetLastError());
            return STATUS_FAILED;
        }
        if (!decode_resp(recvbuf, ret, &stSrc)) {
            statistic++; /* 成功接收的数目++ */
        }
        Sleep(1000);
    }
    /* Display the statistic result */
    fprintf(stdout, "\nPing statistics for %s \n", dest_ip);
    fprintf(stdout,
            "    Packets: Sent = %d,Received = %d, Lost = %d (%2.0f%% loss)\n",
            times,
            statistic,
            (times-statistic),
            (float)(times-statistic) / times * 100);
    /* clean */
    if (sockRaw != INVALID_SOCKET) {
        closesocket(sockRaw);
    }
    WSACleanup();
    return 0;
}
