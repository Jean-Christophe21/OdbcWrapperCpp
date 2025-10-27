#include "OdbcError.h"

OdbcError::OdbcError(const std::string& message) : std::runtime_error(message)
{
}