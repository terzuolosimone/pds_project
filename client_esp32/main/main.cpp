#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <cerrno>
#include <iostream>
#include <string>

#include "lwip/err.h"
#include "lwip/sys.h"

extern "C" {
	int app_main(void);
}

typedef struct{
    int16_t fctl;
    int16_t duration;
    uint8_t mac_dst[6];
    uint8_t mac_src[6];
    uint8_t bssid[6];
    uint16_t seqct;
    unsigned char payload[];
} wifi_mgt_hdr;

char send_out_buff[10000];
int flag;

void client(){
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr;

    memset(recvBuff, '0', sizeof(recvBuff));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    } 

    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(15100); 

    if(inet_pton(AF_INET, "192.168.4.3"/*"192.168.1.134"*/, &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return;
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       std::cout << strerror(errno) << std::endl;
       return;
    } 
    printf("size: %u\n", strlen(send_out_buff));
    printf("\nCONNECTED LOL\n");

    /*int i;

    for(i=0;i<strlen(send_out_buff);i++){
        printf("%c", send_out_buff[i]);
    }
    print("SIZE = %d\n", i);*/
    char size[20];
    int dim=500;
    sprintf(size, "%d", strlen(send_out_buff));
    printf("size: %s\n", size);
    int size_atoi = (int)atoi(size);
    write(sockfd, size, strlen(size));
    if(size_atoi >= dim){
	    int x=0;
	    int count=((size_atoi/dim)+1);
	    printf("ATOI SIZE: %d\n", size_atoi);
	    printf("COUNT: %d\n", count);
	    char tmp[dim];
	    int j;
	    int i = 0;
	    for(j=0;j<count;j++){
	    	bzero(tmp, sizeof(tmp));
	    	for(x=0; x<dim; x++){
	    		if(send_out_buff[i] == '\0')
	    			break;
	    		tmp[x] = send_out_buff[i];
	    		i++;
	    	}
	    	tmp[x] = '\0';
			write(sockfd, tmp, strlen(tmp));
	    }
    }
    else{
	    printf("PORCO IL DIO");
            write(sockfd, send_out_buff, strlen(send_out_buff));
    }
    if((n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    if(n < 0)
    {
        printf("\n Read error \n");
    } 
    close(sockfd);
}

void sniffer(void *buf, wifi_promiscuous_pkt_type_t type){
    wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*)buf;
    wifi_mgt_hdr *wh = (wifi_mgt_hdr*) p->payload;
    char tmp[255];
    if((int)wh->fctl == 64 && (int)wh->bssid[3] == 255){
        /*printf("\n\nPROBE_REQUEST_FOUND!!!!\n\n");
        printf("RSSI: %d, TIMESTAMP: %u ", (int)p->rx_ctrl.rssi, p->rx_ctrl.timestamp);
        printf("MAC DST: %x:%x:%x:%x:%x:%x, "
        "MAC SRC %x:%x:%x:%x:%x:%x, "
        "BSSID: %x:%x:%x:%x:%x:%x\n\n",
        wh->mac_dst[0], wh->mac_dst[1], wh->mac_dst[2],
        wh->mac_dst[3], wh->mac_dst[4], wh->mac_dst[5],
        wh->mac_src[0], wh->mac_src[1], wh->mac_src[2],
        wh->mac_src[3], wh->mac_src[4], wh->mac_src[5],
        wh->bssid[0], wh->bssid[1], wh->bssid[2],
        wh->bssid[3], wh->bssid[4], wh->bssid[5]);*/
        sprintf(tmp,
            "RSSI: %d, TIMESTAMP: %u "
            "MAC DST: %x:%x:%x:%x:%x:%x, "
            "MAC SRC %x:%x:%x:%x:%x:%x, "
            "BSSID: %x:%x:%x:%x:%x:%x\n\n",
            (int)p->rx_ctrl.rssi, p->rx_ctrl.timestamp,
            wh->mac_dst[0], wh->mac_dst[1], wh->mac_dst[2],
            wh->mac_dst[3], wh->mac_dst[4], wh->mac_dst[5],
            wh->mac_src[0], wh->mac_src[1], wh->mac_src[2],
            wh->mac_src[3], wh->mac_src[4], wh->mac_src[5],
            wh->bssid[0], wh->bssid[1], wh->bssid[2],
            wh->bssid[3], wh->bssid[4], wh->bssid[5]);
        strcpy(send_out_buff+strlen(send_out_buff), tmp);
        //printf("\n SIZEOF BUF = %d\n", strlen(send_out_buff));
    }
}

void start_loop(){
    while(true){
        printf("SNIFFER TURN\n");
        flag = 0;
        memset(send_out_buff, '0', sizeof(send_out_buff));
        send_out_buff[0] = '\0';
        esp_wifi_set_promiscuous(true);
        vTaskDelay(7000 / portTICK_PERIOD_MS);
        printf("CLIENT TURN\n");
        flag = 1;
        client();
        vTaskDelay(7000 / portTICK_PERIOD_MS);
     }
}

esp_err_t event_handler(void *ctx, system_event_t *event){
switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        printf("got ip:%s\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        break;
    default:
        break;
}
 return ESP_OK;
}

int app_main(void){
	 nvs_flash_init();
	 tcpip_adapter_init();
	 ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
	 wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	 ESP_ERROR_CHECK(esp_wifi_init(&cfg) );

	 wifi_config_t sta_config = { };
	 strcpy((char*)sta_config.sta.ssid, "myssid");//"simosoneppe");
	 strcpy((char*)sta_config.sta.password, "mypassword");//"Pamparato.11.7");

	 ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA));
	 ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	 ESP_ERROR_CHECK(esp_wifi_start());
     esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
     esp_wifi_set_promiscuous_rx_cb(&sniffer);
     start_loop();
 return 0;
}
