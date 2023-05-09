/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "tcp_recv_picture.h"
#include <string.h>

// #if !defined(TEST_TCP_SERVER_IP)
// #error TEST_TCP_SERVER_IP not defined
// #endif

#define TCP_PORT 9999
#define TEST_TCP_SERVER_IP "192.168.50.222"
#define DEBUG_printf printf
#define BUF_SIZE ((128 * 64 / 8) + 1) 

#define TEST_ITERATIONS 10
#define POLL_TIME_S 5


#if 0
static void dump_bytes(const uint8_t *bptr, uint32_t len) {
    unsigned int i = 0;

    printf("dump_bytes %d", len);
    for (i = 0; i < len;) {
        if ((i & 0x0f) == 0) {
            printf("\n");
        } else if ((i & 0x07) == 0) {
            printf(" ");
        }
        printf("%02x ", bptr[i++]);
    }
    printf("\n");
}
#define DUMP_BYTES dump_bytes
#else
#define DUMP_BYTES(A,B)
#endif

typedef struct TCP_CLIENT_T_ {
    struct tcp_pcb *tcp_pcb;
    ip_addr_t remote_addr;
		uint16_t port;
    uint8_t buffer[BUF_SIZE];
    int buffer_len;
    int sent_len;
    bool complete;
    int run_count;
    bool connected;
		void (*pictureReceivedCallback)(void);
} TCP_CLIENT_T;

extern char picture_buffer[];
static TCP_CLIENT_T *state;

static void printState(TCP_CLIENT_T *state, char *msg)
{
	if (msg)
	{
		printf("%s\n", msg);
	}
	printf("IP: ");
	ip_addr_debug_print(0xff, state->remote_addr);
	printf("\nPORT: %d\n", state->port);
	printf("BUF: ");
	// for (uint16_t i = 0; i < BUF_SIZE; i++)
	// {
	// 	printf("%02x ", state->buffer[i]);
	// }
	printf("\n");
	printf("BUFLEN: %d\n", state->buffer_len);
	printf("SENTLEN: %d\n", state->sent_len);
	printf("COMPLETE: %d\n", state->complete);
	printf("RUNCT: %d\n", state->run_count);
	printf("CONNECTED: %d\n", state->connected);
}

static void copyBufToPicture(void)
{
	memcpy(&picture_buffer, state->buffer, BUF_SIZE);
}

static err_t tcp_client_close(void *arg) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    err_t err = ERR_OK;
    if (state->tcp_pcb != NULL) {
        tcp_arg(state->tcp_pcb, NULL);
        tcp_poll(state->tcp_pcb, NULL, 0);
        tcp_sent(state->tcp_pcb, NULL);
        tcp_recv(state->tcp_pcb, NULL);
        tcp_err(state->tcp_pcb, NULL);
        err = tcp_close(state->tcp_pcb);
        if (err != ERR_OK) {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(state->tcp_pcb);
            err = ERR_ABRT;
        }
        state->tcp_pcb = NULL;
    }
    return err;
}

// Called with results of operation
static err_t tcp_result(void *arg, int status, bool closeConn) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	if (status == 0) {
		printf("test success\n");
	} else {
		printf("test failed %d\n", status);
	}
	state->complete = true;
	if (closeConn)
	{
		return tcp_client_close(arg);
	}
	else
	{
		return 0;
	}
}

static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    DEBUG_printf("tcp_client_sent %u\n", len);
    state->sent_len += len;

    if (state->sent_len >= BUF_SIZE) {

        state->run_count++;
        if (state->run_count >= TEST_ITERATIONS) {
            tcp_result(arg, 0, false);
            return ERR_OK;
        }

        // We should receive a new buffer from the server
        state->buffer_len = 0;
        state->sent_len = 0;
        printf("Waiting for buffer from server\n");
    }

    return ERR_OK;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    if (err != ERR_OK) {
        printf("connect failed %d\n", err);
        return tcp_result(arg, err, true);
    }
    state->connected = true;
    printf("Waiting for buffer from server\n");
    return ERR_OK;
}

static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb) {
    printf("tcp_client_poll\n");
    // return tcp_result(arg, -1, false); // no response is an error?
		return 0;
}

