#include "MyPOSIXFunctions.h"

// Inicializa alguns recursos necessários
// para usar as funções de sockets e define unicode
int _initialize(void) {
	setlocale(LC_ALL, "");

#ifdef __windows__
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		wprintf(L"%s > WSAStartup falhou: %i\n", __func__, errno);
		return EXIT_FAILURE;
	}
#endif

	return EXIT_SUCCESS;
}

// Finaliza recursos do winsock
int _finish(void) {
#ifdef __windows__
	if(WSACleanup() == SOCKET_ERROR) {
		wprintf(L"%s > WSACleanup falhou: %i\n", __func__, WSAGetLastError());
		return EXIT_FAILURE;
	}
#endif
	return EXIT_SUCCESS;
}

// Verifica se um bloco alocado é nulo
// caso não seja, irá libera-la
void __myfree__(void** block) {
	if (*block) {
		free(*block);
		*block = NULL;
	}
}

// Libera os campos da estrutura server
// e fecha o descritor de arquivo
void freeServer(Server* server) {
	myfree(&server->hostname);
	myfree(&server->ipv4_addr);
	freeaddrinfo(server->addr);
	close(server->sockfd);
}

// Retorna um bloco alocado que contém o 
// diretório atual
char* get_current_dir(void) {
	char* result = (char*)malloc(sizeof(char) * (MAX_PATH + 1));

	if (!result) {
		printf("get_current_dir > malloc falhou.\n");
		return result;
	}

	// getcwd retorna o ponteiro para result
	return getcwd(result, MAX_PATH);
}

// Cria um servidor local especificando a porta
// e a quantidade de clients a serem conectados, pela estrutura
// Sever. A estrutura irá recerber tanto o descritor de arquivo,
// quanto o endereço no formato de string relacionado ao servidor local.
int create_local_server(Server* server) {

	// Definir o endereço e a porta alvo
    // htons (host to network short) e htonl (host to network long)
    // estão sendo usados para converter o valor para ordem big-endian
	struct sockaddr_in local_addr = {
		.sin_family = AF_INET, // ipv4
		.sin_port = htons(server->port), // ordem big-endian
		.sin_addr = htonl(INADDR_ANY) // localhost
	};

	// Criar um socket para conexões tcpip
	SOCKET sockfd = socket(local_addr.sin_family, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == SOCKET_ERROR) {
        // __func__ retorna o nome da função atual apenas
        wprintf(L"%s > socket falhou: %i.\n", __func__, errno);
        return EXIT_FAILURE;
    }

	// Associar o endereço local e a porta definido ao socket criado
    if (bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) == SOCKET_ERROR) {
        wprintf(L"%s > bind falhou: %i.\n", __func__, errno);

        close(sockfd);
        return EXIT_FAILURE;
    }

    // Definir o numero de clientes a ser conectados
    listen(sockfd, server->cclient);

	// Recuperando o nome do host
	char* hostname = (char*)malloc(sizeof(char) * (NI_MAXHOST + 1));
	if (!hostname) {
		wprintf(L"%s > hostname > malloc falhou: %i\n", __func__, errno);

		close(sockfd);
		return EXIT_FAILURE;
	}

	if (gethostname(hostname, NI_MAXHOST) == SOCKET_ERROR) {
		wprintf(L"%s > gethostname falhou: %i\n", __func__, errno);

		close(sockfd);
		myfree(&hostname);
		return EXIT_FAILURE;
	}

	// Obtendo o endereço ip local referente ao nome do host
    int ipv4_addr = get_ipv4_addr(hostname, NULL);
    if (!ipv4_addr) {
        wprintf(L"%s > get_ipv4_addr falhou: %i.\n", __func__, errno);

        close(sockfd);
        return EXIT_FAILURE;
    }

	server->sockfd = sockfd;
	// server->ipv4_addr = ipv4_addr;
	// server->hostname = hostname;

	return EXIT_SUCCESS;
}

