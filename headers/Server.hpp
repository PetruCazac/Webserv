#ifndef SERVER_HPP
#define SERVER_HPP

#include "Socket.hpp"
#include "Config.hpp"

class Server {
	public:
		Server(ServerDirectives& inputConfig, size_t client_max_body_size);
		~Server();

		bool addListeningSocket();
		const std::vector<pollfd_t>& getPollFdVector() const;
		void handleEvents(const std::vector<pollfd_t>& active_fds);
		void handleServerSocketEvents(const pollfd_t& poll_fd);
		void handleClientSocketEvents(const pollfd_t& poll_fd);
		void updatePollFdForWrite(int fd);
		void updatePollFdForRead(int fd);
		void addServerConfig(ServerDirectives& serverConfig);
        void closeServer(void);
		// whichconfig(host name from request)
	private:
		const int _client_max_body_size;
		std::vector<ServerDirectives> _server_config;
		std::vector<pollfd_t> _poll_fd_vector;
		std::map<int, Socket*> _socket_map;
};

#endif