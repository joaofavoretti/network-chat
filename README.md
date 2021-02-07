# network-chat
Programa para envio de mensagens em uma rede privada
Para você que não esta mais gostando das políticas de privacidade do Whatsapp e problemas de localização do Telegram

Sem garantias de funcionamento no Windows (Mas se executar no WSL/WSL2 provavelmente vai funcionar)

Compile os dois programas com 
`
gcc tcp_server.c -o tcp_server
`

E

`
gcc tcp_client.c -o tcp_client
`

Para usar mantenha um terminal rodando o servidor para hospedar o servico com (E escolha a porta que você deseja usar)
Se lembre que as portas abaixo de 1024 são bloqueadas pelo sistema operacional
`
./tcp_server <porta>
`

Por padrão o programa esta rodando na interface de rede de loopback (localhost) mas não é dificil fazer funcionar com uma interface de rede Ethernet do seu computador e acessar em outros (Experiencia própria)

Rode
`
tcp_client <hostname> <porta>
`
Para acessar o chat. Claro que nesse caso o hostname pode ser "localhost" ou "127.0.0.1", se você quiser parecer saber o que esta fazendo fique com a segunda opção

Obrigado por utilizar!
