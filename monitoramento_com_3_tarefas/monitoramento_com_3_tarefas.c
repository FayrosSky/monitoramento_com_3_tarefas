#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Definições de pinos
#define BOTAO_PIN 5  // Pino GPIO para simular o botão
#define LED_PIN 25   // Pino GPIO para o LED onboard

// Variáveis globais
QueueHandle_t xButtonQueue; // Fila para compartilhar o estado do botão
QueueHandle_t xLedQueue;    // Fila para controlar o LED

// Tarefa 1: Leitura do Botão
void vTaskReadButton(void *pvParameters) {
    int buttonState = 0;

    while (1) {
        buttonState = gpio_get(BOTAO_PIN);

        // Envia o estado do botão para a fila
        if (xQueueSend(xButtonQueue, &buttonState, portMAX_DELAY) != pdPASS) {
            printf("Erro ao enviar estado do botão para a fila!\n");
        } else {
            printf("Estado do botão enviado para a fila: %d\n", buttonState);
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Aguarda 100ms antes da próxima leitura
    }
}

// Tarefa 2: Processamento do Botão
void vTaskProcessButton(void *pvParameters) {
    int buttonState = 0;

    while (1) {
        // Recebe o estado do botão da fila
        if (xQueueReceive(xButtonQueue, &buttonState, portMAX_DELAY) == pdPASS) {
            int ledState = (buttonState == 0) ? 1 : 0; // Nível lógico invertido
            if (xQueueSend(xLedQueue, &ledState, portMAX_DELAY) != pdPASS) {
                printf("Erro ao enviar sinal para a fila do LED!\n");
            } else {
                printf("Botão %s. Enviando comando para %s o LED.\n",
                       (buttonState == 0) ? "pressionado" : "solto",
                       (buttonState == 0) ? "ligar" : "apagar");
            }
        }
    }
}

// Tarefa 3: Controle do LED
void vTaskControlLed(void *pvParameters) {
    int ledState = 0;

    while (1) {
        // Recebe o estado do LED da fila
        if (xQueueReceive(xLedQueue, &ledState, portMAX_DELAY) == pdPASS) {
            gpio_put(LED_PIN, ledState);
            printf("LED atualizado: %s\n", ledState ? "Ligado" : "Desligado");
        }
    }
}

// Função principal
int main() {
    // Inicialização do hardware
    stdio_init_all();
    vTaskDelay(pdMS_TO_TICKS(2000)); // Aguarda inicialização da serial
    printf("Inicializando sistema...\n");

    // Configuração dos pinos
    gpio_init(BOTAO_PIN);
    gpio_set_dir(BOTAO_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_PIN); // Configura pull-up interno para o botão

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Criação das filas
    xButtonQueue = xQueueCreate(1, sizeof(int));
    xLedQueue = xQueueCreate(1, sizeof(int));

    if (xButtonQueue == NULL || xLedQueue == NULL) {
        printf("Erro ao criar as filas!\n");
        return -1;
    } else {
        printf("Filas criadas com sucesso.\n");
    }

    // Criação das tarefas
    xTaskCreate(vTaskReadButton, "ReadButton", 128, NULL, 2, NULL);
    xTaskCreate(vTaskProcessButton, "ProcessButton", 128, NULL, 2, NULL);
    xTaskCreate(vTaskControlLed, "ControlLed", 128, NULL, 2, NULL);

    // Inicia o escalonador do FreeRTOS
    vTaskStartScheduler();

    // O programa nunca deve chegar aqui
    while (1) {
        ;
    }
}