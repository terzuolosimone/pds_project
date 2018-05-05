
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include <stdio.h>
#include "esp_log.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

typedef struct{
	int16_t fctl;
	int16_t duration;
	uint8_t mac_dst[6];
	uint8_t mac_src[6];
	uint8_t bssid[6];
	uint16_t seqct;
	unsigned char payload[];
} wifi_mgt_hdr;

static esp_err_t event_handler(void *ctx, system_event_t *event){
	return ESP_OK;
}

void sniffer(void *buf, wifi_promiscuous_pkt_type_t type){
	wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*)buf;
	wifi_mgt_hdr *wh = (wifi_mgt_hdr*) p->payload;
	if((int)wh->mac_dst[0] == 255){
	if((int)wh->bssid[0] == 255){
		printf("\n\nPROBE_REQUEST_FOUND!!!!\n\n");
		printf("RSSI: %d, TIMESTAMP: %u ", (int)p->rx_ctrl.rssi, p->rx_ctrl.timestamp);
		printf("MAC DST: %x:%x:%x:%x:%x:%x, "
		"MAC SRC %x:%x:%x:%x:%x:%x, "
		"BSSID: %x:%x:%x:%x:%x:%x\n\n",
		wh->mac_dst[0], wh->mac_dst[1], wh->mac_dst[2],
		wh->mac_dst[3], wh->mac_dst[4], wh->mac_dst[5],
		wh->mac_src[0], wh->mac_src[1], wh->mac_src[2],
		wh->mac_src[3], wh->mac_src[4], wh->mac_src[5],
		wh->bssid[0], wh->bssid[1], wh->bssid[2],
		wh->bssid[3], wh->bssid[4], wh->bssid[5]);
	}}
}

void app_main(){
	nvs_flash_init();
	tcpip_adapter_init();
	esp_event_loop_init(event_handler, NULL);
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);
	esp_wifi_set_storage(WIFI_STORAGE_RAM);
	esp_wifi_set_mode(WIFI_MODE_NULL);
	esp_wifi_start();
	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(&sniffer);
	esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
}