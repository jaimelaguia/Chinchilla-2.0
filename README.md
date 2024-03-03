# ChinchillaV2C
Scada de Trydan con un esp32-2432s028r

**Insertar manual de installacion aqui**
//En la carpeta Arduino estan las librerias que hacen falta, se pueden instalar con el manager salvo el archivo User_Setup.h de la libreria TFT_eSPI


//Con esto se suben las imagenes con LittleFS

![Captura desde 2024-03-03 02-42-21](https://github.com/jaimelaguia/ChinchillaV2C/assets/106588791/2f88a974-572c-47f5-9a87-971ad95ecc57)

//Esta es la config que uso


![imagen](https://github.com/jaimelaguia/ChinchillaV2C/assets/106588791/deea7632-37db-49f6-8461-169189635005)



//Placa que he comprado y scada


![imagen](https://github.com/jaimelaguia/Chinchilla-2.0/assets/106588791/a33c5dd8-9866-430f-bbed-0c13229ab3b5)![imagen](https://github.com/jaimelaguia/Chinchilla-2.0/assets/106588791/0aee3b8d-d451-4dd1-9b80-64f9d32a04f2)




//To do para primera version:
- Dejar como dios manda el manejo de variables para facilitar la inclusion de nuevas interfaces
- Gestionar despues las propiedades para mandar la peticion a Trydan en caso de cambio desde Chinchilla
- Tarea infumable de meter cordenadas y tamaños con defines que permitan modificar la interfaz sin querer morir.
- Alguien que este mas acostumbrado a Arduino podria dejar esto mucho mejor. Soy de C/C++ me gusta el barro lo siento hago lo que puedo XD
  
//Ideas para futuro
- Incluir botones controlables por Home Assistant
- Incluir ESPAlexa para que tambien maneje comandos de Alexa
- Levantar un broker mqtt para usarla de pasarela en integraciones


//Tips
Las tareas de FreeRTOS con Arduino vaaaan.... bueno, van. Usarlas solo en caso obligatorio si ralentizan el bucle principal.
