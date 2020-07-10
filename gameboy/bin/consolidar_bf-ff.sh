 ./gameboy BROKER CATCH_POKEMON Charmander 23 87 # 22 Bytes
 ./gameboy BROKER CATCH_POKEMON Bulbasaur 23 87 # 21 Bytes
 ./gameboy BROKER APPEARED_POKEMON Bulbasaur 23 87 5  # 21 Bytes
 ./gameboy BROKER GET_POKEMON Venusaur  # 12 Bytes: Reemplaza Particion 1- Ocupa Bytes 0  a 11 - Genera un hueco de 10 Bytes
 ./gameboy SUSCRIPTOR CATCH_POKEMON 1
 ./gameboy BROKER GET_POKEMON Charizard  # 13 Bytes: Por LRU Reemplaza Particion Mensaje APPEARED (Bytes 43 a 63) Ocupa Bytes 43 a 55 - Genera un hueco de 8 Bytes
 ./gameboy BROKER GET_POKEMON Onix # 8 Bytes : FF Ocupa una partición del Byte 12 al 19 -  BF Ocupa una partición del Byte 56 al 63 - En ambos casos quedan 10 bytes libres