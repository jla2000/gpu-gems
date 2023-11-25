#pragma once

#include <functional>
#include <string>

class Watcher {
public:
  Watcher(std::string const &path);
  ~Watcher();

  bool FilesChanged() const;

private:
  int inotify_handle;
  int watch;
};
