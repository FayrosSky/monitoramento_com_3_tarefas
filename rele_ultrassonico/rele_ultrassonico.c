#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"

// Definição dos pinos GPIO
#define RELAY_PIN 16      // Pino conectado ao módulo relé
#define BUTTON_PIN 5      // Pino do botão BOOTSEL (GP5)
#define TRIG_PIN 17       // Pino TRIG do sensor ultrassônico
#define ECHO_PIN 20       // Pino ECHO do sensor ultrassônico

// Variáveis para controle do relé
bool relay_state = false;

// Limites de nível de água em centímetros
#define MIN_WATER_LEVEL 10.0f  // Nível mínimo para ligar a bomba
#define MAX_WATER_LEVEL 20.0f  // Nível máximo para desligar a bomba

// Função para inicializar o pino do relé
void relay_init(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT); // Configura o pino como saída
}

// Função para alternar o estado do relé
void toggle_relay(uint pin, bool state) {
    gpio_put(pin, state); // Atualiza o estado do pino
    if (state) {
        printf("Relé ligado - Bomba d'água ativada.\n");
    } else {
        printf("Relé desligado - Bomba d'água desativada.\n");
    }
}

// Função para inicializar os pinos do sensor ultrassônico
void ultrasonic_init(uint trig_pin, uint echo_pin) {
    gpio_init(trig_pin);
    gpio_init(echo_pin);

    gpio_set_dir(trig_pin, GPIO_OUT); // TRIG é saída
    gpio_set_dir(echo_pin, GPIO_IN);  // ECHO é entrada
}

// Função para medir a distância usando o sensor ultrassônico
float measure_distance(uint trig_pin, uint echo_pin) {
    // Envia um pulso de 10 µs no pino TRIG
    gpio_put(trig_pin, 1);
    sleep_us(10);
    gpio_put(trig_pin, 0);

    // Aguarda o início do pulso no ECHO
    absolute_time_t start_time = get_absolute_time();
    while (!gpio_get(echo_pin)) {
        if (absolute_time_diff_us(start_time, get_absolute_time()) > 10000) {
            return -1; // Timeout
        }
    }

    // Mede a duração do pulso no ECHO
    start_time = get_absolute_time();
    while (gpio_get(echo_pin)) {
        if (absolute_time_diff_us(start_time, get_absolute_time()) > 10000) {
            return -1; // Timeout
        }
    }

    int64_t pulse_duration = absolute_time_diff_us(start_time, get_absolute_time());

    // Calcula a distância em centímetros (velocidade do som = 343 m/s)
    float distance_cm = (pulse_duration / 2.0f) / 29.1f;
    return distance_cm;
}

// Função para inicializar o botão
void button_init(uint pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);       // Configura o pino como entrada
    gpio_pull_up(pin);                // Habilita resistor pull-up interno
}

int main() {
    // Inicializa o stdio para comunicação serial
    stdio_init_all();

    // Aguarda inicialização da interface serial (opcional)
    sleep_ms(2000);

    // Inicializa os pinos
    relay_init(RELAY_PIN);
    ultrasonic_init(TRIG_PIN, ECHO_PIN);
    button_init(BUTTON_PIN);

    printf("Sistema de Controle de Nível de Água com Sensor Ultrassônico\n");

    // Variável para detectar o estado anterior do botão
    bool last_button_state = true; // Assume que o botão está solto inicialmente

    while (1) {
        // Mede a distância (nível de água)
        float water_level = measure_distance(TRIG_PIN, ECHO_PIN);

        if (water_level < 0) {
            printf("Erro ao medir o nível da água.\n");
        } else {
            printf("Nível da água: %.2f cm\n", water_level);

            // Lógica para controlar a bomba d'água automaticamente
            if (water_level < MIN_WATER_LEVEL && !relay_state) {
                // Liga a bomba se o nível estiver abaixo do mínimo
                relay_state = true;
                toggle_relay(RELAY_PIN, relay_state);
            } else if (water_level > MAX_WATER_LEVEL && relay_state) {
                // Desliga a bomba se o nível estiver acima do máximo
                relay_state = false;
                toggle_relay(RELAY_PIN, relay_state);
            }
        }

        // Lê o estado atual do botão
        bool button_state = gpio_get(BUTTON_PIN);

        // Detecta a borda de descida (botão pressionado)
        if (button_state == false && last_button_state == true) {
            // Alterna o estado do relé manualmente
            relay_state = !relay_state;
            toggle_relay(RELAY_PIN, relay_state);

            // Aguarda um curto período para evitar bouncing
            sleep_ms(200);
        }

        // Atualiza o estado anterior do botão
        last_button_state = button_state;

        // Aguarda antes da próxima medição
        sleep_ms(1000);
    }

    return 0;
} 