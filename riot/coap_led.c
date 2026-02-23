/*
 * Copyright (c) 2015-2016 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       gcoap example
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 *
 * @}
 */

#include <stdio.h>
#include "msg.h"
#include "net/gcoap.h"
#include "shell.h"
#include "fmt.h"
#include "periph/gpio.h"

/* Include lpsxxx headers */
#include "lpsxxx.h"
#include "lpsxxx_params.h"

/* Declare lpsxxx_t sensor variable (globally) */
static lpsxxx_t sensor;

/* Declare _value variable (globally) */
static uint16_t _value = 0;

static ssize_t _encode_link(const coap_resource_t *resource, char *buf,
                            size_t maxlen, coap_link_encoder_ctx_t *context);
static ssize_t _riot_board_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);
/* Declare cpu handler */
static ssize_t _riot_cpu_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);
/* Declare temperature handler */
static ssize_t _temperature_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);
/* Declare value handler */
static ssize_t _value_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    {"/riot/board", COAP_GET, _riot_board_handler, NULL},
    /* Add cpu resource */
    {"/riot/cpu", COAP_GET, _riot_cpu_handler, NULL},
    /* Add temperature resource */
    {"/temperature", COAP_GET, _temperature_handler, NULL},
    /* Add value resource */
    {"/value", COAP_GET | COAP_PUT | COAP_POST, _value_handler, NULL},
};

static ssize_t _riot_board_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    /* write the RIOT board name in the response buffer */
    if (pdu->payload_len >= strlen(RIOT_BOARD))
    {
        memcpy(pdu->payload, RIOT_BOARD, strlen(RIOT_BOARD));
        return resp_len + strlen(RIOT_BOARD);
    }
    else
    {
        puts("gcoap_cli: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }
}

static ssize_t _riot_cpu_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;

    /* Implement cpu GET handler */
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    /* write the RIOT board name in the response buffer */
    if (pdu->payload_len >= strlen(RIOT_CPU))
    {
        memcpy(pdu->payload, RIOT_CPU, strlen(RIOT_CPU));
        return resp_len + strlen(RIOT_CPU);
    }
    else
    {
        puts("gcoap_cli: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }
}

static ssize_t _temperature_handler(coap_pkt_t *pdu, uint8_t *buf,
                                    size_t len, void *ctx)
{
    (void)ctx;

    /* Implement temperature GET handler */
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    char response[32];
    int16_t temp;
    lpsxxx_read_temp(&sensor, &temp);
    int temp_abs = temp / 100;
    temp -= temp_abs * 100;
    sprintf(response, "%2i.%02i°C", temp_abs, temp);

    /* write the temperature value in the response buffer */
    if (pdu->payload_len >= strlen(response))
    {
        memcpy(pdu->payload, response, strlen(response));
        return resp_len + strlen(response);
    }
    else
    {
        puts("gcoap: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }
}

/* CoAP resource handler for /value */
static ssize_t _value_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;

    /* Implement value GET|PUT|POST handler */
    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));
    switch (method_flag)
    {
    case COAP_GET:
        // Handle GET request
        // Initialize CoAP response with a content code
        gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
        // Add CoAP option for response format (text)
        coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
        // Finish CoAP options and payload
        size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

        /* write the response buffer with the request count value */
        resp_len += fmt_u16_dec((char *)pdu->payload, _value);
        return resp_len;

    case COAP_PUT:
        // Handle PUT request
        // Extract payload from CoAP packet
        if (pdu->payload_len <= 5)
        {
            char payload[6] = {0};
            memcpy(payload, (char *)pdu->payload, pdu->payload_len);
            // Update the internal value with the payload (interpreted as an integer)
            _value = (uint16_t)strtoul(payload, NULL, 10);

            // Update the state of an LED based on the payload
            if (pdu->payload_len == 2)
            {
                char led_value = ((char *)pdu->payload)[0];
                char led_state = ((char *)pdu->payload)[1];

                switch (led_value)
                {
                case '0':
                    if (led_state == '0')
                    {
                        LED0_OFF;
                        puts("LED0 OFF");
                    }
                    else if (led_state == '1')
                    {
                        LED0_ON;
                        puts("LED0 ON");
                    }
                    break;
                case '1':
                    if (led_state == '0')
                    {
                        LED1_OFF;
                        puts("LED1 OFF");
                    }
                    else if (led_state == '1')
                    {
                        LED1_ON;
                        puts("LED1 ON");
                    }
                    break;
                case '2':
                    if (led_state == '0')
                    {
                        LED2_OFF;
                        puts("LED2 OFF");
                    }
                    else if (led_state == '1')
                    {
                        LED2_ON;
                        puts("LED2 ON");
                    }
                    break;
                default:
                    puts("Invalid LED value");
                    break;
                }
            }

            else
            {
                puts("Admitted values for modifying LEDs are: 00/01~on/ff led0 | 10/11~on/ff led1 | 20/21~on/ff led2");
            }
            return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
        }

        else
        {
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_REQUEST);
        }

    case COAP_POST:
        // Handle POST request
        // Convert the payload to an integer and update the internal value
        if (pdu->payload_len <= 5)
        {
            char payload[6] = {0};
            memcpy(payload, (char *)pdu->payload, pdu->payload_len);
            _value = (uint16_t)strtoul(payload, NULL, 10);
            return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
        }
        else
        {
            return gcoap_response(pdu, buf, len, COAP_CODE_BAD_REQUEST);
        }

    default:
        return gcoap_response(pdu, buf, len, COAP_CODE_BAD_REQUEST);
    }
    return 0;
}

static gcoap_listener_t _listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    _encode_link,
    NULL,
    NULL};

/* Adds link format params to resource list */
static ssize_t _encode_link(const coap_resource_t *resource, char *buf,
                            size_t maxlen, coap_link_encoder_ctx_t *context)
{
    ssize_t res = gcoap_encode_link(resource, buf, maxlen, context);

    return res;
}

void gcoap_cli_init(void)
{
    gcoap_register_listener(&_listener);
}

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int gcoap_cli_cmd(int argc, char **argv);
extern void gcoap_cli_init(void);

static const shell_command_t shell_commands[] = {
    {"coap", "CoAP example", gcoap_cli_cmd},
    {NULL, NULL, NULL}};

int main(void)
{
    /* Configure LEDs as output */
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    /* Initialize and enable the lps331ap device */
    if (lpsxxx_init(&sensor, &lpsxxx_params[0]) != LPSXXX_OK)
    {
        puts("LPS331AP initialization failed");
        return 1;
    }
    lpsxxx_enable(&sensor);

    /* for the thread running the shell */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_cli_init();
    puts("gcoap example app");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should never be reached */
    return 0;
}