// Recupera o endereço ipv4 do host espeficidado e salva o endereço de
// socket (da forma binária) no campo addr da estrutura IPV4_ADDR
int get_ipv4_addr(char* host, IPV4_DATA* ipv4_data) {
	char hostname[NI_MAXHOST];

	// caso host NULL recupera o endereço local da maquina
	if (!host) {
		// Recuperando o nome do host local
		if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
			wprintf(L"%s > gethostname falhou: %i\n.", __func__, errno);
			return EXIT_FAILURE;
		}

// No sistema windows, ha varios adaptadores de rede, cada um representando um endereço
// e uma função especifica. Será necessário recuperar o endereço do adaptador de
// rede principal utilizado, que tenha um endereço gateway padrão utilizado para
// comunicação com a internet
#ifdef __windows__
		PIP_ADAPTER_INFO ipai = NULL;
		unsigned long ipai_len = 0;

		// Recuperar o tamanho necessário para a estrutura PIP_ADAPTER_INFO
		int result = GetAdaptersInfo(ipai, &ipai_len);
		if(result != ERROR_BUFFER_OVERFLOW) {
			wprintf(L"%s > GetAdaptersInfo{1} falhou: %i\n", __func__, result);
			return EXIT_FAILURE;
		}

		ipai = (IP_ADAPTER_INFO*)malloc(ipai_len);
		if (!ipai) {
			wprintf(L"%s > ipai > malloc falhou.\n", __func__);
			return EXIT_FAILURE;
		}

		// Recuperar informações (lista encadeada) de todos os adaptadores de rede local
		result = GetAdaptersInfo(ipai, &ipai_len);
		if (result != ERROR_SUCCESS) {
			wprintf(L"%s > GetAdaptersInfo{2} falhou: %i\n", __func__, result);

			myfree(&ipai);
			return EXIT_FAILURE;
		}

		// Percorrendo a lista de adaptadores de rede e verificando se contem
		// um endereço gateway padrão que não seja 0.0.0.0
		IP_ADAPTER_INFO* iai_in;
		char* ipv4_addr;

		for (iai_in = ipai; iai_in != NULL; iai_in = iai_in->Next) {
			char* gateway = iai_in->GatewayList.IpAddress.String;

			if (strcmp(gateway, "0.0.0.0") != 0) {
				char* addrs = ipai->IpAddressList.IpAddress.String;
				size_t ipv4_addr_len = sizeof(char) * strlen(addrs) + 1;

				ipv4_addr = (char*)malloc(ipv4_addr_len);
				if (!ipv4_addr) {
					wprintf(L"%s > ipv4_addr > malloc{1} falhou.\n", __func__);

					myfree(&ipai);
					return EXIT_FAILURE;
				}

				memset(ipv4_addr, 0, ipv4_addr_len);
				memcpy(ipv4_addr, addrs, strlen(addrs));
				break;
			}
		}

		myfree(&ipai);

		ipv4_data->ipv4_addr = ipv4_addr;
		ipv4_data->hostname = hostname;

		// Ao recuperar o endereço ip local da maquina que contenha um gateway padrão
		// retornar a mesma função passando o endereço ip como host
		return get_ipv4_addr(ipv4_addr, ipv4_data);
#endif
	}

	else {
		memcpy(hostname, host, strlen(host) + 1);
	}

    // hints fornece dicas do que vai ser escrito no ponteiro res pela função getaddrinfo,
    // podemos filtrar apenas endereços ipv4 (AF_INET) por exemplo, especificando
    // na estrutura hints. A estrutura res conterá uma lista encadeada de todos os endereços
    // na forma binaria correspondente ao hostname passado na função getaddrinfo
    struct addrinfo hints, *res;

    // Zerar a estrutura hints
    memset(&hints, 0, sizeof(hints));

    // Definir o que será filtrado
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0) {
        wprintf(L"%s > getaddrinfo falhou: %i.\n", __func__, errno);
        return EXIT_FAILURE;
    }

	// Caso ipv4_data->ipv4_addr não esteja definido, recupera-lo
	if (!ipv4_data->ipv4_addr) {
		char* ipv4_addr = (char*)malloc(sizeof(char) * (INET_ADDRSTRLEN + 1));
		if (!ipv4_addr) {
			wprintf(L"%s > ipv4_addr > malloc{2} falhou.\n", __func__);

			freeaddrinfo(res);
			return EXIT_FAILURE;
		}

		// Fazer a conversão do endereço de socket (sockaddr) para uma representação
		// de cadeia de caracteres do primeiro elemento da lista encadeada. Será necessário
		// fazer um cast da estrutura sockaddr para sockaddr_in, sockaddr_in contem um 
		// campo sin_addr que representa o endereço ip na forma binaria a ser convertida
		if (!inet_ntop(AF_INET, &(((struct sockaddr_in*)res->ai_addr)->sin_addr), ipv4_addr, INET_ADDRSTRLEN)) {
			wprintf(L"%s > inet_ntop falhou: %i.\n", __func__, errno);

			freeaddrinfo(res);
			myfree(&ipv4_addr);
			return EXIT_FAILURE;
		}

		ipv4_data->ipv4_addr = ipv4_addr;
	}

	ipv4_data->addr = res;

	return EXIT_SUCCESS;
}
