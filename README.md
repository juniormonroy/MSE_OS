## Se realiza la implementación de un sistema operativo el cual contiene:

* Interrupción por hardware tecla 1 y 2 flanco de bajada
* dos semáforos binarios
* delay bloqueante
* Indicación de error hooks para chequeo de logs en caso de fallo
* scheduler round robing
* colas para envio de mensajes entre tareas.


Se implemento el OS en una lógica el cual se pretende indicar la temperatura mediante un adc y una rtd con amplificador, se
tiene la opción de indicarla en grados centígrados o Fahrenheit, mediante la tecla 1 vía interrupción flanco de bajada y semáforo, 
se envía el mensaje por la cola "C" para grados centígrados, tecla 2 envía por interrupción flanco de bajada el mensaje "F" para farenheit,
La tarea ADC recibe el mensaje, y lo cataloga si es centígrados o farenheit, y ejecuta la conversión y calculo del adc con prioridad máxima solo en ese instante.
Una tarea de baja prioridad siempre esta corriendo en paralelo para encender y apagar el led verde como heartbeat, llamado tarea watchdog.


![Diagrama ISO](https://user-images.githubusercontent.com/87079881/187349380-d29026ac-fa5c-4450-a85b-d4d870658a51.png)
