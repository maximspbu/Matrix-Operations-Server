#include "../include/session.h"

Session::Session(tcp::socket&& socket): socket_(std::move(socket)) {}


