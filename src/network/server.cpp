#include <iostream>
#include <netdb.h>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace network {

constexpr std::string_view port = "6283";
constexpr int backlog = 10;

class server {
public:
  server() {
    if (start() != 0)
      std::cerr << "sad" << std::endl;
  }

  ~server() {
    if (sockfd != -1)
      close(sockfd);
    if (new_fd != -1)
      close(new_fd);
    if (servinfo != nullptr)
      freeaddrinfo(servinfo);
  }

private:
  int sockfd{-1};
  int new_fd{-1};
  struct addrinfo *servinfo{nullptr};
  struct sockaddr_storage theiraddr;
  socklen_t addr_size = sizeof theiraddr;

  int start() {

    auto error = [](const char *error) {
      perror(error);
      return -1;
    };

    int status;
    struct addrinfo hints{};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(nullptr, port.data(), &hints, &servinfo);

    if (status != 0) {
      std::cerr << "Gai error:" << gai_strerror(status) << std::endl;
      return -1;
    }

    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
                    servinfo->ai_protocol);

    if (sockfd < 0)
      return error("No sockfd");

    int yes = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
      error("setsockopt");

    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
      return error("Could not bind");

    if (listen(sockfd, backlog) < 0)
      return error("Could not listen");

    new_fd = accept(sockfd, (struct sockaddr *)&theiraddr, &addr_size);

    return 0;
  };
};

} // namespace network
