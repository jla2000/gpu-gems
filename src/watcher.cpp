#include <stdexcept>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <unistd.h>

#include "watcher.h"

Watcher::Watcher(std::string const &path) {
  inotify_handle = inotify_init();
  if (inotify_handle < 0) {
    throw std::runtime_error("Failed to start watcher");
  }

  watch = inotify_add_watch(inotify_handle, path.c_str(), IN_MODIFY);
  if (watch < 0) {
    throw std::runtime_error("Failed to watch shaders");
  }
}

Watcher::~Watcher() {
  inotify_rm_watch(inotify_handle, watch);
  close(inotify_handle);
}

bool Watcher::Poll() const {
  pollfd poll_fd{inotify_handle, POLLIN, 0};
  if (poll(&poll_fd, 1, 0) > 0) {
    char event_buffer[(sizeof(inotify_event) + 16) * 16];
    int bytes_read = read(inotify_handle, event_buffer, sizeof(event_buffer));
    return bytes_read > 0;
  }
  return false;
}
