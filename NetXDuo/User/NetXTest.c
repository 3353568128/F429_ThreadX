#include "NetXTest.h"

NX_PACKET_POOL    pool_0;
NX_IP             ip_0; 

/* Define the IP thread's stack area.  */

ULONG             ip_thread_stack[2 * 1024 / sizeof(ULONG)];


/* Define packet pool for the demonstration.  */

#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 60)

ULONG             packet_pool_area[NX_PACKET_POOL_SIZE/4 + 4];


/* Define the ARP cache area.  */

ULONG             arp_space_area[512 / sizeof(ULONG)];

                                                           
/* Define an error counter.  */

ULONG             error_counter;

/* Define what the initial system looks like.  */
UINT  status;
void PingTest(void)
{
    /* Initialize the NetX system.  */
    nx_system_initialize();
    
    /* Create a packet pool.  */
    status =  nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 1536,  (ULONG*)(((int)packet_pool_area + 15) & ~15) , NX_PACKET_POOL_SIZE);

    /* Check for pool creation error.  */
    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, 
                          "NetX IP Instance 0", 
                          IP_ADDRESS(192, 168, 1, 33), 
                          0xFFFFFF00, 
                          &pool_0, nx_driver_stm32f429,
                          (UCHAR*)ip_thread_stack,
                          sizeof(ip_thread_stack),
                          1);
    
    /* Check for IP create errors.  */
    if (status)
        error_counter++;
        
    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status =  nx_arp_enable(&ip_0, (void *)arp_space_area, sizeof(arp_space_area));

    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable TCP traffic.  */
    status =  nx_tcp_enable(&ip_0);
    
    /* Check for TCP enable errors.  */
    if (status)
        error_counter++;
    
    /* Enable UDP traffic.  */
    status =  nx_udp_enable(&ip_0);
    
    /* Check for UDP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP.  */
    status =  nx_icmp_enable(&ip_0);
   
    /* Check for errors.  */
    if (status)
        error_counter++;   

}