static void tcp_client_err(void *arg, err_t err) {
    if (err != ERR_ABRT) {
        DEBUG_printf("tcp_client_err %d\n", err);
        tcp_result(arg, err, true);
    }
}

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
	if (p == NULL)
	{
		printf("recv pbuf null!\n");
	}
	else
	{
		cyw43_arch_lwip_check();
		if (p->tot_len > 0) {
			printf("recv %d err %d\n", p->tot_len, err);
			for (struct pbuf *q = p; q != NULL; q = q->next) {
				DUMP_BYTES(q->payload, q->len);
			}
			// Receive the buffer
			const uint16_t buffer_left = BUF_SIZE - state->buffer_len;
			state->buffer_len += pbuf_copy_partial(p, state->buffer + state->buffer_len,
																					p->tot_len > buffer_left ? buffer_left : p->tot_len, 0);
			tcp_recved(tpcb, p->tot_len);
		}
		pbuf_free(p);
		// If we have received the whole buffer, send it back to the server
		if (state->buffer_len == BUF_SIZE) {
			// printf("Writing %d bytes to server\n", state->buffer_len);
			// err_t err = tcp_write(tpcb, state->buffer, state->buffer_len, TCP_WRITE_FLAG_COPY);
			// if (err != ERR_OK) {
			// 	printf("Failed to write data %d\n", err);
			// 	return err;
			// 	// return tcp_result(arg, -1, false);
			// }
			copyBufToPicture();
			if (state->pictureReceivedCallback)
			{
				state->pictureReceivedCallback();
			}
			printf("Buf len reached! Resetting tcp buffer\n");
			state->buffer_len = 0;
			// return tcp_result(arg, -1, true);
		}
	}
	printState(state, "RECV");
	return ERR_OK;
}

static bool tcp_client_open(void *arg) {
    TCP_CLIENT_T *state = (TCP_CLIENT_T*)arg;
    printf("Connecting to %s port %u\n", ip4addr_ntoa(&state->remote_addr), state->port);
    state->tcp_pcb = tcp_new_ip_type(IP_GET_TYPE(&state->remote_addr));
    if (!state->tcp_pcb) {
        printf("failed to create pcb\n");
        return false;
    }

    tcp_arg(state->tcp_pcb, state);
    tcp_poll(state->tcp_pcb, tcp_client_poll, POLL_TIME_S * 2);
    tcp_sent(state->tcp_pcb, tcp_client_sent);
    tcp_recv(state->tcp_pcb, tcp_client_recv);
    tcp_err(state->tcp_pcb, tcp_client_err);

    state->buffer_len = 0;

    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(state->tcp_pcb, &state->remote_addr, state->port, tcp_client_connected);
    cyw43_arch_lwip_end();

		// printState(state, "OPENED");
    return err == ERR_OK;
}

// Perform initialisation
static TCP_CLIENT_T* tcp_client_init(TcpRecvInitArgs_s *arg) {
	TCP_CLIENT_T *state = calloc(1, sizeof(TCP_CLIENT_T));
	printf("Initializing tcp_recv_picture client. IP:%s Port:%d\n", arg->ipAddr, arg->port);
	if (!state) {
		DEBUG_printf("failed to allocate state\n");
		return NULL;
	}
	state->port = arg->port;
	state->pictureReceivedCallback = arg->pictureReceivedCallback;
	ip4addr_aton(arg->ipAddr, &state->remote_addr);
	return state;
}

bool TcpRecvPicture_Init(void *arg)
{
	if (arg == NULL) // no args provided
	{
		TcpRecvInitArgs_s defaultArg = (TcpRecvInitArgs_s) {.ipAddr = "192.168.50.222", .port = 9999, .pictureReceivedCallback = NULL};
		state = tcp_client_init(&defaultArg);
	}
	else // args provided
	{
		state = tcp_client_init((TcpRecvInitArgs_s *) arg);
	}
	// printState(state, "INITED");
	if (!state) {
		return false;
	}
	if (!tcp_client_open(state)) {
		printf("Couldnt open tcp client!\n");
		tcp_result(state, -1, true);
		return false;
	}
	return true;
}

bool TcpRecvPicture_Deinit(void)
{
	free(state);
	return true;
}

bool TcpRecvPicture_Update(void)
{

}
